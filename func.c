#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#define MAX_ADR_NAME 1000
#define MAX_NME_LNG 100

int flecpy(FILE *paste, FILE *source){
    char ch;
    while((ch=getc(source)) != EOF)
        fprintf(paste, "%c", ch);
}

char *cnct(const char *a, const char * b){
    static char *c;
    c=malloc(strlen(a)+strlen(b)+1);
    memcpy(c, a, strlen(a));
    memcpy(c+strlen(a), b, strlen(b)+1);
    return c;
}

char *from_home(char *a){
    static char adr[MAX_ADR_NAME];
    int sz=strlen(getenv("HOME"));
    memcpy(adr, getenv("HOME"), sz);
    adr[sz]='/';

    memcpy(adr+sz+1, a, strlen(a)+1);
    return adr;
}

void deblank(char *s){
    char *t=s;
    while(1){
        int x=0;
        while(isspace(s[x])) ++x;
        if(x) memmove(s, s+x, strlen(s+x)+1);
        while(strlen(s) && !isspace(*s)) ++s;
        if(strlen(s) == 0)
            break;
        ++s;   
    }
}
