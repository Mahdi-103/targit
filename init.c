#include "config.c"

int init(){
    if(where_is_inited() != NULL){
        perror("The repo is already initialized");
        return 1;
    }
    if(mkdir(".targit", 0755) != 0) return 1;
    if(chdir(".targit") != 0) exit(1);
    char dir_adr[MAX_ADR_NAME];
    if(getcwd(dir_adr, MAX_ADR_NAME) == NULL) return 1;
    init_conf(dir_adr);
    if(mkdir("stage", 0755) != 0) return 1;
    FILE *f=fopen("tracked", "w");
    if(mkdir("commits", 0755) != 0) return 1;
    chdir("..");
    printf("The repo initialized successfully\n");
    return 0;
}