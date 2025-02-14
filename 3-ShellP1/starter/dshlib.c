#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
/*
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char* saveptr1;
    char* saveptr2;
    clist->num = 0;

    char* token = strtok_r(cmd_line, "|", &saveptr1);
    while (token != NULL && clist->num < CMD_MAX) {
        while (*token == ' ') token++;

        command_t *cmd = &clist->commands[clist->num];

        char* subtoken = strtok_r(token, " ", &saveptr2);
        if (subtoken != NULL) {
            strncpy(cmd->exe, subtoken, EXE_MAX - 1);
            cmd->exe[EXE_MAX - 1] = '\0';
        } else {
            cmd->exe[0] = '\0';
        }

        char* remaining_args = saveptr2 ? saveptr2 : "";
        strncpy(cmd->args, remaining_args, ARG_MAX - 1);
        cmd->args[ARG_MAX - 1] = '\0';

        clist->num++;
		if (clist->num > CMD_MAX)
		{
			return ERR_TOO_MANY_COMMANDS;
		}
        token = strtok_r(NULL, "|", &saveptr1);
    }

    return 0;
}
*/

void trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == '\0') {
        return;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || clist == NULL) {
        return WARN_NO_CMDS;
    }

    trim_whitespace(cmd_line);
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    clist->num = 0;

    char *saveptr1;
    char *command_token = strtok_r(cmd_line, PIPE_STRING, &saveptr1);

    while (command_token != NULL) {
        trim_whitespace(command_token);
        if (strlen(command_token) == 0) {
            command_token = strtok_r(NULL, PIPE_STRING, &saveptr1);
            continue;
        }

        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        char *saveptr2;
        char *exe_token = strtok_r(command_token, " ", &saveptr2);

        if (exe_token == NULL) {
            command_token = strtok_r(NULL, PIPE_STRING, &saveptr1);
            continue;
        }
        if (strlen(exe_token) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strcpy(clist->commands[clist->num].exe, exe_token);

        clist->commands[clist->num].args[0] = '\0';
        char args_buffer[ARG_MAX];
        args_buffer[0] = '\0';
        int first_arg = 1;
        char *arg_token;

        while ((arg_token = strtok_r(NULL, " ", &saveptr2)) != NULL) {
            if (!first_arg) {
                strcat(args_buffer, " ");
            }
            first_arg = 0;
            strcat(args_buffer, arg_token);

            if (strlen(args_buffer) >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
        }

        strcpy(clist->commands[clist->num].args, args_buffer);

        clist->num++;

        command_token = strtok_r(NULL, PIPE_STRING, &saveptr1);
    }

    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    return OK;
}
