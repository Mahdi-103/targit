#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "errno.h"
#define MAX_ADR_NAME 1000
#define MAX_NME_LNG 100
char repo_path[MAX_ADR_NAME], the_head[10], branch_name[MAX_NME_LNG];

char *head(){ // cwd could be anywhere
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return NULL;
    chdir(repo_path);
    FILE *h=fopen("HEAD", "r");
    fscanf(h, "%s", the_head);
    fclose(h);
    chdir(cwd);
    return the_head;
}

char *fgetS(char *str, int __n, FILE *f){
    char *res=fgets(str, __n, f);
    if(res != NULL && strlen(str) && str[strlen(str)-1]=='\n')
        str[strlen(str)-1]='\0';
    return res;
}

char *cnct(const char *a, const char * b){
    static char *c;
    c=malloc(strlen(a)+strlen(b)+1);
    memcpy(c, a, strlen(a));
    memcpy(c+strlen(a), b, strlen(b)+1);
    return c;
}

char *strcnct(char *res, const char *a, const char *b){
    char *tmp_res=cnct(a, b);
    memcpy(res, tmp_res, strlen(tmp_res)+1);
    return res;
}

char *which_branch(){
    FILE *f=fopen(cnct(repo_path, "/current_branch"), "r");
    fgetS(branch_name, MAX_NME_LNG, f);
    fclose(f);
    return branch_name;
}

int is_dir(const char *path){
    DIR *dir=opendir(path);
    if(dir){
        closedir(dir);
        return 1;
    }
    return 0;
}

int contains_line(char *path, const char *str){
    if(access(path, F_OK) != 0 || is_dir(path))
        return -1;
    char ln[MAX_ADR_NAME];
    FILE *f=fopen(path, "r");
    while(fgetS(ln, MAX_ADR_NAME, f) != NULL){
        if(strcmp(str, ln) == 0){
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
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

int flecpy(FILE *paste, FILE *source){
    char ch;
    while((ch=getc(source)) != EOF)
        fprintf(paste, "%c", ch);
    return 0;
}

void flecpy_path(char *paste, char *source){
    FILE *pst=fopen(paste, "w");
    FILE *src=fopen(source, "r");
    flecpy(pst, src);
    fclose(pst);
    fclose(src);
}

int dircpy_opr(char *pst_path, char *src_name){
    char new_path[MAX_ADR_NAME];
    strcnct(new_path, pst_path, "/");
    if((access(cnct(new_path, src_name), F_OK) == 0) || (chdir(src_name) != 0))
        return 1;
    mkdir(strcnct(new_path, new_path, src_name), 0755);
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL){
        if(entry->d_type==8){
            char new_new[MAX_ADR_NAME];
            strcnct(new_new, new_path, "/");
            flecpy_path(cnct(new_new, entry->d_name), entry->d_name);
        }
        else if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            if(dircpy_opr(new_path, entry->d_name)){
                closedir(dir);
                return 1;
            }
        }
    }
    closedir(dir);
    chdir("..");
    return 0;
}

int dircpy(char *pst_path, char *src_pth){
    char abs_pst[MAX_ADR_NAME], cwd[MAX_ADR_NAME], src_path[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(abs_path(abs_pst, pst_path) == NULL) return 1;
    int sz=strlen(src_pth);
    memcpy(src_path, src_pth, sz+1);
    if(chdir(src_path) != 0) return 1;
    chdir("..");
    char *src_name=strtok(src_path, "/");
    while(src_name+strlen(src_name) != src_path+sz)
        src_name=strtok(NULL, "/");
    int ret_val=dircpy_opr(abs_pst, src_name);
    chdir(cwd);
    return ret_val;
}

char *from_home(const char *a){
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

int in_repo(const char *path){
    if(strncmp(path, repo_path, strlen(repo_path)-7) == 0) return 1;
    return 0;
}

int is_tracked(const char *path){
    FILE *f=fopen(cnct(repo_path, "/tracked"), "r");
    char tmp_path[MAX_ADR_NAME], abs_p[MAX_ADR_NAME];
    path=abs_path(abs_p, path);
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

int is_ok_dir(const char *name){
    if(strcmp(name, ".") == 0)
        return 0;
    if(strcmp(name, "..") == 0)
        return 0;
    if(strcmp(name, ".targit") == 0)
        return 0;
    return 1;
}

int is_same(const char *path_f, const char *path_g){
    if(is_dir(path_f) || is_dir(path_g))
        return -1;
    if(access(path_f, F_OK) != 0){
        if(access(path_g, F_OK) != 0)
            return 0;
        return 1;
    }
    if(access(path_g, F_OK) != 0)
        return 2;
    FILE *f=fopen(path_f, "r");
    FILE *g=fopen(path_g, "r");
    char c_f, c_g;
    while(1){
        c_f=getc(f);
        c_g=getc(g);
        if(c_f!=c_g)
            return 1;
        if(c_f==EOF)
            break;
    }
    fclose(f);
    fclose(g);
    return 0; // 0 if same, 1 if g modified, 2 if g deleted, 3 if g T(dastresi)
}

int is_staged(char *path){
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
        fgetS(tmp_path, MAX_ADR_NAME, g);
        if(strcmp(tmp_path, path) == 0){
            closedir(dir);
            fclose(g);
            ok=(is_same(cnct(entry->d_name, "/file"), path) == 0);
            break;
        }
    }    
    chdir(cwd);
    return ok;
}

char file_status(const char *path){
    char abs_p[MAX_ADR_NAME], cwd[MAX_ADR_NAME], commit_id[10];
    if((path = abs_path(abs_p, path)) == NULL) exit(1);
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) exit(1);
    chdir(cnct(repo_path, "/commits"));
    if((strcmp(head(), "-1") == 0) || (contains_line(cnct(the_head, "/tracked"), path) != 1)){
        chdir(cwd);
        return 'A';
    }
    int res=-1;
    memcpy(commit_id, the_head, strlen(the_head)+1);
    do{
        chdir(cnct(commit_id, "/stage"));
        DIR *dir=opendir(".");
        struct dirent *entry;
        while((entry=readdir(dir)) != NULL){
            if(entry->d_name[0] != '.'){
                chdir(entry->d_name);
                if(contains_line("file_path", path)){
                    res=is_same("file", path);
                    break;
                }
                chdir("..");
            }
        }
        closedir(dir);
        if(res != -1)
            break;
        FILE *pr=fopen("../par", "r");
        fscanf(pr, "%s", commit_id);
        fclose(pr);
        chdir("../..");
    }while(strcmp(commit_id, "-1"));
    chdir(cwd);
    if(res == -1)
        exit(1);
    if(res == 1)
        return 'M';
    if(res == 2)
        return 'D';
    return '\0';
}

void remove_dir_here(){ // makes the cwd empty
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL){
        if(entry->d_type==8)
            remove(entry->d_name);
        else if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            chdir(entry->d_name);
            remove_dir_here();
            chdir("..");
            rmdir(entry->d_name);
        }
    }
}

int remove_dir(char *path){
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    chdir(path);
    remove_dir_here();
    chdir(cwd);
    rmdir(path);
    return 0;
}

char *write_time(char *path){
    time_t rawtime;
    struct tm *time_info;
    time(&rawtime);
    time_info=localtime(&rawtime);
    static char cur_time[100];
    sprintf(cur_time, "%d/%d/%d %d:%d:%d", time_info->tm_year+1900, time_info->tm_mon+1,
            time_info->tm_mday, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    FILE *f=fopen(path, "w");
    fprintf(f, "%s", cur_time);
    fclose(f);
    return cur_time;
}