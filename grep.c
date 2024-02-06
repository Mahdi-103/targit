#include "diff.c"

void chap_rangi(char *sent, char *word){
    char *h=strstr(sent, word);
    while(*sent != '\0'){
        while(sent!=h){
            printf("%c", *sent);
            ++sent;
        }
        printf("\033[0;31m");
        printf("%s", word);
        printf("\033[0m");
        sent+=strlen(word);
        if(strlen(sent) == 0)
            break;
        h=strstr(sent, word);
        if(h == NULL){
            printf("%s", sent);
            break;
        }
    }
    printf("\n");
}

int grep_check(char *path, char *word, int line){
    char ln[10000];
    FILE *f=fopen(path, "r");
    for(int i=1;fgetS(ln, 10000, f);++i){
        if(strstr(ln, word) != NULL){
            if(line)
                printf("line %d:\n", i);
            chap_rangi(ln, word);
        }
    }
    fclose(f);
    return 0;
}

int grep_opr(char *path, char *word, int line){
    char abs_p[MAX_ADR_NAME];
    if((path = abs_path(abs_p, path)) == NULL) return 1;
    if(access(path, F_OK) || is_dir(path)) return 2;
    return grep_check(path, word, line);
}

int grep(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(argc < 6){
        printf("Invalid command\n");
        return 1;
    }
    char *path=NULL, *word=NULL;
    int x=2, ret=0, line = 0;
    while(x<argc){
        if(strcmp(argv[x], "-f") == 0){
            if(x+1 == argc || path != NULL){
                ret = 1;
                break;
            } 
            path=argv[x+1];
            x+=2;
        }
        else if(strcmp(argv[x], "-p") == 0){
            if(x+1 == argc || word != NULL){
                ret = 1;
                break;
            }
            word = argv[x+1];
            x+=2;
        }
        else if(strcmp(argv[x], "-n") == 0){
            line = 1;
            ++x;
        }
        else{
            ret = 1;
            break;
        }
    }
    if(ret || word == NULL || path == NULL){
        printf("Invalid command\n");
        return 1;
    }
    ret = grep_opr(path, word, line);
    if(ret == 2)
        printf("%s is not valid file path\n", path);
    return ret;
}