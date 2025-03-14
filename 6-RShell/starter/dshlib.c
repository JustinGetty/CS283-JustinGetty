#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

static void handle_redirection(cmd_buff_t *cmd)
{
    char *in_file = NULL;
    char *out_file = NULL;
    bool append = false;
    int new_argc = 0;
    for (int i = 0; cmd->argv[i] != NULL; i++)
    {
        if (strcmp(cmd->argv[i], "<") == 0)
        {
            if (cmd->argv[i + 1] != NULL)
            {
                in_file = cmd->argv[i + 1];
                i++;
            }
        }
        else if (strcmp(cmd->argv[i], ">") == 0)
        {
            if (cmd->argv[i + 1] != NULL)
            {
                out_file = cmd->argv[i + 1];
                append = false;
                i++;
            }
        }
        else if (strcmp(cmd->argv[i], ">>") == 0)
        {
            if (cmd->argv[i + 1] != NULL)
            {
                out_file = cmd->argv[i + 1];
                append = true;
                i++;
            }
        }
        else
        {
            cmd->argv[new_argc++] = cmd->argv[i];
        }
    }
    cmd->argv[new_argc] = NULL;
    if (in_file != NULL)
    {
        int fd = open(in_file, O_RDONLY);
        if (fd < 0)
        {
            perror("open");
            exit(ERR_EXEC_CMD);
        }
        if (dup2(fd, STDIN_FILENO) < 0)
        {
            perror("dup2");
            exit(ERR_EXEC_CMD);
        }
        close(fd);
    }
    if (out_file != NULL)
    {
        int fd;
        if (append)
            fd = open(out_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            perror("open");
            exit(ERR_EXEC_CMD);
        }
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2");
            exit(ERR_EXEC_CMD);
        }
        close(fd);
    }
}

void trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
        str++;
    if (*str == '\0')
        return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';
}

