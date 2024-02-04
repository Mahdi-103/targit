#include "log.c"

void restore_file(const char *path, FILE *f){ // path must be absolute
    char cwd[MAX_ADR_NAME], tmp_path[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) exit(1);
    chdir("/");
    memcpy(tmp_path, path, strlen(path)+1);
    int sz=strlen(tmp_path);
    path = strtok(tmp_path, "/");
    while(1){
        if(path+strlen(path) == tmp_path+sz)
            break;
        if(access(path, F_OK))
            if(mkdir(path, 0777) != 0) exit(1);
        else if(!is_dir(path)){
            if(remove(path) != 0) exit(1);
            if(mkdir(path, 0777) != 0) exit(1);
        }
        chdir(path);
        path = strtok(NULL, "/");
    }
    if(access(path, F_OK))
        delete(path);
    FILE *g=fopen(path, "w");
    char ch;
    while((ch=getc(f)) != EOF)
        fprintf(g, "%c", ch);
    fclose(g);
    chdir(cwd);
}

void restore(int id){ //cwd must be .targit/commits
    if(access(itos(id), F_OK)) return;
    restore(par_com(id));
    chdir(cnct(itos(id), "/stage"));
    DIR *dir=opendir(".");
    struct dirent *entry;
    char path[MAX_ADR_NAME];
    while((entry = readdir(dir)) != NULL){
        if(is_ok_dir(entry->d_name)){
            chdir(entry->d_name);
            FILE *f=fopen("file_path", "r");
            fgetS(path, MAX_ADR_NAME, f);
            fclose(f);
            f=fopen("file", "r");
            restore_file(path, f);
            fclose(f);
            chdir("..");
        }
    }
    chdir("../..");
}

int go_to(int id){ //cwd must be .targit
    if(access(cnct("commits/", itos(id)), F_OK) != 0) return 10;
    chdir(cnct("commits/", itos(id)));
    FILE *t=fopen("../../tracked", "r");
    char path[MAX_ADR_NAME];
    while(fgetS(path, MAX_ADR_NAME, t) != NULL)
        delete(path);
    fclose(t);
    t=fopen("tracked", "r");
    while(fgetS(path, MAX_ADR_NAME, t) != NULL)
        delete(path);
    fclose(t);
    t=fopen("../../HEAD", "w");
    fprintf(t, "%d", id);
    fclose(t);
    flecpy_path("../../tracked", "tracked");
    chdir("..");
    restore(id);
    return 0;
}

int checkout_opr(int argc, char *argv[]){ // cwd must be .targit
    if(argc != 3) return 2;
    if(!stage_empty()) return 3;
    int id;
    char branch[MAX_NME_LNG], path[MAX_ADR_NAME];
    if(strcmp(argv[2], "HEAD") == 0){
        if(argc > 3) return 2;
        FILE *f=fopen("current_branch", "r");
        fgetS(branch, MAX_NME_LNG, f);
        fclose(f);
        chdir(cnct("branches/", branch));
        f=fopen("HEAD", "r");
        fscanf(f, "%d", &id);
        fclose(f);
        chdir("../..");
        return go_to(id);
    }
    if(strncmp(argv[2], "HEAD-", 5) == 0){
        int n, lst_id;
        sscanf(argv[2], "HEAD-%d", &n);
        FILE *f=fopen("current_branch", "r");
        fgetS(branch, MAX_NME_LNG, f);
        fclose(f);
        chdir(cnct("branches/", branch));
        f=fopen("HEAD", "r");
        fscanf(f, "%d", &id);
        fclose(f);
        f=fopen("first_commit", "r");
        fscanf(f, "%d", &lst_id);
        fclose(f);
        chdir("../..");
        for(int i=0;i<n;++i){
            id=par_com(id);
            if(id == lst_id && i < n-1)
                return 5;
        }
        return go_to(id);
    }
    if((access(cnct("commits/", argv[2]), F_OK) == 0) && is_dir(cnct("commits/", argv[2])))
        return go_to(atoi(argv[2]));
    if(access(cnct("branches/", argv[2]), F_OK) == 0){
        FILE *f=fopen("current_branch", "w");
        fprintf(f, "%s", argv[2]);
        fclose(f);
        chdir(cnct("branches/", argv[2]));
        f=fopen("HEAD", "r");
        fscanf(f, "%d", id);
        fclose(f);
        chdir("../..");
        return go_to(id);
    }
    return 4; //0 if ok, 2 if invalid, 3 if unempty stage, 4 if no brn nor com
}

int checkout(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    int retr=checkout_opr(argc, argv);
    if(retr == 0)
        printf("Checkout done!\n");
    else if(retr == 2)
        printf("Invalid command\n");
    else if(retr == 3)
        printf("The staging area is unempty\nPlease commit or reset the files\n");
    else if(retr == 4)
        printf("There is no commit id nor branch name as %s\n", argv[2]);
    else if(retr == 5)
        printf("The head doesn't have as much as n parents in its branch");
    return retr;
}