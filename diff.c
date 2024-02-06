#include "precommit.c"

int diff_check(char *path_1, char *name_1, char *path_2, char *name_2){
    FILE *f=fopen(path_1, "r");
    FILE *g=fopen(path_2, "r");
    char ln_1[10000], ln_2[10000];
    while(fgetS(ln_1, 10000, f) != NULL){
        if(fgetS(ln_2, 10000, g) != NULL){
            int st_1=0, st_2=0;
            while(!isspace(ln_1[st_1]))
                ++st_1;
            while(!isspace(ln_2[st_2]))
                ++st_2;
            ln_1[st_1++]='\0';
            ln_2[st_2++]='\0';
            if(strcmp(ln_1+st_1, ln_2+st_2)){
                printf("«««««\n%s-%s:\n", name_1, ln_1);
                printf("\033[0;36m%s\033[0m\n", ln_1 + st_1);
                printf("%s-%s:\n", name_2, ln_2);
                printf("\033[0;33m%s\033[0m\n»»»»»\n\n", ln_2 + st_2);
            }
        }
        else{
            int st_1;
            while(!isspace(ln_1[st_1]))
                ++st_1;
            printf("«««««\n%s-%s:\n", name_1, ln_1);
            printf("\033[0;36m%s\033[0m\n", ln_1 + st_1);
            printf("%s:\n\n»»»»»\n\n", name_2);
        }
    }
    while(fgetS(ln_2, 10000, g) != NULL){
        int st_2;
        while(!isspace(ln_2[st_2]))
            ++st_2;
        ln_2[st_2++]='\0';
        printf("«««««\n%s:\n\n", name_1);
        printf("%s-%s:\n", name_2, ln_2);
        printf("\033[0;33m%s\033[0m\n»»»»»\n\n", ln_2 + st_2);
    }
    fclose(f);
    fclose(g);
}

int diff_opr(char *path_1, int bg_1, int end_1, char *path_2, int bg_2, int end_2){
    char abs_p_1[MAX_ADR_NAME], abs_p_2[MAX_ADR_NAME], ln[10000];
    char *tmp_path_1=path_1, *tmp_path_2=path_2;
    if(access(path_1, F_OK) || is_dir(path_1)) return 2;
    if(access(path_2, F_OK) || is_dir(path_2)) return 3;
    if((path_1 = abs_path(abs_p_1, path_1)) == NULL) return 1;
    if((path_2 = abs_path(abs_p_2, path_2)) == NULL) return 1;
    chdir(repo_path);
    FILE *f=fopen(path_1, "r");
    FILE *g=fopen("tmp-file-1", "w");
    for(int i = 1 ;fgetS(ln, 10000, f) != NULL; ++i){
        if(i>=bg_1 && i<=end_1){
            while(strlen(ln) && isspace(ln[strlen(ln)-1]))
                ln[strlen(ln) - 1] = '\0';
            int st = 0;
            while(isspace(ln[st]))
                ++st;
            if(strlen(ln+st))
                fprintf(g, "%d %s\n", i, ln+st);
        }
    }
    fclose(g);
    fclose(f);
    f=fopen(path_2, "r");
    g=fopen("tmp-file-2", "w");
    for(int i = 1 ;fgetS(ln, 10000, f) != NULL; ++i){
        if(i>=bg_2 && i<=end_2){
            while(strlen(ln) && isspace(ln[strlen(ln)-1]))
                ln[strlen(ln) - 1] = '\0';
            int st = 0;
            while(isspace(ln[st]))
                ++st;
            if(strlen(ln+st))
                fprintf(g, "%d %s\n", i, ln+st);
        }
    }
    fclose(g);
    fclose(f);
    diff_check("tmp-file-1", tmp_path_1, "tmp-file-2", tmp_path_2);
}

int diff(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(argc<5){
        printf("Invalid command\n");
        return 1;
    }
    if(strcmp(argv[2], "-f") == 0){
        int bg_1=0, end_1=__INT32_MAX__, bg_2=0, end_2=__INT32_MAX__;
        int x=5, ret = 0;
        while(x<argc){
            if(x+1==argc){
                ret = 1;
                break;
            }
            if(strcmp(argv[x], "-line1") == 0){
                if(bg_1 != 0 || end_1 != __INT32_MAX__){
                    ret =1;
                    break;
                }
                sscanf(argv[x+1], "%d-%d", &bg_1, &end_1);
            }
            else if(strcmp(argv[x], "-line2") == 0){
                if(bg_2 !=0 || end_2 != __INT32_MAX__){
                    ret = 1;
                    break;
                }
                sscanf(argv[x+1], "%d-%d", &bg_2, &end_2);
            }
            else{
                ret = 1;
                break;
            }
            x+=2;
        }
        if(ret){
            printf("Invalid command\n");
            return 1;
        }
        ret = diff_opr(argv[3], bg_1, end_1, argv[4], bg_2, end_2);
        if(ret == 2)
            printf("%s is not a valid file path\n", argv[3]);
        else if(ret == 3)
            printf("%s is not a valid file path\n", argv[4]);
        return ret;
    }
    else{
        printf("Invalid command\n");
        return 1;
    }
}