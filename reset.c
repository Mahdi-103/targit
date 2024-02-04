#include "checkout.c"

int reset_file_opr(char *path){ // cwd is .targit and path is absolute and tracked
    chdir("stage");
    DIR *dir=opendir(".");
    struct dirent *entry;
    char name[10];
    name[0]='\0';
    while((entry = readdir(dir)) != NULL){
        if(is_ok_dir(entry->d_name)){
            if(contains_line(cnct(entry->d_name, "/file_path"), path)){
                memcpy(name, entry->d_name, strlen(entry->d_name)+1);
                break;
            }
        }
    }
    closedir(dir);
    if(strlen(name)) delete(name);
    chdir("..");
    return 0;
}
/*
int reset_file(char *path){ // adds from anywhere and path could be either absolute or not
    char cwd[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(abs_path(abs_p, path) == NULL)   return 1;
    if(in_repo(abs_p) == 0) return 3;
    chdir(repo_path);
    int res=reset_file_opr(abs_p);
    chdir(cwd);
    return res;//returns 3 if out of repo, 2 if not existed, 1 if unexpected error, 0 if ok
}
*/
int reset_dir_file_opr(char *path){ // cwd is .targit and path is absolute
    int res=0;
    char tmp_path[MAX_ADR_NAME];
    FILE *t=fopen("tracked", "r");
    while(fgetS(tmp_path, MAX_ADR_NAME, t) != NULL){
        //printf("fuck :: %s\n%s\n", tmp_path, path);
        if(strncmp(tmp_path, path, strlen(path)) == 0){
            reset_file_opr(tmp_path);
            res=1;
        }
    }
    fclose(t);
    if(res == 0){
        if(access(path, F_OK))
            return 2;
        if(is_dir(path))
            return 3;
        return 4;
    }
    return 0; // 0 if ok, 2 if doesnt exist, 3 if untracked dir, 4 if untracked file
}

int reset_dir_file(char *path){ // from anywhere and path could be either abs or not
    char cwd[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(abs_path(abs_p, path) == NULL)   return 1;
    if(in_repo(abs_p) == 0) return 5;
    chdir(repo_path);
    int res=reset_dir_file_opr(abs_p);
    chdir(cwd);
    return res;
}

int reset(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(ok_head() != 1){
        printf("You must be on HEAD of project to reset\n");
        return 1;
    }
    if(argc == 2){
        printf("Invalid command\n");
        return 1;
    }
    if(strcmp(argv[2], "-undo") == 0){
        return 0;
    }
    int x=2;
    if(strcmp(argv[2], "-f") == 0){
        ++x;
        if(argc==3){
            printf("Please enter file and directory paths\n");
            return 1;
        }
    }
    for(int i=x;i<argc;++i){
        int hh = reset_dir_file(argv[i]);
        if(hh == 2)
            printf("No file or directory which path is %s\n", argv[i]);
        else if(hh == 3)
            printf("No tracked files in %s\n", argv[i]);
        else if(hh == 4)
            printf("%s is untracked\n", argv[i]);
        else if(hh == 5)
            printf("%s is out of repository\n", argv[i]);
        else if(hh == 0)
            printf("%s reset successfully\n", argv[i]);
        else 
            return 1;
    }
    return 0;
}