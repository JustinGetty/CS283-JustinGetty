#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dshlib.h"
#include "rshlib.h"

int exec_remote_cmd_loop(char *address, int port)
{
    char *cmd_buff = malloc(1024);
    char *rsp_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!cmd_buff || !rsp_buff)
    {
        perror("malloc");
        return client_cleanup(-1, cmd_buff, rsp_buff, ERR_RDSH_CLIENT);
    }
    int cli_socket = start_client(address, port);
    if (cli_socket < 0)
    {
        perror("start client");
        return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_CLIENT);
    }

    while (1)
    {
        printf("%s", SH_PROMPT);
        fflush(stdout);
        if (fgets(cmd_buff, 1024, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        ssize_t send_len = strlen(cmd_buff) + 1; // include null terminator
        if (send(cli_socket, cmd_buff, send_len, 0) != send_len)
        {
            perror("send");
            return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_COMMUNICATION);
        }
        if (strcmp(cmd_buff, "exit") == 0 || strcmp(cmd_buff, "stop-server") == 0)
            break;
        while (1)
        {
            ssize_t recv_size = recv(cli_socket, rsp_buff, RDSH_COMM_BUFF_SZ, 0);
            if (recv_size < 0)
            {
                perror("recv");
                return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_COMMUNICATION);
            }
            if (recv_size == 0)
            {
                printf("Server closed connection\n");
                return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_COMMUNICATION);
            }
            int is_last_chunk = (rsp_buff[recv_size - 1] == RDSH_EOF_CHAR) ? 1 : 0;
            if (is_last_chunk)
                rsp_buff[recv_size - 1] = '\0'; // Remove EOF marker.
            printf("%.*s", (int)recv_size, rsp_buff);
            if (is_last_chunk)
                break;
        }
    }
    return client_cleanup(cli_socket, cmd_buff, rsp_buff, OK);
}

int start_client(char *server_ip, int port)
{
    int cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0)
    {
        perror("socket");
        return ERR_RDSH_CLIENT;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }
    if (connect(cli_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }
    return cli_socket;
}

int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc)
{
    if (cli_socket > 0)
        close(cli_socket);
    free(cmd_buff);
    free(rsp_buff);
    return rc;
}
