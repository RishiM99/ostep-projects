#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

char *initialPath = "/bin";
char **paths = &initialPath;
int numberOfPaths = 1;

void writeError() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

// Counts how many tokens are in a line like "  ls -la  " (2 in that case)
int countTokens(char *line) {
    int count = 0;
    bool inToken = false;
    for (int i = 0; i < strlen(line); i++) {
        if (!inToken) {
            if (strncmp(line + i, " ", 1) != 0 && strncmp(line + i, "\t", 1) != 0) {
                inToken = true;
                count++;
            }
        }
        else {
            if (strncmp(line + i, " ", 1) == 0 || strncmp(line + i, "\t", 1) == 0) {
                inToken = false;
            }
        }
    }

    return count;
}


char** splitTokens(char *line, int tokenCount) {
    char** tokens = (char **) malloc(sizeof(char *) * tokenCount); 
    if (tokens == NULL) {
        writeError();
        return NULL;
    }
    int tokenIndex = 0;
    int i = 0;
    while (i < strlen(line)) {
        if (strncmp(line + i, " ", 1) != 0 && strncmp(line + i, "\t", 1) != 0 && strncmp(line + i, "\n", 1) != 0) {
            int j = i;
            while (j < strlen(line) && strncmp(line + j, " ", 1) != 0 && strncmp(line + j, "\t", 1) != 0 && strncmp(line + j, "\n", 1) != 0) {
                j++;
            }
            int substringLength = j - i;
            char *substring = (char *) malloc((substringLength + 1) * sizeof(char));
            if (substring == NULL) {
                writeError();
                return NULL;
            }
            strncpy(substring, line + i, substringLength);
            substring[substringLength] = '\0';
            tokens[tokenIndex] = substring;
            tokenIndex++;
            i = j;
        } else {
            i++;
        }
    }
    return tokens;
}

char** addNullToEndOfTokensList(char** tokens, int tokenCount) {
    char** reallocedTokens = (char **) realloc(tokens, sizeof(char *) * (tokenCount + 1));
    if (reallocedTokens == NULL) {
        writeError();
        return NULL;
    }
    return reallocedTokens;
}

void freeTokens(char** tokens, int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        free(tokens[i]);
    }

    free(tokens);
}

char* getExecutableUsingPaths(char* commandName) {
    for (int i = 0; i < numberOfPaths; i++) {
        char* currentPathPlusCommand = strdup(paths[i]);
        strcat(currentPathPlusCommand, "/");
        strcat(currentPathPlusCommand, commandName);
        if (access(currentPathPlusCommand, X_OK) == 0) {
            return currentPathPlusCommand;
        }
    }
    return NULL;
}

void processLine(char *line, size_t len) {
    int status;
    printf("PARENT PID: %d\n", getpid());
    pid_t childPID = fork(); 
    if (childPID < 0) {
        // Fork failed
        writeError(); 
        return;
    } else if (childPID == 0) {
        // Child process
        printf("CHILD PID: %d\n", getpid());
        int tokenCount = countTokens(line);
        char **tokens = splitTokens(line, tokenCount);
        if (tokens == NULL) {
            return;
        }
        char *commandName = tokens[0];
        if (strcmp(commandName, "exit") == 0 || strcmp(commandName, "cd") == 0 || strcmp(commandName, "path") == 0) {
            // Built in command
            if (strcmp(commandName, "exit") == 0) {
                if (tokenCount > 1) {
                    writeError();
                    return;
                }
                printf("IN EXIT\n");
                exit(0);
            } else if (strcmp(commandName, "cd") == 0) {
                if (tokenCount != 2) {
                    writeError();
                    return;
                }
                char* newDirectory = tokens[1];
                int result = chdir(newDirectory);
                if (result != 0) {
                    writeError();
                    return;
                }
            } else if (strcmp(commandName, "path") == 0) {
                numberOfPaths = tokenCount - 1;

                if (numberOfPaths != 0) {
                    paths = tokens + 1;
                }
            }
        } else {
            char* executableWithPath = getExecutableUsingPaths(commandName);
            if (executableWithPath == NULL) {
                writeError();
                return;
            }
            tokens[0] = executableWithPath;
            tokens = addNullToEndOfTokensList(tokens, tokenCount);
            if (tokens == NULL) {
                return;
            }
            execv(executableWithPath, tokens);
        }
        freeTokens(tokens, tokenCount);
    } else {
        // Parent process
        waitpid(childPID, &status, 0);
    }
}

int main(int argc, char *argv[]) {  
    // Interactive mode
    if (argc == 1) {
        while (true) {
            char *line = NULL;
            size_t len = 0;

            printf("wish> ");

            if (getline(&line, &len, stdin) == -1) {
                printf("ERROR AT GETLINE\n");
                writeError();
            }

            processLine(line, len);
            printf("PID: %d\n", getpid());

            free(line);
        }
    } else if (argc == 2) {
        char* fileName = argv[1];
        FILE* file = fopen(fileName, "r");

        if (file == NULL) {
            writeError();
            exit(1);
        }

        char *line = NULL;
        size_t len = 0;

        while (getline(&line, &len, file) != -1) {
            processLine(line, len);
            free(line);
            line = NULL;
            len = 0;
        }

        if (ferror(file) != 0) {
            fclose(file);
            writeError();
            exit(1);
        }

        fclose(file);
    } else {
        writeError();
        exit(1);
    }

    return 0;
}