int parse_command_line(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || clist == NULL)
        return WARN_NO_CMDS;
    trim_whitespace(cmd_line);
    if (strlen(cmd_line) == 0)
        return WARN_NO_CMDS;
    clist->num = 0;
    char *saveptr1 = NULL;
    char *pipe_token = strtok_r(cmd_line, PIPE_STRING, &saveptr1);
    while (pipe_token != NULL)
    {
        trim_whitespace(pipe_token);
        if (strlen(pipe_token) == 0)
        {
            pipe_token = strtok_r(NULL, PIPE_STRING, &saveptr1);
            continue;
        }
        if (clist->num >= CMD_MAX)
            return ERR_TOO_MANY_COMMANDS;
        cmd_buff_t *cmd = &clist->commands[clist->num];
        cmd->_cmd_buffer = pipe_token;
        cmd->argc = 0;
        char *ptr = pipe_token;
        while (*ptr)
        {
            while (*ptr && isspace((unsigned char)*ptr))
                ptr++;
            if (!*ptr)
                break;
            char *token;
            if (*ptr == '"')
            {
                ptr++;
                token = ptr;
                while (*ptr && *ptr != '"')
                    ptr++;
                if (*ptr == '"')
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            else
            {
                token = ptr;
                while (*ptr && !isspace((unsigned char)*ptr))
                    ptr++;
                if (*ptr)
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            if (cmd->argc < CMD_ARGV_MAX - 1)
                cmd->argv[cmd->argc++] = token;
            else
                return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        cmd->argv[cmd->argc] = NULL;
        clist->num++;
        pipe_token = strtok_r(NULL, PIPE_STRING, &saveptr1);
    }
    if (clist->num == 0)
        return WARN_NO_CMDS;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    if (!cmd_buff)
        return -1;
    if (cmd_line == NULL)
    {
        fprintf(stderr, "Error: cmd_line is NULL\n");
        return ERR_MEMORY;
    }
    cmd_buff->_cmd_buffer = cmd_line;
    cmd_buff->argc = 0;
    char *ptr = cmd_line;
    while (*ptr)
    {
        while (*ptr && isspace((unsigned char)*ptr))
            ptr++;
        if (!*ptr)
            break;
        char *token;
        if (*ptr == '"')
        {
            ptr++;
            token = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            if (*ptr == '"')
            {
                *ptr = '\0';
                ptr++;
            }
        }
        else
        {
            token = ptr;
            while (*ptr && !isspace((unsigned char)*ptr))
                ptr++;
            if (*ptr)
            {
                *ptr = '\0';
                ptr++;
            }
        }
        if (strcmp(token, "<") == 0)
        {
            while (*ptr && isspace((unsigned char)*ptr))
                ptr++;
            if (!*ptr)
                break;
            char *file;
            if (*ptr == '"')
            {
                ptr++;
                file = ptr;
                while (*ptr && *ptr != '"')
                    ptr++;
                if (*ptr == '"')
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            else
            {
                file = ptr;
                while (*ptr && !isspace((unsigned char)*ptr))
                    ptr++;
                if (*ptr)
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            cmd_buff->argv[cmd_buff->argc++] = token;
            cmd_buff->argv[cmd_buff->argc++] = file;
            continue;
        }
        else if (strcmp(token, ">") == 0)
        {
            while (*ptr && isspace((unsigned char)*ptr))
                ptr++;
            if (!*ptr)
                break;
            char *file;
            if (*ptr == '"')
            {
                ptr++;
                file = ptr;
                while (*ptr && *ptr != '"')
                    ptr++;
                if (*ptr == '"')
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            else
            {
                file = ptr;
                while (*ptr && !isspace((unsigned char)*ptr))
                    ptr++;
                if (*ptr)
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            cmd_buff->argv[cmd_buff->argc++] = token;
            cmd_buff->argv[cmd_buff->argc++] = file;
            continue;
        }
        else if (strcmp(token, ">>") == 0)
        {
            while (*ptr && isspace((unsigned char)*ptr))
                ptr++;
            if (!*ptr)
                break;
            char *file;
            if (*ptr == '"')
            {
                ptr++;
                file = ptr;
                while (*ptr && *ptr != '"')
                    ptr++;
                if (*ptr == '"')
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            else
            {
                file = ptr;
                while (*ptr && !isspace((unsigned char)*ptr))
                    ptr++;
                if (*ptr)
                {
                    *ptr = '\0';
                    ptr++;
                }
            }
            cmd_buff->argv[cmd_buff->argc++] = token;
            cmd_buff->argv[cmd_buff->argc++] = file;
            continue;
        }
        if (cmd_buff->argc < CMD_ARGV_MAX - 1)
            cmd_buff->argv[cmd_buff->argc++] = token;
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

int exec_cmd(cmd_buff_t *cmd)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(ERR_EXEC_CMD);
    }
    if (pid == 0)
    {
        handle_redirection(cmd);
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(ERR_EXEC_CMD);
    }
    else
    {
        int status;
        wait(&status);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        return ERR_EXEC_CMD;
    }
}

int execute_pipeline(command_list_t *clist)
{
    if (clist == NULL || clist->num == 0)
    {
        fprintf(stderr, "No commands to execute.\n");
        return WARN_NO_CMDS;
    }
    int num_cmds = clist->num;
    int prev_pipe_fd = -1;
    pid_t last_pid = -1;
    for (int i = 0; i < num_cmds; i++)
    {
        int pipefd[2] = {-1, -1};
        if (i < num_cmds - 1)
        {
            if (pipe(pipefd) < 0)
            {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return ERR_EXEC_CMD;
        }
        else if (pid == 0)
        {
            if (prev_pipe_fd != -1)
            {
                if (dup2(prev_pipe_fd, STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
                close(prev_pipe_fd);
            }
            if (i < num_cmds - 1)
            {
                close(pipefd[0]);
                if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    exit(ERR_EXEC_CMD);
                }
                close(pipefd[1]);
            }
            handle_redirection(&clist->commands[i]);
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
        else
        {
            if (prev_pipe_fd != -1)
                close(prev_pipe_fd);
            if (i < num_cmds - 1)
            {
                close(pipefd[1]);
                prev_pipe_fd = pipefd[0];
            }
            if (i == num_cmds - 1)
                last_pid = pid;
        }
    }
    int last_status;
    if (last_pid != -1)
        waitpid(last_pid, &last_status, 0);
    while (wait(NULL) > 0)
        ;
    if (WIFEXITED(last_status))
        return WEXITSTATUS(last_status);
    return ERR_EXEC_CMD;
}

Built_In_Cmds match_command(const char *input);

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (cmd->argc < 2)
        return BI_EXECUTED;
    if (chdir(cmd->argv[1]) != 0)
        perror("chdir");
    return BI_EXECUTED;
}

int exec_local_cmd_loop()
{
    /* If input is piped, clear the first printed line (from dsh_cli.c) */
    if (!isatty(STDIN_FILENO))
    {
        printf("\033[1A\033[2K");
        fflush(stdout);
    }

    char *cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);
    if (cmd_buff == NULL)
    {
        fprintf(stderr, "Error: malloc failed\n");
        exit(ERR_MEMORY);
    }
    int rc = OK, last_rc = OK;
    while (1)
    {
        if (isatty(STDIN_FILENO))
            printf("%s", SH_PROMPT);
        fflush(stdout);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        if (strlen(cmd_buff) == 0 || strspn(cmd_buff, " \t") == strlen(cmd_buff))
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (strcmp(cmd_buff, EXIT_CMD) == 0)
            exit(last_rc);
        if (strchr(cmd_buff, PIPE_CHAR) != NULL)
        {
            command_list_t clist;
            rc = parse_command_line(cmd_buff, &clist);
            if (rc != OK)
            {
                fprintf(stderr, "Error parsing command line.\n");
                continue;
            }
            last_rc = execute_pipeline(&clist);
            if (last_rc != OK)
                fprintf(stderr, "Error executing pipeline.\n");
        }
        else
        {
            cmd_buff_t cmd;
            char single_cmd[SH_CMD_MAX];
            strncpy(single_cmd, cmd_buff, SH_CMD_MAX);
            single_cmd[SH_CMD_MAX - 1] = '\0';
            rc = build_cmd_buff(single_cmd, &cmd);
            if (rc == OK)
            {
                if (strcmp(cmd.argv[0], "cd") == 0)
                    last_rc = exec_built_in_cmd(&cmd);
                else
                    last_rc = exec_cmd(&cmd);
            }
            else if (rc == WARN_NO_CMDS)
                printf("%s", CMD_WARN_NO_CMD);
        }
    }
    free(cmd_buff);
    return last_rc;
}
