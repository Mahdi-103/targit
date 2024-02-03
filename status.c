#include "commit.c"

char *status_here(char *prnt, int depth){ // shows status for cwd
    char ans[20000];
    DIR *dir=opendir(".");
    struct dirent *entry;
    int ans_ptr=0;
    while((entry=readdir(dir)) != NULL){
        if(entry->d_type==8){
            char sts=file_status(entry->d_name);
            if(sts == '\0')
                continue;
            for(int i=0;i<depth;++i)
                prnt[ans_ptr++]='\t';
            sprintf(prnt+ans_ptr, "%s : ", entry->d_name);
            ans_ptr+=strlen(entry->d_name)+3;
            if(!is_tracked(entry->d_name)){
                sprintf(prnt+ans_ptr, "-U\n");
                ans_ptr+=3;
                continue;
            }
            if(is_staged(entry->d_name))
                prnt[ans_ptr++] = '+';
            else 
                prnt[ans_ptr++] = '-';
            sprintf(prnt+ans_ptr, "%c\n", sts);
            ans_ptr+=2;
        }
        else if(is_ok_dir(entry->d_name)){
            chdir(entry->d_name);
            status_here(ans, depth+1);
            chdir("..");
            if(strlen(ans) == 0) continue;
            for(int i=0;i<depth;++i)
                prnt[ans_ptr++]='\t';
            sprintf(prnt+ans_ptr, "%s : \n", entry->d_name);
            ans_ptr+=strlen(entry->d_name)+4;
            sprintf(prnt+ans_ptr, "%s", ans);
            ans_ptr+=strlen(ans);
        }
    }
    closedir(dir);
    prnt[ans_ptr]='\0';
    return prnt;
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
    if(argc==3)
        chdir(cnct(repo_path, "/.."));
    char ans[20000];
    status_here(ans, 0);
    printf("%s", ans);
    return 0;
}
/*
int main(int argc, char *argv[]){
    show_status(argc, argv);
}*/