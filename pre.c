#include "func.c"

void pre_check(){
    char *adr=from_home("/.targits");
    DIR *dir=opendir(adr);
    if(dir!=NULL) return;
    if(mkdir(adr, 0755) != 0) exit(1);
    FILE *f=fopen(from_home("/.targits/config"), "w");
    fprintf(f, "default_user\ndefault@mail.com\n");
    fclose(f);
    f=fopen(from_home("/.targits/repos"), "w");
    fclose(f);
}