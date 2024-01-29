#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

DIR *where_is_inited(){
    char adr[1000], tmp_adr[1000];
    if(getcwd(adr, 1000) == NULL) exit(1);
    DIR *dir, *res=NULL;
    struct dirent *entry;
    while(true){
        dir=opendir(".");
        while((entry=readdir(dir)) != 0){
            if(entry->d_type==4 && strcmp(entry->d_name, ".targit") == 0){
                res=opendir(".targit");
                break;
            }
        }
        closedir(dir);
        if(res != NULL) break;
        if(getcwd(tmp_adr, 1000) == NULL) exit(1);
        if(strcmp(tmp_adr, "/") == 0) break;
        chdir("..");
    }
    chdir(adr);
    return res;
}

int init(){
    if(where_is_inited() != NULL){
        perror("The repo is already initialized\n");
        return 1;
    }
    if(mkdir(".targit", 0755) != 0) return 1;
    FILE *f=fopen(".targit/config", "w");
    f=fopen(".targit/staged", "w");
    if(mkdir(".targit/commits", 0755) != 0) return 1;
    printf("The repo initialized successfully\n");
    return 0;
}