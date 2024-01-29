#include <stdio.h>
#include <stdlib.h>
#include "repo.c"
#include "errno.h"
#define MAX_NME_LNG 100

void init_conf(char *dir_adr){
    char conf[MAX_ADR_NAME];
    memcpy(conf, dir_adr, strlen(dir_adr));
    memcpy(conf+strlen(dir_adr), "config", 7);
    FILE *f=fopen(conf, "w");
    FILE *g=fopen(from_home("/.targits/config"), "r");
    while(fgets(conf, MAX_NME_LNG, g) != NULL){
        fprintf(f, "%s\n", conf);
    }
    fclose(f);
    fclose(g);
}

void upd_conf(char *dir_adr, char *usr, char *eml){
    char conf[MAX_ADR_NAME];
    memcpy(conf, dir_adr, strlen(dir_adr));
    memcpy(conf+strlen(dir_adr), "/config", 8);
    printf("khor :: %s\n", conf);
    FILE *f=fopen(conf, "r");
    char pre_usr[MAX_NME_LNG], pre_eml[MAX_NME_LNG];
    fgets(pre_usr, MAX_NME_LNG, f);
    fgets(pre_eml, MAX_NME_LNG, f);
    fclose(f);
    if(strlen(usr) == 0) usr=pre_usr;
    if(strlen(eml) == 0) eml=pre_eml;
    f=fopen(conf, "w");
    fprintf(f, "%s\n%s\n", usr, eml);
    fclose(f);
}

void glob_upd_conf(char *usr, char *eml){
    upd_conf(from_home("/.targits"), usr, eml);
    FILE *f=fopen(from_home("/.targits/config"), "r");
    char dir_adr[MAX_ADR_NAME];
    while(fgets(dir_adr, MAX_ADR_NAME, f) != NULL)
        upd_conf(dir_adr, usr, eml);
    fclose(f);
}

int conf(int argc, char *argv[]){
    printf("%d\n", argc);
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
    printf("aha\n");
    if(id+1==argc){
        fprintf(stderr, "The %s should not be empty: %s\n", argv[id], strerror(errno));
        return 1;
    }
    if(id+2==argc)
        memcpy(tmp, argv[id+1], strlen(argv[id+1])+1);
    else{
        fprintf(stderr, "If you want to use a %s with whitspaces, you have to put it in double quotations: %s\n", argv[id], strerror(errno));
        return 1;
    }
    printf("%d khob\n", id);
    if(id==3)
        glob_upd_conf(usr, eml);
    else{
        printf("%s\n", dir);
        upd_conf(dir, usr, eml);
    }
    printf("%s updated!\n", argv[id]);
    return 0;
}

int main(int argc, char *argv[]){
    return conf(argc, argv);
}