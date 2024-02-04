#include "branch.c"

int log_commit(char *id){//cwd could be anywhere
    if(id == NULL) return 1;
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    chdir(cnct(repo_path, "/commits"));
    if((access(id, F_OK) != 0) || !is_dir(id)) return 2;
    chdir(id);
    char *out=calloc(10000, 1);
    sprintf(out, "commit_id : %s\n", id);
    FILE *f=fopen("user", "r");
    sprintf(out+strlen(out), "user.name : ");
    fgetS(out+strlen(out), MAX_NME_LNG, f);
    sprintf(out+strlen(out), "\nuser.email : ");
    fgetS(out+strlen(out), MAX_NME_LNG, f);
    fclose(f);
    f=fopen("branch", "r");
    sprintf(out+strlen(out), "\nbranch : ");
    fgetS(out+strlen(out), MAX_NME_LNG, f);
    fclose(f);
    DIR *dir=opendir("stage");
    struct dirent *entry;
    int num=-2;
    while((entry=readdir(dir)) != NULL)
        ++num;
    closedir(dir);
    sprintf(out+strlen(out), "\nnumber of files commmited : %d\ntime : ", num);
    f=fopen("time", "r");
    fgetS(out+strlen(out), MAX_NME_LNG, f);
    fclose(f);
    sprintf(out+strlen(out), "\nmessage : ");
    f=fopen("message", "r");
    fgetS(out+strlen(out), MAX_NME_LNG, f);
    fclose(f);
    out[strlen(out)]='\n';
    printf("%s", out);
    chdir(cwd);
    return 0;
}

int log(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    chdir(cnct(repo_path, "/commits"));
    int n;
    FILE *num=fopen("num", "r");
    fscanf(num, "%d", n);
    fclose(num);
    if(argc == 2){

    }
}

int main(){
    where_is_inited();
    log_commit("0");
}
