#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAX_ADR_NAME 1000
#define MAX_NME_LNG 100
char repo_path[MAX_ADR_NAME];

int flecpy(FILE *paste, FILE *source){
    char ch;
    while((ch=getc(source)) != EOF)
        fprintf(paste, "%c", ch);
    return 0;
}

char *cnct(const char *a, const char * b){
    static char *c;
    c=malloc(strlen(a)+strlen(b)+1);
    memcpy(c, a, strlen(a));
    memcpy(c+strlen(a), b, strlen(b)+1);
    return c;
}

char *from_home(char *a){
    static char adr[MAX_ADR_NAME];
    int sz=strlen(getenv("HOME"));
    memcpy(adr, getenv("HOME"), sz);
    adr[sz]='/';

    memcpy(adr+sz+1, a, strlen(a)+1);
    return adr;
}

void deblank(char *s){
    char *t=s;
    while(1){
        int x=0;
        while(isspace(s[x])) ++x;
        if(x) memmove(s, s+x, strlen(s+x)+1);
        while(strlen(s) && !isspace(*s)) ++s;
        if(strlen(s) == 0)
            break;
        ++s;   
    }
}

char *abs_path(char *path, const char *tmp_path){
    char dir_adr[MAX_ADR_NAME];
    if(*tmp_path!='/'){
        if(getcwd(dir_adr, MAX_ADR_NAME) == NULL) return NULL;
        int sz=strlen(dir_adr);
        dir_adr[sz]='/';
        memcpy(dir_adr+sz+1, tmp_path, strlen(tmp_path)+1);
        tmp_path=dir_adr;
    }
    path[0]='/';
    path[1]='\0';
    char tmp[MAX_NME_LNG];
    int i=1, j=0;
    while(i<strlen(tmp_path)){
        while(tmp_path[i] != '/' && tmp_path[i]!='\0'){
            tmp[j]=tmp_path[i];
            ++j;
            ++i;
        }
        tmp[j]='/';
        tmp[j+1]='\0';
        if(strcmp(tmp, "../") == 0){
            if(strlen(path)==1)
                continue;
            do{
                path[strlen(path)-1]='\0';
            }while(path[strlen(path)-1]!='/');
        }
        else if(strcmp(tmp, "./"))
            memcpy(path+strlen(path), tmp, j+2);
        ++i;
        j=0;
    }
    if(strlen(path) > 1)
        path[strlen(path)-1]='\0';
    return path;
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

int is_dir(char *path){
    DIR *dir=opendir(path);
    if(dir){
        closedir(dir);
        return 1;
    }
    return 0;
}

int in_repo(char *path){
    if(strncmp(path, repo_path, strlen(repo_path)-7) == 0) return 1;
    return 0;
}

int is_tracked(const char *path){
    FILE *f=fopen(cnct(repo_path, "/tracked"), "r");
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
    FILE *f=fopen(cnct(repo_path, "/tracked"), "a");
    fprintf(f, "%s\n", path);
    fclose(f);
}

int is_ok_dir(char *name){
    if(strcmp(name, ".") == 0)
        return 0;
    if(strcmp(name, "..") == 0)
        return 0;
    if(strcmp(name, ".targit") == 0)
        return 0;
    return 1;
}

int is_changed(){

}

int is_staged(char *path){//path must be absolute
    char cwd[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 2;
    if((path = abs_path(abs_p, path)) == NULL) return 2;
    chdir(cnct(repo_path, "/stage"));
    DIR *dir=opendir(".");
    struct dirent *entry;
    int ok=0;
    while((entry=readdir(dir)) != NULL){
        if(*entry->d_name == '.')
            continue;
        char tmp_path[MAX_ADR_NAME];
        FILE *g=fopen(cnct(entry->d_name, "/file_path"), "r");
        fscanf(g, "%s", tmp_path);
        if(strcmp(tmp_path, path) == 0){
            closedir(dir);
            fclose(g);
            ok=1;
            break;
        }
    }    
    chdir(cwd);
    return ok;
}