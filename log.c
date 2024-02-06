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
    printf("%s\n", out);
    chdir(cwd);
    return 0;
}

int show_log(int argc, char *argv[]){ // cwd is .targit
    int n;
    FILE *num=fopen("commits/num", "r");
    fscanf(num, "%d", &n);
    fclose(num);
    if(argc == 2){
        for(int i=n;i>=0;--i)
            log_commit(itos(i));
        return 0;
    }
    else if(strcmp(argv[2], "-n") == 0){
        if(argc<4){
            printf("Please enter the number\n");
            return 1;
        }
        if(argc>4)
            return 2;
        int nn=atoi(argv[3]);
        if(nn>n+1){
            printf("There is no more than %d commits\n", n+1);
            return 1;
        }
        for(int i=0;i<atoi(argv[3]);++i)
            log_commit(itos(n-i));
        return 0;
    }
    else if(strcmp(argv[2], "-branch") == 0){
        if(argc<4){
            printf("Please enter the branch name\n");
            return 1;
        }
        if(argc>4)
            return 3;
        if(access(cnct("branches/", argv[3]), F_OK) != 0)
            return 4;
        int lst_com, frs_com;
        chdir(cnct("branches/", argv[3]));
        FILE *f=fopen("first_commit", "r");
        fscanf(f, "%d", &frs_com);
        fclose(f);
        f=fopen("HEAD", "r");
        fscanf(f, "%d", &lst_com);
        fclose(f);
        chdir("../..");
        while(lst_com != frs_com){
            log_commit(itos(lst_com));
            lst_com=par_com(lst_com);
        }
        return 0;
    }
    else if(strcmp(argv[2], "-author") == 0){
        if(argc<4){
            printf("Please enter the name\n");
            return 1;
        }
        if(argc>4)
            return 3;
        chdir("commits");
        for(int i=n;i>=0;--i){
            char name[MAX_NME_LNG];
            FILE *f=fopen(cnct(itos(i), "/user"), "r");
            fgetS(name, MAX_NME_LNG, f);
            fclose(f);
            if(strcmp(name, argv[3]) == 0)
                log_commit(itos(i));
        }
        chdir("..");
        return 0;
    }
    else if(strcmp(argv[2], "-since") == 0){
        if(argc<4){
            printf("Please enter the date\n");
            return 1;
        }
        if(argc>4 || strlen(argv[3]) != 19)
            return 5;
        chdir("commits");
        for(int i=n;i>=0;--i){
            char com_time[30];
            FILE *f=fopen(cnct(itos(i), "/time"), "r");
            fgetS(com_time, 30, f);
            fclose(f);
            if(strcmp(com_time, argv[3]) >= 0)
                log_commit(itos(i));
        }
        chdir("..");
        return 0;
    }
    else if(strcmp(argv[2], "-before") == 0){
        if(argc<4){
            printf("Please enter the date\n");
            return 1;
        }
        if(argc>4 || strlen(argv[3]) != 19)
            return 5;
        chdir("commits");
        for(int i=n;i>=0;--i){
            char com_time[30];
            FILE *f=fopen(cnct(itos(i), "/time"), "r");
            fgetS(com_time, 30, f);
            fclose(f);
            if(strcmp(com_time, argv[3]) <= 0)
                log_commit(itos(i));
        }
        chdir("..");
        return 0;
    }
    else if(strcmp(argv[2], "-search") == 0){
        if(argc < 4){
            printf("Please enter the word\n");
            return 1;
        }
        if(argc > 4){
            printf("If you want to search a word with spaces use double qoutations\n");
            return 1;
        }
        chdir("commits");
        for(int i=n;i>=0;--i){
            char msg[80];
            FILE *f=fopen(cnct(itos(i), "/message"), "r");
            fgetS(msg, 80, f);
            fclose(f);
            if(strstr(msg, argv[3]) != NULL)
                log_commit(itos(i));
        }
        chdir("..");
        return 0;
    }
    return 2; // 0 if ok, 2 if invalid, 3 if not "", 4 if wrong branch, 5 if wrong date
}

int logg(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    chdir(repo_path);
    int retr=show_log(argc, argv);
    if(retr == 2)
        printf("Invalid command\n");
    else if(retr == 3)
        printf("Name with whitespaces must be between qoutations\n");
    else if(retr == 4)
        printf("There is no branch with this name\n");
    else if(retr == 5)
        printf("Invalid date\n");
    return retr;
}
/*
int main(int argc, char *argv[]){
    where_is_inited();
    return logg(argc, argv);
}*/