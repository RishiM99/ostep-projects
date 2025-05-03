#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv[]) {  
    if (argc == 1) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        char *fileName = argv[i];

        FILE *fp = fopen(fileName, "r");
        if (fp == NULL) {
            printf("wunzip: cannot open file\n");
            exit(1);
        }

        int32_t count; 
        char character; 

        int countBytesRead = fread(&count, sizeof(int32_t), 1, fp);
        int characterBytesRead = fread(&character, sizeof(char), 1, fp);

        while (countBytesRead == 1 && characterBytesRead == 1) {
            int i = 0;  
            while (i < count) {
                printf("%c", character);
                i++;
            }

            countBytesRead = fread(&count, sizeof(int32_t), 1, fp);
            characterBytesRead = fread(&character, sizeof(char), 1, fp);
        }

        fclose(fp);
    }

    return 0;
}