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
#include <dirent.h>
#include <stdbool.h>
#include <pthread.h>

int min, max;

int maxp, curp = 1;

long long mint, maxt;

FILE* output;

void* getinfo(void* param){
    char cwd_name[64] = {0};
    strcpy(cwd_name, (char*) param);
    printf("%lu: Current working directory: %s\n",pthread_self(), cwd_name);
    fprintf(output, "%lu: Current working directory: %s\n",pthread_self(), cwd_name);
    DIR* cwd;
    cwd = opendir(cwd_name);
    int j = -1, amount = 0;
    struct dirent* dir_info = readdir(cwd);
    strcat(cwd_name, "/");
    while (1) {
        char curfile[64];
        dir_info = readdir(cwd);
        if (!dir_info){
            break;
        }
        struct statx buffer;

        memset(curfile, 0, sizeof(curfile));
        strcpy(curfile, cwd_name);
        strcat(curfile, dir_info->d_name);
        
        if (dir_info->d_type == DT_REG){
            statx(AT_FDCWD,curfile, AT_STATX_SYNC_AS_STAT, STATX_SIZE | STATX_BTIME, &buffer);
            amount++;
            if(buffer.stx_size >= min && buffer.stx_size <= max 
            && buffer.stx_btime.tv_sec >= mint && buffer.stx_btime.tv_sec <= maxt){
                printf("\t%lu:%s;\t",pthread_self(), curfile);
                fprintf(output,"\t%lu:%s;\t",pthread_self(), curfile);
                time_t btime_t = buffer.stx_btime.tv_sec;
                struct tm *btime = localtime(&btime_t);
                printf("%lu: size = %d bytes\t birth time: %d:%d  %d.%d.%d\n", pthread_self(), (int)buffer.stx_size,
                btime->tm_hour, btime->tm_min, btime->tm_mday, btime->tm_mon+1, 1900+btime->tm_year);
                fprintf(output,"%lu: size = %d bytes\t birth time: %d:%d  %d.%d.%d\n", pthread_self(), (int)buffer.stx_size,
                btime->tm_hour, btime->tm_min, btime->tm_mday, btime->tm_mon+1, 1900+btime->tm_year);
            }
        }else if (dir_info->d_type == DT_DIR && dir_info->d_name[0] != '.'){
            if(curp < maxp){
                char newdir[64] = {0};
                strcpy(newdir, curfile);
                curp++;
                pthread_t tid;
                pthread_create(&tid, NULL, getinfo, (void* )newdir);
                printf("%lu: Created new thread, tid: %lu\n",pthread_self(), tid);
                fprintf(output,"%lu: Created new thread, tid: %lu\n",pthread_self(), tid);
                pthread_join(tid, NULL);
            }else{
                getinfo(curfile);
            }
        }
    }
    printf("%lu: Amount of files in %s : %d\n",pthread_self(),cwd_name, amount);
    fprintf(output,"%lu: Amount of files in %s : %d\n",pthread_self(),cwd_name, amount);
    closedir(cwd);
    curp--;
}

int main(int argc, char *argv[]) {
    
    char catalog_name[64] = "./";
    strcat(catalog_name, argv[1]);

    output = fopen(argv[2], "w");
    
    min = atoi(argv[3]);
    max = atoi(argv[4]);

    struct tm temp = {0};

    temp.tm_mday = atoi(strtok(argv[5], "."));
    temp.tm_mon = atoi(strtok(NULL, ".")) - 1;
    temp.tm_year = atoi(strtok(NULL, ".")) - 1900;
    mint = mktime(&temp);

    temp.tm_mday = atoi(strtok(argv[6], "."));
    temp.tm_mon = atoi(strtok(NULL, ".")) - 1;
    temp.tm_year = atoi(strtok(NULL, ".")) - 1900;
    maxt = mktime(&temp);

    printf("Enter max amount of threads: ");
    scanf("%d", &maxp);

    getinfo(catalog_name);
    
    fclose(output);
    
    return 0;
}

//gcc -lpthread -o threads threads.c

// ./threads dir output 0 20 20.11.2023 25.11.2023