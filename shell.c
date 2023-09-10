#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_LEN 1024
#define MAX_ARGS 200


int execute_commands(char *cmds[], int n_cmds);

int main(int argc, char *args[])
{
    char line[BUFFER_LEN];

    while (1)
    {
        printf("jsh$ "); // print shell prompt

        if (!fgets(line, BUFFER_LEN, stdin)) // get command and put it in line
        {
            break; // if user hits CTRL+D or CTRL+C break
        }

        // remove newline character from input
        size_t length = strlen(line);
        if (line[length - 1] == '\n')
            line[length - 1] = '\0';

        if (strcmp(line, "exit") == 0) // check if command is exit
        {
            break;
        }
        char *cmds[BUFFER_LEN];
        int n_cmds = 0;

        // split commands on pipes
        char *token = strtok(line, "|");
        while (token != NULL)
        {
            // trim whitespace from beginning and end of token
            while (*token == ' ')
                token++;
            char *end = token + strlen(token) - 1;
            while (*end == ' ')
                end--;
            *(end + 1) = '\0';

            cmds[n_cmds++] = token;

            token = strtok(NULL, "|");
        }

        execute_commands(cmds, n_cmds);
    }

    return 0;
}

int execute_commands(char *cmds[], int n_cmds) {
    int pipes[n_cmds - 1][2];
    int i;

    // Create all pipes
    for (i = 0; i < n_cmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            fprintf(stderr, "Error: pipe creation failed\n");
            exit(errno);
        }
    }
    int processPidArr[n_cmds];
    for (i = 0; i < n_cmds; i++) {
        int rc = fork();
        if (rc < 0) {
            fprintf(stderr, "Error: fork failed\n");
            exit(errno);
        } else if (rc == 0) {
            // child process
            if (i == 0 && n_cmds > 1) { // first command with more than one command
                close(pipes[i][0]); // close read end of first pipe
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                    fprintf(stderr, "Error: dup2 failed for stdout redirection in command \n");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i][1]); // close write end of first pipe
            } else if (i == n_cmds - 1 && n_cmds > 1) { // last command with more than one command
                close(pipes[i - 1][1]); // close write end of previous pipe
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0) {
                    fprintf(stderr, "Error: dup2 failed for stdin redirection in command \n");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i - 1][0]); // close read end of previous pipe
            } else if (n_cmds > 2) { // middle commands with more than two commands
                close(pipes[i - 1][1]); // close write end of previous pipe
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0) {
                    fprintf(stderr, "Error: dup2 failed for stdin redirection in command \n");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i - 1][0]); // close read end of previous pipe
                close(pipes[i][0]); // close read end of current pipe
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                    fprintf(stderr, "Error: dup2 failed for stdout redirection in command \n");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i][1]); // close write end of current pipe
            }

            char *args[BUFFER_LEN];
            int j = 0;
            char *token = strtok(cmds[i], " ");
            while (token != NULL) {
                args[j] = token;
                token = strtok(NULL, " ");
                j++;
            }
            args[j] = NULL;
            if (execvp(args[0], args) < 0) {
                //Error Handling
                if(((args[0][0] == '.' && args[0][1] == '/') || args[0][0] == '/')){
                    fprintf(stderr, "Error: no such file or directory: %s\n", args[0]);
                } else {
                   fprintf(stderr, "Error: command not found: %s\n", args[0]); 
                };
                int err = 127;
                exit(err);
            }
        } else {
            // parent process
            if (i > 0 && n_cmds > 1) { // close read end of previous pipe
                close(pipes[i - 1][0]);
            }
            if (i < n_cmds - 1 && n_cmds > 1) { // close write end of current pipe
                close(pipes[i][1]);
            }
            processPidArr[i] = rc;
        }
    }

    // close all pipes
    for (i = 0; i < n_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for all child processes to finish
    int status, exit_status = 0;
    for (i = 0; i < n_cmds; i++) {
        waitpid(processPidArr[i], &status, 0);
    }
    exit_status = WEXITSTATUS(status);
    printf("jsh status %d\n", exit_status);
    return 0;
}
