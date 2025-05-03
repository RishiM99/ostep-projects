#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv[]) {  
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    FILE *firstFp = fopen(argv[1], "r");
    if (firstFp == NULL) {
        printf("wzip: cannot open file\n");
        exit(1);
    }

    char currChar = (char) fgetc(firstFp); 
    int32_t currCharCount = 0;

    fclose(firstFp);

    for (int i = 1; i < argc; i++) {
        char *fileName = argv[i];

        FILE *fp = fopen(fileName, "r");
        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            exit(1);
        }

        char c = (char) fgetc(fp);

        while (c != EOF) {
            if (c == currChar) {
                currCharCount++;
                c = (char) fgetc(fp);
            }
            else {
                fwrite(&currCharCount, sizeof(int32_t), 1, stdout);
                fwrite(&currChar, sizeof(char), 1, stdout);
                currChar = c; 
                currCharCount = 0;
            }
        }

        fclose(fp);
    }

    fwrite(&currCharCount, sizeof(int32_t), 1, stdout);
    fwrite(&currChar, sizeof(char), 1, stdout);

    return 0;
}