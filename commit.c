#include "add.c"

int new_commit_id(){ // cwd is .targit/commits
    int id;
    FILE *num=fopen("num", "r");
    fscanf(num, "%d", &id);
    fclose(num);
    num=fopen("num", "w");
    fprintf(num, "%d", ++id);
    fclose(num);
    return id;
}

int upd_head(int new_head){
    int res;
    FILE *h=fopen(cnct(repo_path, "/HEAD"), "r");
    fscanf(h, "%d", res);
    fclose(h);
    h=fopen(cnct(repo_path, "/HEAD"), "w");
    fprintf(h, "%d", new_head);
    fclose(h);
    return res;
}

int commit(char *msg){ // cwd is .targit
    if(stage_empty() == 1) return 2;
    int id;
    char name[MAX_NME_LNG];
    chdir("commits");
    sprintf(name, "%d", id=new_commit_id());
    if(mkdir(name, 0755) != 0) return 1;
    chdir(name);
    dircpy(".", "../../stage");
    flecpy_path("tracked", "../../tracked");
    FILE *f=fopen("par", "w");
    fprintf(f, "%d", upd_head(id));
    fclose(f);
    f=fopen("message", "w");
    fprintf(f, "%s", msg);
    fclose(f);
}

int main(){
    where_is_inited();
    chdir(repo_path);
    printf("%d", commit("Hello"));
}