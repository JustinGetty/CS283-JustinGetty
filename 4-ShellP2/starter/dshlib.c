#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int build_cmd_buff(char* cmd_line, cmd_buff_t* cmd_buff) {
    if (!cmd_buff) return -1;

    cmd_buff->_cmd_buffer = cmd_line;
    cmd_buff->argc = 0;

    char *ptr = cmd_line;
    while (*ptr) {
        while (*ptr && isspace((unsigned char)*ptr))
            ptr++;
        if (!*ptr)
            break;

        char *token;
        if (*ptr == '"') {
            ptr++;
            token = ptr;
            while (*ptr && *ptr != '"')
                ptr++;
            if (*ptr == '"') {
                *ptr = '\0'; 
                ptr++;       
            }
        } else {
            token = ptr;
            while (*ptr && !isspace((unsigned char)*ptr))
                ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
        }

        if (cmd_buff->argc < CMD_ARGV_MAX - 1) {
            cmd_buff->argv[cmd_buff->argc++] = token;
        }
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

int exec_cmd(cmd_buff_t *cmd)
{

    //create the childddd
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(0);
    }
    if (pid == 0)
    {
        execvp(cmd->argv[0], cmd->argv);
        perror("execv");
        exit(1);
    }
    else
    {
        int status;
        wait(&status);
    }

}

Built_In_Cmds match_command(const char *input); 
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    chdir(cmd->argv[1]);
    return 0;
}


int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(sizeof(char) * SH_CMD_MAX);
    int rc = 0;
    cmd_buff_t cmd;

    // clist to be replaced by cmd
    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff) == 0 || strspn(cmd_buff, " \t") == strlen(cmd_buff)) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            exit(0);
        }

        rc = build_cmd_buff(cmd_buff, &cmd);

        if (rc == OK) {
            
            //printf(CMD_OK_HEADER, clist.num);
            cmd_buff_t* cmd_ptr = &cmd;
            if (strcmp(cmd.argv[0], "cd") == 0)
            {
                Built_In_Cmds bi_status = exec_built_in_cmd(cmd_ptr);
            }
            else{
                int status = exec_cmd(cmd_ptr);
            }

        } else if (rc == WARN_NO_CMDS) {
            printf("%s", CMD_WARN_NO_CMD);

    }
    }

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    return OK;
}
