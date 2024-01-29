#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include "logs.c"

char *where_is_inited(){
    char adr[MAX_ADR_NAME], tmp_adr[MAX_ADR_NAME];
    if(getcwd(adr, MAX_ADR_NAME) == NULL) exit(1);
    DIR *dir;
    static char res[MAX_ADR_NAME];
    *res='\0';
    struct dirent *entry;
    while(1){
        dir=opendir(".");
        while((entry=readdir(dir)) != 0){
            if(entry->d_type==4 && strcmp(entry->d_name, ".targit") == 0){
                chdir(".targit");
                if(getcwd(res, MAX_ADR_NAME) == NULL) exit(1);
                printf("kristof :: %s\n", res);
                break;
            }
        }
        closedir(dir);
        if(*res != '\0') break;
        if(getcwd(tmp_adr, MAX_ADR_NAME) == NULL) exit(1);
        if(strcmp(tmp_adr, "/") == 0) break;
        chdir("..");
    }
    chdir(adr);
    if(*res != '\0')
        return res;
    return NULL;
}