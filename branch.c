#include "status.c"

int new_branch(char *name){ // changes current branch or not?
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    chdir(cnct(repo_path, "/branches"));
    if(access(name, F_OK) == 0) return 2;
    if(mkdir(name, 0755) != 0) return 1;
    FILE *f/*=fopen("../current_branch", "w");
    fprintf(f, "%s", name);
    fclose(f)*/;
    chdir(name);
    f=fopen("first_commit", "w");
    fprintf(f, "%s", head());
    fclose(f);
    f=fopen("HEAD", "w");
    fprintf(f, "%s", the_head);
    fclose(f);
    chdir(cwd);
    return 0; // 0 if ok, 1 if unexpected, 2 if already exists
}

void list_branches(){
    printf("branches : \n");
    DIR *dir=opendir(cnct(repo_path, "/branches"));
    struct dirent *entry;
    while((entry=readdir(dir)) != NULL){
        if(is_ok_dir(entry->d_name))
            printf("\t%s\n", entry->d_name);
    }
    closedir(dir);
}

int branch(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(argc == 2){
        list_branches();
        return 0;
    }
    if(argc>3){
        printf("Branch name with whitespaces must be between qoutations\n");
        return 1;
    }
    int res=new_branch(argv[2]);
    if(res==2)
        printf("This name already exists\n");
    if(res==0)
        printf("Branch %s created successfully\n", argv[2]);
    return res;
}