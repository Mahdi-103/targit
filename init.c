#include "repo.c"

int init(){
    if(where_is_inited() != NULL){
        perror("The repo is already initialized");
        return 1;
    }
    if(mkdir(".targit", 0755) != 0) return 1;
    if(chdir(".targit") != 0) exit(1);
    FILE *f=fopen("config", "w");
    f=fopen("staged", "w");
    if(mkdir("commits", 0755) != 0) return 1;
    chdir("..");
    printf("The repo initialized successfully");
    return 0;
}