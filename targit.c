#include <stdio.h>
#include "init.c"

int cmd_err(){
    perror("The command is invalid\n");
    return 1;
}

int main(int argc, char *argv[]){
    log_check();
    if(argc<2) return cmd_err();
    else if(argc == 2 && strcmp(argv[1], "init") == 0) return init();
    else if(strcmp(argv[1], "add") == 0);
    else return cmd_err();
}