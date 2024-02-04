#include "commit.c"

void status_here(char *path){ // show status for path 
    chdir(repo_path);
    FILE *t=fopen(cnct(repo_path, "/tracked"), "r");
    char tmp_path[MAX_ADR_NAME];
    while(fgetS(tmp_path, MAX_ADR_NAME, t) != NULL){
        if(strncmp(path, tmp_path, strlen(path)) == 0){
            char sts = file_status(tmp_path);
            if(sts == '\0')
                continue;
            printf("%s : ", tmp_path+strlen(path)+1);
            if(is_staged(tmp_path))
                printf("+");
            else 
                printf("-");
            printf("%c\n", sts);
        }
    }
}

int show_status(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(argc>3 || ((argc == 3) && strcmp(argv[2], "all"))){
        printf("Invalid command\n");
        return 1;
    }
    char cwd[MAX_ADR_NAME];
    if(argc==2)
        if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    else
        abs_path(cwd, cnct(repo_path, "/.."));
    status_here(cwd);
    return 0;
}