#include <stdio.h>
#include "init.c"

int cmd_err(){
    perror("The command is invalid\n");
    return 1;
}

int main(int argc, char *argv[]){
    pre_check();
    if(argc<2) return cmd_err();
    else if(argc == 2 && strcmp(argv[1], "init") == 0) return init();
    else if(strcmp(argv[1], "config") == 0) return conf(argc, argv);
    else if(strcmp(argv[1], "add") == 0) return add(argc, argv);
    else if((strcmp(argv[1], "commit") == 0) || (strcmp(argv[1], "set") == 0) ||
            (strcmp(argv[1], "replace") == 0) || (strcmp(argv[1], "remove") == 0))
            return commit(argc, argv);
    else if(strcmp(argv[1], "status") == 0) return show_status(argc, argv);
    else if(strcmp(argv[1], "branch") == 0) return branch(argc, argv);
    else if(strcmp(argv[1], "log") == 0) return logg(argc, argv);
    else if(strcmp(argv[1], "checkout") == 0) return checkout(argc, argv);
    else if(strcmp(argv[1], "reset") == 0) return reset(argc, argv);
    else if(strcmp(argv[1], "tag") == 0) return tag_opr(argc, argv);
    else if(strcmp(argv[1], "pre-commit") == 0) return precommit(argc, argv);
    else if(strcmp(argv[1], "diff") == 0) return diff(argc, argv);
    else if(strcmp(argv[1], "grep") == 0) return grep(argc, argv);
    else return cmd_err();
}