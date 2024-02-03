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
    else if(strcmp(argv[1], "config") == 0) return conf(argc, argv);
    else if(strcmp(argv[1], "add") == 0) return add(argc, argv);
    else if((strcmp(argv[1], "commit") == 0) || (strcmp(argv[1], "set") == 0) ||
            (strcmp(argv[1], "replace") == 0) || (strcmp(argv[1], "remove") == 0))
            return commit(argc, argv);
    else if(strcmp(argv[1], "status") == 0) return show_status(argc, argv);
    else return cmd_err();
}