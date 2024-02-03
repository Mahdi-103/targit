#include "add.c"

struct commit_info{
    int ret_val, id;
    char *cur_time, *msg;
};

int ok_head(){ // cwd could be anywhere
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return -1;
    chdir(repo_path);
    int now_head, branch_head;
    FILE *h=fopen("HEAD", "r");
    fscanf(h, "%d", &now_head);
    fclose(h);
    chdir(cnct("branches/", which_branch()));
    h=fopen("HEAD", "r");
    fscanf(h, "%d", &branch_head);
    chdir(cwd);
    if(branch_head == now_head)
        return 1;
    return 0;
}

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

int upd_head(int new_head){ // cwd could be anywhere
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return -1;
    chdir(repo_path);
    int res;
    FILE *h=fopen("HEAD", "r");
    fscanf(h, "%d", &res);
    fclose(h);
    h=fopen("HEAD", "w");
    fprintf(h, "%d", new_head);
    fclose(h);
    chdir(cnct("branches/", which_branch()));
    h=fopen("HEAD", "w");
    fprintf(h, "%d", new_head);
    fclose(h);
    chdir(cwd);
    return res;
}

int set_srct(char *shrtcut, char *msg){ // cwd is .targit
    if(access(cnct("shortcuts/message/", shrtcut), F_OK) == 0) return -2;
    if(strlen(msg) > 72) return 3;
    FILE *f=fopen(cnct("shortcuts/message/", shrtcut), "w");
    fprintf(f, "%s", msg);
    fclose(f);
    return 0;// 0 if ok, -2 if already exist, 3 if too long
}

int rem_srct(char *shrtcut){ // cwd is .targit
    if(access(cnct("shortcuts/message/", shrtcut), F_OK) != 0)
        return 2;
    remove(cnct("shortcuts/message/", shrtcut));
    return 0;// 0 if ok, 2 if doesn't exist
}

int rep_srct(char *shrtcut, char *msg){// cwd is .targit
    if(access(cnct("shortcuts/message/", shrtcut), F_OK) != 0) return 2;
    if(strlen(msg) > 72) return 3;
    FILE *f=fopen(cnct("shortcuts/message/", shrtcut), "w");
    fprintf(f, "%s", msg);
    fclose(f);
    return 0;// 0 if ok, 2 if doesn't exist, 3 if too long
}

char *msg_srct(char *shrtcut){ // cwd is .targit
    if(access(cnct("shortcuts/message/", shrtcut), F_OK) != 0)
        return NULL;
    static char msg[80];
    FILE *f=fopen(cnct("shortcuts/message/", shrtcut), "r");
    fgetS(msg, 80, f);
    fclose(f);
    return msg;
}

struct commit_info *commit_opr(int argc, char *argv[]){ // cwd is .targit
    static struct commit_info ret;
    char *msg;
    if(stage_empty() == 1){
        ret.ret_val=5;
        return &ret;
    }
    if(argc<4){
        ret.ret_val=2;
        return &ret;
    }
    if(argc>4){
        ret.ret_val=3;
        return &ret;
    }
    if(strcmp(argv[2], "-s") == 0){
        msg=msg_srct(argv[3]);
        if(msg==NULL){
            ret.ret_val=-1;
            return &ret;
        }
    }
    else 
        msg=argv[3];
    if(strlen(msg) > 72){
        ret.ret_val=4;
        return &ret;
    }
    ret.ret_val=1;
    ret.msg=msg;
    char name[MAX_NME_LNG];
    chdir("commits");
    sprintf(name, "%d", ret.id=new_commit_id());
    if(mkdir(name, 0755) != 0) return &ret;
    chdir(name);
    dircpy(".", "../../stage");
    remove_dir("../../stage");
    if(mkdir("../../stage", 0755) != 0) return &ret;
    flecpy_path("tracked", "../../tracked");
    FILE *f=fopen("par", "w");
    fprintf(f, "%d", upd_head(ret.id));
    fclose(f);
    f=fopen("message", "w");
    fprintf(f, "%s", msg);
    fclose(f);
    ret.cur_time=write_time("time");
    flecpy_path("user", "../../config");
    chdir("../..");
    ret.ret_val=0;
    return &ret;//0 if ok, 1 if unexp, 2 if no msg, 3 if no "", 4 too long, 5 if emp st
}   //-1 if wrong shortcut

int commit(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        perror("The repo is not initialized\n");
        return 1;
    }
    if(ok_head() != 1){
        printf("You must be on HEAD of project to commit\n");
        return 1;
    }
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    chdir(repo_path);
    if(strcmp(argv[1], "commit") == 0){
        struct commit_info *retr=commit_opr(argc, argv);
        if(retr->ret_val == 0){
            printf("Staged changes commited successfully!\n");
            printf("id: %d\ntime: %s\nmessage: %s\n", retr->id, retr->cur_time, retr->msg);
        }
        else if(retr->ret_val == 2)
            printf("The message should not be empty\n");
        else if(retr->ret_val == 3)
            printf("The message with whitespaces must be between qoutations\n");
        else if(retr->ret_val == 4)
            printf("The message is too long\nIt should be at most 72 characters\n");
        else if(retr->ret_val == 5)
            printf("There is no staged changes to be commited\n");
        else if(retr->ret_val == -1)
            printf("There is no shortcut as %s\n", argv[3]);
        chdir(cwd);
        return retr->ret_val;
    }
    else{
        int retr;
        if(strcmp(argv[1], "set") == 0){
            if((argc!=6) || strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
                retr = -1;
            else
                retr = set_srct(argv[5], argv[3]);
        }
        else if(strcmp(argv[1], "replace") == 0){
            if((argc!=6) || strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
                retr = -1;
            else
                retr = rep_srct(argv[5], argv[3]);
        }
        else if(strcmp(argv[1], "remove") == 0){
            if((argc!=4) || strcmp(argv[2], "-s"))
                retr = -1;
            else 
                retr = rem_srct(argv[3]);
        }
        else 
            retr = -1;
        if(retr == -1)
            printf("Invalid command\n");
        else if(retr == -2)
            printf("This shortcut-name already exists\n");
        else if(retr == 2)
            printf("No shortcut with this name\n");
        else if(retr == 3)
            printf("The message is too long\nIt should be at most 72 characters\n");
        else if(retr == 0)
            printf("Shortcut updated successfully\n");
        chdir(cwd);
        return retr;
    }
    printf("Invalid command\n");
    chdir(cwd);
    return 1;
}