#include "precommit.c"

int init(){
    if(where_is_inited() != NULL){
        printf("The repo is already initialized\n");
        return 1;
    }
    if(mkdir(".targit", 0755) != 0) return 1;
    if(chdir(".targit") != 0) exit(1);
    char dir_adr[MAX_ADR_NAME];
    if(getcwd(dir_adr, MAX_ADR_NAME) == NULL) return 1;
    init_conf(dir_adr);
    if(mkdir("tags", 0755) != 0) return 1;
    if(mkdir("stage", 0755) != 0) return 1;
    if(mkdir("Hooks", 0755) != 0) return 1;
    if(mkdir("commits", 0755) != 0) return 1;
    if(mkdir("branches", 0755) != 0) return 1;
    if(mkdir("shortcuts", 0755) != 0) return 1;
    if(mkdir("branches/master", 0755) != 0) return 1;
    if(mkdir("shortcuts/message", 0755) != 0) return 1;
    FILE *f=fopen("tracked", "w");
    fclose(f);
    f=fopen("HEAD", "w");
    fprintf(f, "-1");
    fclose(f);
    f=fopen("current_branch", "w");
    fprintf(f, "master");
    fclose(f);
    f=fopen("commits/num", "w");
    fprintf(f, "-1");
    fclose(f);
    f=fopen("branches/master/first_commit", "w");
    fprintf(f, "-1");
    fclose(f);
    f=fopen("branches/master/HEAD", "w");
    fprintf(f, "-1");
    fclose(f);
    chdir("Hooks");
    f=fopen("todo-check", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("eof-blank-space", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("format-check", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("balance-braces", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("indentation-check", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("static-error-check", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("file-size-check", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("character-limit", "w");
    fprintf(f, "OFF");
    fclose(f);
    f=fopen("time-limit", "w");
    fprintf(f, "OFF");
    fclose(f);
    chdir("..");
    printf("The repo initialized successfully\n");
    return 0;
}