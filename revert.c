#include "tag.c"

int revert(int argc, char *argv[]){ 
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    chdir(repo_path);
    delete("stage");
    if(mkdir("stage", 0755) != 0) return 1;
    int id, com=1;
    char msg[80];
    msg[0]='\0';
    if(argc == 2){
        printf("Invalid command\n");
        return 1;
    }
    int x = 2;
    if(strcmp(argv[2], "-n") == 0){
        com=0;
        if(argc > 4){
            printf("Invalid command\n");
            return 1;
        }
        if(argc == 3){
            char *tmp_argv[3]={"targit", "checkout", "HEAD"};
            checkout_opr(3, tmp_argv);
            id = atoi(head());
        }
        else 
            id = atoi(argv[3]);
        x=-1;
    }
    if(strcmp(argv[2], "-m") == 0){
        if(argc == 3){
            printf("Invalid command\n");
            return 1;
        }
        memcpy(msg, argv[3], strlen(argv[3])+1);
        x=4;
    }
    if(x != -1){
        if(x+1 != argc){
            printf("Invalid command\n");
            return 1;
        }
        if(strncmp(argv[x], "HEAD-", 5) == 0){
            int n;
            sscanf(argv[x], "HEAD-%d", &n);
            char *tmp_argv[3]={"targit", "checkout", "HEAD"};
            checkout_opr(3, tmp_argv);
            id = atoi(head());
            for(int i=0;i<n;++i){
                id = par_com(id);
                if(id == -1){
                    printf("HEAD doesn't have %d parents\n", n);
                    return 1;
                }
            }
        }
        else
            id = atoi(argv[x]);
    }
    if(x != -1 && strlen(msg) == 0){
        chdir(cnct("commits/", iots(id)));
        FILE *f=fopen("message", "r");
        fgetS(msg, 80, f);
        fclose(f);
        chdir("../..");
    }
    go_to(id);
    
}