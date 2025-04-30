#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {  
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    
    char *searchTerm = argv[1];
    char *line = NULL;
    size_t len = 0;

    if (strcmp(argv[1], "") == 0) {
        return 0;
    }

    if (argc == 2) {
        while (getline(&line, &len, stdin) != -1) {
            if (strstr(line, searchTerm) != NULL) {
                printf("%s", line);
            }
        }
    } else {
        for (int i = 2; i < argc; i++) {
            char *fileName = argv[i];

            FILE *fp = fopen(fileName, "r");
            if (fp == NULL) {
                printf("wgrep: cannot open file\n");
                exit(1);
            }

            while (getline(&line, &len, fp) != -1) {
                if (strstr(line, searchTerm) != NULL) {
                    printf("%s", line);
                }
            }

            fclose(fp);
        }
    }

    return 0;
}