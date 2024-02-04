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
    if(!stage_empty()) return 2;
    if(access(cnct("commits/", itos(id)), F_OK) != 0) return 3;
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
}

int checkout(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    
}