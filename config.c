#include <stdio.h>
#include <stdlib.h>
#include "repo.c"
#include "errno.h"
#define MAX_NME_LNG 100

void init_conf(char *dir_adr){
    char conf[MAX_ADR_NAME];
    memcpy(conf, dir_adr, strlen(dir_adr));
    memcpy(conf+strlen(dir_adr), "/config", 8);
    printf("khir :: %s\n", conf);
    FILE *f=fopen(conf, "w");
    FILE *g=fopen(from_home("/.targits/config"), "r");
    if(g == NULL) exit(1);
    while(fgets(conf, MAX_NME_LNG, g) != NULL){
        int sz=strlen(conf);
        if(conf[sz-1]=='\n') conf[sz-1]='\0';
        fprintf(f, "%s\n", conf);
    }
    fclose(f);
    fclose(g);
    g=fopen(from_home("/.targits/repos"), "a");
    fprintf(g, "%s\n", dir_adr);
    fclose(g);
}

void upd_conf(char *dir_adr, char *usr, char *eml){
    char conf[MAX_ADR_NAME];
    memcpy(conf, dir_adr, strlen(dir_adr));
    memcpy(conf+strlen(dir_adr), "/config", 8);
    FILE *f=fopen(conf, "r");
    if(f == NULL)   return;
    char pre_usr[MAX_NME_LNG], pre_eml[MAX_NME_LNG];
    fgets(pre_usr, MAX_NME_LNG, f);{
        int sz=strlen(pre_usr);
        if(pre_usr[sz-1]=='\n') pre_usr[sz-1]='\0';
    }
    fgets(pre_eml, MAX_NME_LNG, f);{
        int sz=strlen(pre_eml);
        if(pre_eml[sz-1]=='\n') pre_eml[sz-1]='\0';
    }
    fclose(f);
    if(strlen(usr) == 0) usr=pre_usr;
    if(strlen(eml) == 0) eml=pre_eml;
    f=fopen(conf, "w");
    fprintf(f, "%s\n%s\n", usr, eml);
    fclose(f);
}

void glob_upd_conf(char *usr, char *eml){
    upd_conf(from_home("/.targits"), usr, eml);
    FILE *f=fopen(from_home("/.targits/repos"), "r");
    if(f == NULL) exit(1);
    char dir_adr[MAX_ADR_NAME];
    while(fscanf(f, "%s", dir_adr) != EOF)
        upd_conf(dir_adr, usr, eml);
    fclose(f);
}

int conf(int argc, char *argv[]){
    if(argc == 2){
        perror("Please declare what you want to config");
        return 1;
    }
    char usr[MAX_NME_LNG], eml[MAX_NME_LNG];
    *usr=*eml='\0';
    int id=2;
    if(strcmp(argv[2], "-global") == 0)
        ++id;
    char *dir=where_is_inited();
    if(id==2 && dir == NULL){
        perror("The repo is not initialized\n");
        return 1;
    }
    char *tmp;
    if(strcmp(argv[id], "user.name") == 0) tmp=usr;
    else if(strcmp(argv[id], "user.email") == 0) tmp=eml;
    else{
        perror("You can only config the user.name or user.email");
        return 1;
    }
    if(id+1==argc){
        fprintf(stderr, "The %s should not be empty: %s\n", argv[id], strerror(errno));
        return 1;
    }
    if(id+2==argc)
        memcpy(tmp, argv[id+1], strlen(argv[id+1])+1);
    else{
        fprintf(stderr, "If you want to use a %s with whitespaces, you have to put it in double quotations: %s\n", argv[id], strerror(errno));
        return 1;
    }
    if(id==3)
        glob_upd_conf(usr, eml);
    else
        upd_conf(dir, usr, eml);
    printf("%s updated!\n", argv[id]);
    return 0;
}