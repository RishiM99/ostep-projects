#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct LinkedListNode {
    char* line; 
    struct LinkedListNode* prev;
} LinkedListNode;

int main(int argc, char *argv[]) {  
    char *line = NULL;
    size_t lineSize = 0;
    FILE *inputStream = NULL;
    FILE *outputStream = NULL;

    if (argc == 1) {
        inputStream = stdin;
        outputStream = stdout;
    } else if (argc == 2) {
        char *inputFileName = argv[1];
        inputStream = fopen(inputFileName, "r");

        if (inputStream == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", inputFileName);
            exit(1);
        }

        outputStream = stdout;
    } else if (argc == 3) {
        char *inputFileName = argv[1];
        char *outputFileName = argv[2];

        struct stat stat1, stat2;

        stat(inputFileName, &stat1);
        stat(outputFileName, &stat2);
    
        if (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }

        inputStream = fopen(inputFileName, "r");
        if (inputStream == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", inputFileName);
            exit(1);
        }

        outputStream = fopen(outputFileName, "w");
        if (outputStream == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", outputFileName);
            exit(1);
        }
    } else {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    LinkedListNode *currentNode = (LinkedListNode*) malloc(sizeof(LinkedListNode));
    if (currentNode == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    currentNode->line = NULL;
    currentNode->prev = NULL;

    while (getline(&line, &lineSize, inputStream) != -1) {
        LinkedListNode *nextNode = (LinkedListNode*) malloc(sizeof(LinkedListNode));
        if (nextNode == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        size_t lineLength = strlen(line);
        char *nodeLine = (char *) malloc(lineLength);
        if (nodeLine == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        strncpy(nodeLine, line, lineLength);

        nextNode->line = nodeLine; 
        nextNode->prev = currentNode;
        currentNode = nextNode;
    }

    while (currentNode->prev != NULL) {
        fprintf(outputStream, "%s", currentNode->line);
        currentNode = currentNode->prev;
    }

    if (argc == 2) {
        fclose(inputStream);
    }

    if (argc == 3) {
        fclose(inputStream);
        fclose(outputStream);
    }

    return(0);
}
