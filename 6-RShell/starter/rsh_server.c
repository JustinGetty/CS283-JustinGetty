#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "dshlib.h"
#include "rshlib.h"

int start_server(char *ifaces, int port, int is_threaded)
{
    int svr_socket;
    int rc;
    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0)
        return svr_socket;
    rc = process_cli_requests(svr_socket);
    stop_server(svr_socket);
    return rc;
}

int stop_server(int svr_socket)
{
    return close(svr_socket);
}

int boot_server(char *ifaces, int port)
{
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0)
    {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }
    int enable = 1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ifaces, &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    if (bind(svr_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    if (listen(svr_socket, 20) < 0)
    {
        perror("listen");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    return svr_socket;
}

int process_cli_requests(int svr_socket)
{
    int rc = OK;
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int cli_socket = accept(svr_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (cli_socket < 0)
        {
            perror("accept");
            rc = ERR_RDSH_COMMUNICATION;
            break;
        }
        rc = exec_client_requests(cli_socket);
        close(cli_socket);
        if (rc == OK_EXIT)
            break;
    }
    return rc;
}

int exec_client_requests(int cli_socket)
{
    int io_size;
    char *io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (io_buff == NULL)
        return ERR_RDSH_SERVER;
    while (1)
    {
        memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
        int total = 0;
        while (1)
        {
            io_size = recv(cli_socket, io_buff + total, 1, 0);
            if (io_size <= 0)
            {
                free(io_buff);
                return ERR_RDSH_COMMUNICATION;
            }
            if (io_buff[total] == '\0')
                break;
            total++;
            if (total >= RDSH_COMM_BUFF_SZ - 1)
                break;
        }
        if (strcmp(io_buff, "exit") == 0)
            break; // end client session
        if (strcmp(io_buff, "stop-server") == 0)
        {
            free(io_buff);
            return OK_EXIT;
        }
        char response[1024];
        snprintf(response, sizeof(response), "Executed command: %s\n", io_buff);
        if (send_message_string(cli_socket, response) != OK)
        {
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }
        if (send_message_eof(cli_socket) != OK)
        {
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }
    }
    free(io_buff);
    return OK;
}

int send_message_eof(int cli_socket)
{
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);
    if (sent_len != send_len)
        return ERR_RDSH_COMMUNICATION;
    return OK;
}

int send_message_string(int cli_socket, char *buff)
{
    int len = strlen(buff);
    int sent = send(cli_socket, buff, len, 0);
    if (sent != len)
        return ERR_RDSH_COMMUNICATION;
    return OK;
}

int rsh_execute_pipeline(int socket_fd, command_list_t *clist)
{
    return WARN_RDSH_NOT_IMPL;
}
