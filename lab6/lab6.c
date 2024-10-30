#define _GNU_SOURCE
#define _ATFILE_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX(x,y) (x) > (y) ? (x) : (y)
#define _GNU_SOURCE

FILE* output;

int getinfo(char cwd_name[64]){
    printf("Current working directory: %s\n", cwd_name);
    fprintf(output, "Current working directory: %s\n", cwd_name);
    char sdir[16][64];
    DIR* cwd;
    cwd = opendir(cwd_name);
    int max = 0, sum = 0, j = -1, amount = 0;
    char max_name[64];
    memset(max_name, 0, sizeof(max_name));
    rewinddir(cwd);
    struct dirent* dir_info = readdir(cwd);
    strcat(cwd_name, "/");
    while (1) {
        char curfile[64];
        dir_info = readdir(cwd);
        if (!dir_info){
            break;
        }
        struct stat buffer;
        memset(curfile, 0, sizeof(curfile));
        strcpy(curfile, cwd_name);
        strcat(curfile, dir_info->d_name);
        if (dir_info->d_type == DT_REG){
            stat(curfile, &buffer);
            printf("\t%s;", dir_info->d_name);
            fprintf(output, "\t%s;", dir_info->d_name);
            sum += (int)buffer.st_size;
            if (max < (int)buffer.st_size){
                max = (int)buffer.st_size;
                memset(max_name,0,sizeof(max_name));
                strcpy(max_name, dir_info->d_name);
            }
            amount++;
            printf("size = %d\n", (int)buffer.st_size);
            fprintf(output, "size = %d\n", (int)buffer.st_size);
        }else if (dir_info->d_name[0] != '.'){
            strcpy(sdir[++j], curfile);
        }
    }
    closedir(cwd);
    printf("Biggest file: %s\n",max_name);
    printf("Total size: %d\tAmount of files: %d\n\n", sum, amount);
    fprintf(output,"Biggest file: %s\n",max_name);
    fprintf(output,"Total size: %d\tAmount of files: %d\n\n", sum, amount);
    while(j >= 0){
        getinfo(sdir[j--]);
    }
}


int main(int argc, char* argv[]){
    char catalog_name[64] = "./";
    strcat(catalog_name, argv[1]);
    char file_name[64];
    strcpy(file_name, argv[2]);
    output = fopen(file_name, "w");
    getinfo(catalog_name);
    fclose(output);
}

/*
Подсчитать суммарный размер файлов в заданном каталоге (аргумент 1 командной строки) 
и для каждого его подкаталога отдельно. Вывести на консоль и в файл (аргумент 2 командной строки)
название подкаталога, количество файлов в нём, суммарный размер файлов, имя файла с наибольшим размером.
*/