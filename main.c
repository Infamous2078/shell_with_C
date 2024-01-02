#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/wait.h>

#define true 1
#define false 0
#define bool int

typedef int error_code;

#define ERROR (-1)
#define SUCCESS 0
#define HAS_ERROR(code) ((code) < 0)
#define NULL_TERMINATOR '\0'
#define MAX_LENGTH 200

//function that is used to split the command into an array with a classic algorithm using strtok()
error_code splitString(char *string, char ***cmdArgs) {
    char **strArray = (char **) malloc(sizeof(char *) * 20);
    if (strArray == NULL)
        return ERROR;
    strArray[0] = strtok(string, " ");
    int i = 0;
    while (strArray[i] != NULL) {
        char *c = strtok(NULL, " ");
        strArray[++i] = c;
    }
    *cmdArgs = strArray;
    return SUCCESS;
}

//fucntion that is used to execute the bash command
//rnArray contains integers that indicates the number of time each cmd has to be executed
//cmdCounter indicates the number of commands that are contained in the bash string
//cmdExists is the out argument to indicate if the command exists
error_code execCmd(char **cmd, bool *cmdExists, const int cmdCounter, const int *rnArray) {
    for (int i = 0; i < rnArray[cmdCounter]; i++) {
        pid_t pid = fork();
        if (pid == -1)
            return ERROR;   //unable to fork
        //variable that indicates if the child process executed the cmd(exists or not)
        int status;
        if (pid == 0) {
            int code = execvp(cmd[0], cmd);
            if (HAS_ERROR(code)) {
                printf("%s: command not found\n", cmd[0]);
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        else {
            wait(&status);
            if (status == 0)
                *cmdExists = true;
            else
                *cmdExists = false;
        }
    }
    //considering a cmd executed 0 times
    if (rnArray[cmdCounter] == 0)
        *cmdExists = true;
    return SUCCESS;
}

error_code readline(char **out, bool *also, int **rnArray) {
    char *line = malloc(sizeof(char) * MAX_LENGTH);
    if (line == NULL)
        return ERROR; //terminate du to memory issue
    fgets(line, MAX_LENGTH, stdin);
    unsigned long strLength = strlen(line);
    if (line[strLength - 2] == '&') {
        line[strLength - 3] = NULL_TERMINATOR; //we relace the space before & by '\0' to shrink the cmd
        *also = true;
    }
    else {
        line[strLength - 1] = NULL_TERMINATOR;
        *also = false;
    }
    int *intArray = malloc(sizeof(int) * 10); // we assume that there is not more than 10 cmds
    char *newLine = malloc(sizeof(char) * MAX_LENGTH);
    int intArrIndex = 0;
    int newLineArrIndex = 0;
    for (int i = 0;; i++) {
        if ((line[i] == 'r' && line[i + 1] >= '0' && line[i + 1] <= '9')
            && (line[i + 2] == '(' || line[i + 3] == '(' || line[i + 4] == '(') || line[i + 5] == '(') {
            i++;
            char *n = malloc(sizeof(char) * 4);// we assume that a cmd is not going to be executed more than 9999 times
            for (int k = 0; k < 4; k++) {
                if (line[i] >= '0' && line[i] <= '9')
                    n[k] = line[i++];
                else
                    break;
            }
            int number = (int) strtol(n, NULL, 10);
            if (number != 0)
                intArray[intArrIndex] = number;
            else
                intArray[intArrIndex] = -1;
            free(n);
        }
        else if (line[i] != '(' && line[i] != ')') {
            if (line[i] == '&' || line[i] == '|' || line[i] == NULL_TERMINATOR) {
                if (intArray[intArrIndex] == 0) {
                    intArray[intArrIndex++] = 1;
                }
                else if (intArray[intArrIndex] == -1) {
                    intArray[intArrIndex++] = 0;
                }
                else {
                    intArrIndex++;
                }
                newLine[newLineArrIndex++] = line[i];
                newLine[newLineArrIndex++] = line[i + 1];
                i += 2;
            }
            newLine[newLineArrIndex++] = line[i];   //we copy the line to newLine without rN() cmd
            if (line[i] == NULL_TERMINATOR)
                break;
        }
    }
    free(line);
    *rnArray = intArray;
    *out = newLine;
    return SUCCESS;
}

int main(void) {
    char *line;
    char **cmdArray;
    int *rnArray;
    bool also;
    pid_t pid;
    while (true) {
        if (HAS_ERROR(readline(&line, &also, &rnArray)))
            exit(-1);

        if (strcmp(line, "exit") == 0) {
            if (pid != 0) {
                wait(NULL);
            }
            free(rnArray);
            free(line);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(line, "") == 0)
            continue;

        if (also)
            pid = fork();
        //the case where it's the parent process and the child process
        //is still executing a cmd it continues executing other cmds
        if (!also || (also && pid == 0)) {
            splitString(line, &cmdArray);
            int i = 0;
            int j = 0;
            //keeping trace of the number of cmd contained in the string
            int cmdCounter = 0;
            //indicates if the command before and can be executed
            bool execAnd = true;
            //indicates if the command before or can be executed
            bool execOr = true;
            //indicates if the cmd is found in bash
            bool cmdExists;
            //the one by one cmd executed
            char *cmd[5];

            while (true) {
                //final cmd
                if (cmdArray[i] == NULL) {
                    //the last cmd is executed whether an and or an or can be executed
                    if (execAnd || execOr) {
                        cmd[j] = NULL;
                        execCmd(cmd, &cmdExists, cmdCounter, rnArray);
                    }
                    break;
                }
                else if (strcmp(cmdArray[i], "&&") == 0) {
                    if (execAnd) {
                        cmd[j] = NULL;
                        execCmd(cmd, &cmdExists, cmdCounter, rnArray);
                        if (!cmdExists) {
                            execAnd = false;
                            execOr = false;
                        }
                    }
                    else {
                        execOr = true;
                        execAnd = true;
                    }
                    j = 0;
                    i++;
                    cmdCounter++;
                }
                else if (strcmp(cmdArray[i], "||") == 0) {
                    if (execOr) {
                        cmd[j] = NULL;
                        execCmd(cmd, &cmdExists, cmdCounter, rnArray);
                        if (cmdExists) {
                            execAnd = false;
                            execOr = false;
                        }
                    }
                    else
                        execOr = true;
                    j = 0;
                    i++;
                    cmdCounter++;
                }
                //copying the actual cmd that is going to be executed from the cmds array to pass it to execCmd() function
                else
                    cmd[j++] = cmdArray[i++];
            }
            //if the child process has finished executing in the background we terminate it
            if (also && pid == 0) {
                free(cmdArray);
                free(rnArray);
                free(line);
                exit(EXIT_SUCCESS);
            }
            free(cmdArray);
        }
        free(line);
        free(rnArray);
    }
}