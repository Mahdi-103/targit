#include<stdio.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdbool.h>
#include<string.h>

int is_inited(){
    char adr[1000], tmp_adr[1000];
    if(getcwd(adr, 1000) == NULL) return 1;
    DIR *dir;
    struct dirent *entry;
    while(true){
        dir=opendir(".");
        bool exist=false;
        while((entry=readdir(dir)) != 0){
            printf("\t%s\n", entry->d_name);
            if(entry->d_type==4 && strcmp(entry->d_name, ".targit") == 0){
                exist=true;
                return 1;
            }
        }
        closedir(dir);
        if(getcwd(tmp_adr, 1000) == NULL) return 1;
        printf("fuck :: %s\n", tmp_adr);
        if(strcmp(tmp_adr, "/") == 0) break;
        chdir("..");
    }
    chdir(adr);
    return 0;
}

int init(){
    if(is_inited()){
        perror("The repo is already initialized\n");
        return 1;
    }
    if(mkdir(".targit", 0755) != 0) return 1;
    FILE *f=fopen(".targit/config", "w");
    f=fopen(".targit/staged", "w");
    f=fopen(".targit/commits", "w");
    printf("The repo initialized successfully\n");
    return 0;
}

int main(){
    init();
}