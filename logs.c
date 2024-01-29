#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<string.h>
#define MAX_ADR_NAME 1000

char *from_home(char *a){
    static char adr[MAX_ADR_NAME];
    int sz=strlen(getenv("HOME"));
    memcpy(adr, getenv("HOME"), sz);
    adr[sz]='/';
    memcpy(adr+sz+1, a, strlen(a)+1);
    return adr;
}

void log_check(){
    char *adr=from_home("/.targits");
    DIR *dir=opendir(adr);
    if(dir!=NULL) return;
    if(mkdir(adr, 0755) != 0) exit(1);
    FILE *f=fopen(from_home("/.targits/config"), "w");
    fprintf(f, "\n\n");
    fclose(f);
    f=fopen(from_home("/.targits/repos"), "w");
    fclose(f);
}