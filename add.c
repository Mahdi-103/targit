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

int add_file(char *path){
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

void add_dir(char *dir_path){
    FILE *t=fopen("tracked", "r");
    char tmp_path[MAX_ADR_NAME];
    while(fscanf(t, "%s", tmp_path) != EOF){
        if((tmp_path, dir_path, strlen(dir_path)) == 0)
            add_file(tmp_path);
    }
    fclose(t);
    if(getcwd(tmp_path, MAX_ADR_NAME) == NULL) exit(1);
    chdir(dir_path);
    add_dir_oex(dir_path);
    chdir(tmp_path);
}

int wildcard_ok(const char *str, const char *name){
    if(*str=='\0')
        return 1;
    else if(*str=='*'){
        while(*str=='*')
            ++str;
        if(*str=='\0')
            return 1;
        int sz=0;
        while(str[sz]!='*' && str[sz]!='\0')
            ++sz;
        if(str[sz]=='\0'){
            while(strlen(name)>strlen(str))
                ++name;
            return !strcmp(str, name);
        }
        int kmp[MAX_NME_LNG];
        char str_name[2*MAX_NME_LNG];
        memcpy(str_name, str, sz+1);
        memcpy(str_name+sz+1, name, strlen(name)+1);
        kmp[0]=0;
        int h=-1;
        for(int i=1;i<strlen(str_name);++i){
            int x=kmp[i-1];
            kmp[i]=0;
            while(1){
                if(str_name[x]==str_name[i]){
                    kmp[i]=x+1;
                    break;
                }
                if(x==0)
                    break;
                x=kmp[x-1];
            }
            if(kmp[i]==sz){
                h=i;
                break;
            }
        }
        if(h==-1)
            return 0;
        return wildcard_ok(str+sz, name+h-sz);
    }
    else{
        while(*str!='*' && *str!='\0'){
            if(*str!=*name)
                return 0;
            ++str;
            ++name;
        }
        return wildcard_ok(str, name);
    }
}

void wildcard(char *str){

}

int add(int argc, char *argv[]){
    char cur_adr[MAX_ADR_NAME], *tar_adr;
    if(getcwd(cur_adr, MAX_ADR_NAME) == NULL) return 1;
    if((tar_adr=where_is_inited()) == NULL){
        perror("The repo is not initialized\n");
        return 1;
    }
    chdir(tar_adr);
    if(strcmp(argv[2], "-f") == 0){

    }
    else if(strcmp(argv[2], "-n") == 0){

    }
    else if(strcmp(argv[2], "-redo")){

    }

}

int main(){
    chdir(".targit");
    add_file("/home/mahdi/Documents/targit/repo.c");
    printf("fuck\n");   
}