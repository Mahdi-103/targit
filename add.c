#include "config.c"

int stage_empty(){ // cwd must be .targit
    DIR *dir=opendir("stage");
    struct dirent *entry;
    int n=0;
    while((entry=readdir(dir)) != NULL)
        ++n;
    closedir(dir);
    return (n>2 ? 0 : 1);
}

int add_file_opr(char *path){ // cwd should be .targit and path is absolute
    FILE *f=fopen(path, "r");
    DIR *dir=opendir("stage");
    struct dirent *entry;    
    if(f == NULL){
        if(is_tracked(path)){
            chdir("stage");
            while((entry=readdir(dir)) != NULL){
                if(*entry->d_name == '.')
                    continue;
                char tmp_path[MAX_ADR_NAME];
                FILE *g=fopen(cnct(entry->d_name, "/file_path"), "r");
                fgetS(tmp_path, MAX_ADR_NAME, g);
                if(strcmp(tmp_path, path) == 0){
                    remove(cnct(entry->d_name, "/file"));
                    fclose(g);
                    closedir(dir);
                    chdir("..");
                    return 0;
                }
            }
            return 1;
        }
        else return 2;
    }
    int num=0;
    if(is_tracked(path)){
        chdir("stage");
        while((entry=readdir(dir)) != NULL){
            if(*entry->d_name == '.')
                continue;
            ++num;
            char tmp_path[MAX_ADR_NAME];
            FILE *g=fopen(cnct(entry->d_name, "/file_path"), "r");
            fgetS(tmp_path, MAX_ADR_NAME, g);
            if(strcmp(tmp_path, path) == 0){
                fclose(g);
                g=fopen(cnct(entry->d_name, "/file"), "w");
                flecpy(g, f);
                fclose(f);
                fclose(g);
                closedir(dir);
                chdir("..");
                return 0;
            }
        }
    }
    else{
        num=-2;
        track(path);
        while((entry=readdir(dir)) != NULL)
            ++num;
    }
    char name[100];
    sprintf(name, "%d", num);
    chdir("stage");
    if(mkdir(name, 0755) != 0) return 1;
    chdir(name);
    FILE *g=fopen("file_path", "w");
    fprintf(g, "%s", path);
    fclose(g);
    g=fopen("file", "w");
    flecpy(g, f);
    chdir("../..");
    fclose(g);
    fclose(f);
    closedir(dir);
    return 0;
}

int add_file(char *path){ // adds from anywhere and path could be either absolute or not
    char cwd[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(abs_path(abs_p, path) == NULL)   return 1;
    if(in_repo(abs_p) == 0) return 3;
    chdir(repo_path);
    int res=add_file_opr(abs_p);
    chdir(cwd);
    return res;//returns 3 if out of repo, 2 if not existed, 1 if unexpected error, 0 if ok
}

void add_dir_oex(){ // adds existing files in cwd and its subtree
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL){
        if(entry->d_type==8)
            add_file(entry->d_name);
        else if(is_ok_dir(entry->d_name)){
            chdir(entry->d_name);
            add_dir_oex();
            chdir("..");
        }
    }
    closedir(dir);
}

int add_dir_fle_opr(char *path){ // cwd should be .targit and path is absolute
    FILE *t=fopen("tracked", "r");
    char tmp_path[MAX_ADR_NAME];
    int ok=0, res=2;
    while(fscanf(t, "%s", tmp_path) != EOF){
        if(strncmp(tmp_path, path, strlen(path)) == 0){
            res=add_file_opr(tmp_path);
            ok=(strlen(tmp_path)==strlen(path));
        }
    }
    fclose(t);
    if(ok) return res;
    if(is_dir(path)){
        chdir(path);
        add_dir_oex();
        chdir(repo_path);
        return 0;
    }
    t=fopen(path, "r");
    if(t != NULL){
        fclose(t);
        return add_file_opr(path);
    }
    return res;
}

int add_dir_fle(char *path){ // adds from anywhere and path could be either absolute or not
    char cwd[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(abs_path(abs_p, path) == NULL)   return 1;
    if(in_repo(abs_p) == 0) return 3;
    if(strcmp(abs_p+(strlen(abs_p)-7), ".targit") == 0) return 4;
    chdir(repo_path);
    int res=add_dir_fle_opr(abs_p);
    chdir(cwd);
    return res;//returns 3 if out of repo, 2 if not existed, 1 if unexpected error, 0 if ok
}

void redo_opr(){//currently in stage/i
    char cwd[MAX_ADR_NAME], tmp_path[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) exit(1);
    FILE *f_p=fopen(cnct(cwd, "/file_path"), "r");
    fgetS(tmp_path, MAX_ADR_NAME, f_p);
    fclose(f_p);
    int sz=strlen(tmp_path);
    char *name=strtok(tmp_path, "/");
    chdir("/");
    while(1){
        if(name+strlen(name) == tmp_path+sz)
            break;
        if(chdir(name) != 0){
            if(mkdir(name, 0777) != 0) exit(1);
            chdir(name);
        }
        name=strtok(NULL, "/");
    }
    if(access(cnct(cwd, "/file"), F_OK) == 0){
        FILE *f=fopen(name, "w");
        FILE *g=fopen(cnct(cwd, "/file"), "r");
        flecpy(f, g);
        fclose(f);
        fclose(g);
    }
    else if(access(name, F_OK) == 0)
        remove(name);
    chdir(cwd);
}

int redo(){
    char tmp_path[MAX_ADR_NAME];
    chdir(repo_path);
    chdir("stage");
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir)) != 0){
        if(entry->d_name[0] == '.')
            continue;
        chdir(entry->d_name);
        redo_opr();
        chdir("..");
    }
    return 0;
}

void lst_stage(int i, int n){//lists files in cwd till depth n
    if(n==0) return;
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL){
        if(entry->d_type==8){
            for(int j=0;j<i;++j)
                printf("\t");
            printf("%s : ", entry->d_name);
            printf("%s\n", (is_staged(entry->d_name) ? "staged" : "unstaged"));
        }
        else if(is_ok_dir(entry->d_name) && n>1){
            for(int j=0;j<i;++j)
                printf("\t");
            printf("%s : \n", entry->d_name);
            chdir(entry->d_name);
            lst_stage(i+1, n-1);
            chdir("..");
        }
    }
    printf("\n");
}

int add(int argc, char *argv[]){
    if((where_is_inited()) == NULL){
        perror("The repo is not initialized\n");
        return 1;
    }
    if(argc == 2){
        printf("Invalid command\n");
        return 1;
    }
    if(strcmp(argv[2], "-n") == 0){
        if(argc!=4){
            printf("Invalid command\n");
            return 1;
        }
        lst_stage(0, atoi(argv[3]));
        return 0;
    }
    else if(strcmp(argv[2], "-redo") == 0){
        if(argc!=3){
            printf("Invalid command\n");
            return 1;
        }
        return redo();
    }
    else{
        int x=2;
        if(strcmp(argv[2], "-f") == 0)
            ++x;
        for(int i=x;i<argc;++i){
            int hh=add_dir_fle(argv[i]);
            if(hh == 0)
                printf("%s added successfuly\n", argv[i]);
            else if(hh == 2)
                printf("No file or directory wich path is %s\n", argv[i]);
            else if(hh == 3)
                printf("%s is out of repository\n", argv[i]);
            else if(hh == 4)
                printf("Adding .targit can cause unexpected errors and is banned\n");
            else 
                return 1;
        }
    }
    return 0;
}
