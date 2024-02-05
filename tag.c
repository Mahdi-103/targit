#include "reset.c"

struct tag_info{
    int id;
    char name[MAX_NME_LNG], msg[80], usr[MAX_NME_LNG], eml[MAX_NME_LNG], tme[25]; 
};

int new_tag(struct tag_info *tg, int over){ // cwd is .targit
    if(access(cnct("commits/", itos(tg->id)), F_OK))
        return 3;
    chdir("tags");
    if(access(tg->name, F_OK) == 0){
        if(!over)
            return 2;
    }
    else
        if(mkdir(tg->name, 0755) != 0) return 1;
    if(strlen(tg->msg) == 0)
        memcpy(tg->msg, "-", 2);
    chdir(tg->name);
    FILE *f=fopen("commit_id", "w");
    fprintf(f, "%d", tg->id);
    fclose(f);
    f=fopen("message", "w");
    fprintf(f, "%s", tg->msg);
    fclose(f);
    f=fopen("user", "w");
    fprintf(f, "%s\n%s", tg->usr, tg->eml);
    fclose(f);
    f=fopen("time", "w");
    fprintf(f, "%s", tg->tme);
    fclose(f);
    chdir("../..");
    return 0; // 0 if ok , 2 if already exist, 3 if wrong commit
}

int tag_show(char *name){ // cwd is .targit
    chdir("tags");
    if(access(name, F_OK)) return 4;
    chdir(name);
    struct tag_info tg;
    FILE *f=fopen("commit_id", "r");
    fscanf(f, "%d", &tg.id);
    fclose(f);
    f=fopen("message", "r");
    fgetS(tg.msg, 80, f);
    fclose(f);
    f=fopen("user", "r");
    fgetS(tg.usr, MAX_NME_LNG, f);
    fgetS(tg.eml, MAX_NME_LNG, f);
    fclose(f);
    f=fopen("time", "r");
    fgetS(tg.tme, 25, f);
    fclose(f);
    printf("tag : %s\ncommit_id : %d\n", name, tg.id);
    printf("user.name : %s\nuser.email : %s\ntime : %s\nmessage : %s\n", tg.usr, tg.eml, tg.tme, tg.msg);
    chdir("../..");
    return 0;
}

void all_tag(){ // cwd is .targit
    char name[MAX_NME_LNG], tmp_name[MAX_NME_LNG];
    tmp_name[0]=name[0]='\0';
    while(1){
        DIR *dir=opendir("tags");
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(is_ok_dir(entry->d_name) && strcmp(tmp_name, entry->d_name) < 0){
                if((strlen(name) == 0) || (strcmp(name, entry->d_name) > 0))
                    memcpy(name, entry->d_name, strlen(entry->d_name)+1);
            }
        }
        closedir(dir);
        if(strlen(name) == 0)
            break;
        tag_show(name);
        printf("\n");
        memcpy(tmp_name, name, strlen(name)+1);
        name[0]='\0';
    }
}

int tag_opr(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    chdir(repo_path);
    int retr;
    if(argc == 2){
        all_tag();
        return 0;
    }
    if(strcmp(argv[2], "show") == 0){
        if(argc == 3){
            printf("Please enter tag_name\n");
            return 1;
        }
        if(argc > 4){
            printf("Name with whitespaces must be between qoutations\n");
            return 1;
        }
        retr=tag_show(argv[3]);
    }
    else if(strcmp(argv[2], "-a") == 0){
        if(argc == 3){
            printf("Please enter tag name\n");
            return 1;
        }
        int over = 0;
        struct tag_info tg;
        memcpy(tg.name, argv[3], strlen(argv[3])+1);
        tg.id=atoi(head());
        FILE *f=fopen("config", "r");
        fgetS(tg.usr, MAX_NME_LNG, f);
        fgetS(tg.eml, MAX_NME_LNG, f);
        fclose(f);
        char *tmp_time=write_time(NULL);
        memcpy(tg.tme, tmp_time, strlen(tmp_time)+1);
        tg.msg[0]='\0';
        int x = 4, ok = 1;
        while(x<argc){
            if(strcmp(argv[x], "-f") == 0){
                over = 1;
                ++x;
                continue;
            }
            if(strcmp(argv[x], "-m") == 0){
                if(x+1 == argc){
                    ok = 0;
                    break;
                }
                memcpy(tg.msg, argv[x+1], strlen(argv[x+1])+1);
                x+=2;
                continue;
            }
            if(strcmp(argv[x], "-c") == 0){
                if(x+1 == argc){
                    ok = 0;
                    break;
                }
                tg.id=atoi(argv[x+1]);
                x+=2;
                continue;
            }
            ok = 0;
            break;
        }
        if(ok == 0){
            printf("Invalid command\n");
            return 1;
        }
        retr=new_tag(&tg, over);
    }
    else{
        printf("Invalid command\n");
        return 1;
    }
    if(retr == 0 && (strcmp(argv[2], "-a") == 0))
        printf("The tag created successfully\n");
    if(retr == 2)
        printf("This name is already used\nIf you want to overwrite it use -f flag\n");
    else if(retr == 3)
        printf("The given commit_id is invalid\n");
    else if(retr == 4)
        printf("There is no tag which name is %s\n", argv[3]);
    return retr;
}