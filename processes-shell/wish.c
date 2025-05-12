#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *path = "/bin";

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


char** splitTokens(char *line) {
    int tokenCount = countTokens(line);
    char** tokens = (char **) malloc(sizeof(char *) * (tokenCount + 1)); 
    int tokenIndex = 0;
    int i = 0;
    while (i < strlen(line)) {
        if (strncmp(line + i, " ", 1) != 0 && strncmp(line + i, "\t", 1) != 0) {
            int j = i;
            while (j < strlen(line) && strncmp(line + j, " ", 1) != 0 && strncmp(line + j, "\t", 1) != 0) {
                j++;
            }
            int substringLength = j - i;
            char *substring = (char *) malloc((substringLength + 1) * sizeof(char));
            strncpy(substring, line + i, substringLength);
            substring[substringLength] = '\0';
            tokens[tokenIndex] = substring;
            tokenIndex++;
            i = j;
        } else {
            i++;
        }
    }
    tokens[tokenCount] = NULL;
    return tokens;
}

void processLine(char *line, size_t len) {
    int rc = fork(); 
    if (rc < 0) {
        // Fork failed
        writeError(); 
        exit(1);
    } else if (rc == 0) {
        // Child process
        char **tokens = splitTokens(line);
        char *commandName = tokens[0];
        if (strcmp(commandName, "exit") == 0 || strcmp(commandName, "cd") == 0 || strcmp(commandName, "path") == 0) {
            // Built in command
        } else {
            execv(commandName, tokens);
        }
    } else {
        // Parent process
        wait(NULL);
    }

}

int main(int argc, char *argv[]) {  
    // Interactive mode
    if (argc == 1) {
        while (true) {
            printf("wish> ");
            char *line = NULL;
            size_t len = 0;

            if (getline(&line, &len, stdin) == -1) {
                writeError();
            }

            printf("%s", line);

            processLine(line, len);
        }
    }

    return 0;
}