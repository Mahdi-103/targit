#include "repo.c"

int is_taracked(const char *path){
    FILE *f=fopen("tracked", "r");
    char tmp_path[MAX_ADR_NAME];
    int res=0;
    while(fscanf(f, "%s", tmp_path) != EOF){
        if(strcmp(tmp_path, path) == 0){
            res=1;
            break;
        }
    }
    fclose(f);
    return res;
}

void track(const char *path){
    FILE *f=fopen("tracked", "a");
    fprintf(f, "%s\n", path);
    fclose(f);
}

int add_file_opr(char *path){
    FILE *f=fopen(path, "r");
    DIR *dir=opendir("stage");
    struct dirent *entry;    
    if(f == NULL){
        if(is_taracked(path)){
            chdir("stage");
            while((entry=readdir(dir)) != NULL){
                if(*entry->d_name == '.')
                    continue;
                char tmp_path[MAX_ADR_NAME];
                FILE *g=fopen(cnct(entry->d_name, "/file_path"), "r");
                fscanf(g, "%s", tmp_path);
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
        else{
            perror("No such file or directory");
            return 1;
        }
    }
    if(is_taracked(path)){
        while((entry=readdir(dir)) != NULL){
            if(*entry->d_name == '.')
                continue;
            char tmp_path[MAX_ADR_NAME];
            FILE *g=fopen(cnct(entry->d_name, "/file_path"), "r");
            fscanf(g, "%s", tmp_path);
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
        return 1;
    }
    else{
        track(path);
        char name[100];
        int num=0;
        while((entry=readdir(dir)) != NULL)
            ++num;
        sprintf(name, "%d", num-2);
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
    }
    return 0;
}

int add_file(char *path){
    char cur_dir[MAX_ADR_NAME];
    if(getcwd(cur_dir, MAX_ADR_NAME) == NULL) return 1;
    
}

void add_dir_oex(char *dir_adr){ // add dir only existing files in working directory
    DIR *dir=opendir(".");
    struct dirent *entry;
    char tmp_path[MAX_ADR_NAME];
    while((entry=readdir(dir)) != NULL){
        memcpy(tmp_path, dir_adr, strlen(dir_adr));
        memcpy(tmp_path+strlen(dir_adr), entry->d_name, strlen(entry->d_name)+1);
        if(entry->d_type==8)
            add_file(tmp_path);
        else if(*entry->d_name!='.'){
            chdir(entry->d_name);
            add_dir_oex(tmp_path);
            chdir("..");
        }
    }
}

int add_dir_fle(char *path){
    FILE *t=fopen("tracked", "r");
    char tmp_path[MAX_ADR_NAME];
    int ok=0, res=1;
    while(fscanf(t, "%s", tmp_path) != EOF){
        if((tmp_path, path, strlen(path)) == 0){
            add_file(tmp_path);
            res=0;
            ok=(strlen(tmp_path)==strlen(path));
        }
    }
    fclose(t);
    if(ok) return 0;
    t=fopen(path, "r");
    if(t != NULL){
        fclose(t);
        add_file(path);
        return 0;
    }
    DIR *d=opendir(path);
    if(d != NULL){
        closedir(d);
        if(getcwd(tmp_path, MAX_ADR_NAME) == NULL) exit(1);
        chdir(path);
        add_dir_oex(path);
        chdir(tmp_path);
        return 0;
    }
    return res;
}

int add(int argc, char *argv[]){
    char cur_adr[MAX_ADR_NAME];
    if(getcwd(cur_adr, MAX_ADR_NAME) == NULL) return 1;
    if((where_is_inited()) == NULL){
        perror("The repo is not initialized\n");
        return 1;
    }
    chdir(repo_path);
    if(strcmp(argv[2], "-n") == 0){

    }
    else if(strcmp(argv[2], "-redo") == 0){
        
    }
    else{
        int x=2;
        if(strcmp(argv[2], "-f"))
            ++x;
        for(int i=x;i<argc;++i){
            char pth[MAX_ADR_NAME];
            abs_path(pth, argv[i]);
            if(in_repo(pth) == 0){
                printf("%s is out of repository\n", argv[i]);
                continue;
            }
            int hh=add_dir_fle(pth);
            if(hh=0)
                printf("%s added successfuly\n", argv[i]);
            else 
                printf("No file or directory wich path is %s\n", argv[i]);
        }
    }
}

int main(){
    chdir(".targit");
    add_file("/home/mahdi/Documents/targit/repo.c");
    add_file("/home/mahdi/Documents/targit/logs.c");
}