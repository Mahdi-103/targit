#include "tag.c"

int todo_check(char *path){
    if(0 == NULL) printf("%d", 1/0);
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(path);
    if(strcmp(ff, "txt") == 0){
        char ln[10000];
        while(fgetS(ln, 10000, f) != NULL){
            if(strstr(ln, "TODO") != NULL){
                ret = 2;
                break;
            }
        }
    }
    else if(strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        char ln[10000];
        while(fgetS(ln, 10000, f) != NULL){
            int slsh=0;
            while(slsh<strlen(ln)-1){
                if(ln[slsh] == '/' && ln[slsh+1] == '/')
                    break;
                ++slsh;
            }
            if(strstr(ln+slsh, "TODO") != NULL){
                ret = 2;
                break;
            }
        }
    }
    else ret = 3; 
    fclose(f);
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int eof_blank_space(char *path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(path);
    if(strcmp(ff, "txt") == 0 || strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        char ch1='\0', ch0='\0';
        while((ch1 = getc(f)) != EOF)
            ch0=ch1;
        if(isspace(ch0)) ret = 2;
    }
    else ret = 3; 
    fclose(f);
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int format_check(char *path){
    return 3;
}

int balance_braces(char *path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(path);
    if(strcmp(ff, "txt") == 0 || strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        char ch, st[10000];
        int sz=0;
        while((ch = getc(f)) != EOF){
            if(strchr("()[]{}", ch) != NULL){
                if(sz == 0 || ch == st[sz-1] || abs(ch - st[sz-1]) >= 3){
                    st[sz] = ch;
                    ++sz;
                }
                else --sz;
            }
        }
        if(sz) ret = 2;
    }
    else ret = 3; 
    fclose(f);
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int indentation_check(char *path){
    return 3;
}

int static_error_check(char *path){
    int ret = 0;
    char *ff=frmt(path);
    char command[2 * MAX_ADR_NAME];
    if(strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        if(strcmp(ff, "cpp") == 0)
            strcnct(command, "g++ ", path);
        else 
            strcnct(command, "gcc ", path);
        strcnct(command, command, " -w 2> ");
        strcnct(command, command, repo_path);
        strcnct(command, command, "/tmp-cmp-err");
        //char command = "gcc "path" -w 2> /dev/null";
        system(command);
        FILE *f=fopen(cnct(repo_path, "/tmp-cmp-err"), "r");
        if(getc(f) != EOF) ret = 2; 
        fclose(f);
        remove(cnct(repo_path, "/tmp-cmp-err"));
    }
    else ret = 3;
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int file_size_check(char *path){
    struct stat st;
    if(stat(path, &st) == 0){
        if(st.st_size > 5 * 1024 * 1024 * 8L) return 2;
        return 0;
    }
    return 1;
}

int charecter_limit(char *path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(path);
    if(strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0 || strcmp(ff, "txt") == 0){
        int num = 1;
        char ch;
        while((ch = getc(f)) != EOF)
            ++num;
        if(num > 20000) ret = 2;
    }
    else ret = 3;
    fclose(f);
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int time_limit(char *path){
    int ret = 0;
    char *ff=frmt(path);
    char command[MAX_ADR_NAME];
    if(strcmp(ff, "mp4") == 0 || strcmp(ff, "mp3") == 0 || strcmp(ff, "wav") == 0){
        strcnct(command, "ffprobe -v error -select_streams v:0 -show_entries stream=duration -of default=noprint_wrappers=1:nokey=1 \"", path);
        strcnct(command, command, "\" > ");
        strcnct(command, command, repo_path);
        strcnct(command, command, "/tmp-dur-check");
        system(command);
        FILE *f=fopen(cnct(repo_path, "/tmp-dur-check"), "r");
        double dur;
        fscanf(f, "%lf", &dur); 
        fclose(f);
        remove(cnct(repo_path, "/tmp-cmp-err"));
        if(dur > 600.0) ret = 2;
    }
    else ret = 3;
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int add_hook(char *name){ //cwd is .targitc
    chdir("Hooks");
    if(access(name, F_OK)) return 2;
    if(contains_line(name, "ON")) return 3;
    FILE *f=fopen(name, "w");
    fprintf(f, "ON");
    fclose(f);
    chdir("..");
    return 0;
}

int rem_hook(char *name){ //cwd is .targitc
    chdir("Hooks");
    if(access(name, F_OK)) return 2;
    if(contains_line(name, "OFF")) return 4;
    FILE *f=fopen(name, "w");
    fprintf(f, "OFF");
    fclose(f);
    chdir("..");
    return 0;
}

int lst_hook(int all){ //cwd is .targitc
    chdir("Hooks");
    DIR *dir=opendir(".");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(!is_dir(entry->d_name)){
            if(all || contains_line(entry->d_name, "ON"))
                printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
    chdir("..");
    return 0;
}

int hook_check(char *path){
    if(access(path, F_OK)) return 2;
    else if(!is_staged(path)) return 3;
    
}

int precommit(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    chdir(repo_path);
    if(argc == 2){
        return 0;
    }
    if(strcmp(argv[2], "-u") == 0){
        return 0;
    }
    if(strcmp(argv[2], "hooks") == 0){
        if(argc != 4 || strcmp(argv[3], "list")){
            printf("Invalid command\n");
            return 1;
        }
        return lst_hook(1);
    }
    if(strcmp(argv[2], "applied") == 0){
        if(argc != 4 || strcmp(argv[3], "hooks")){
            printf("Invalid command\n");
            return 1;
        }
        return lst_hook(0);
    }
    int retr;
    if(strcmp(argv[2], "add") == 0){
        if(argc != 5 || strcmp(argv[3], "hook")){
            printf("Invalid command\n");
            return 1;
        }
        retr = add_hook(argv[4]);
    }
    if(strcmp(argv[2], "remove") == 0){
        if(argc != 5 || strcmp(argv[3], "hook")){
            printf("Invalid command\n");
            return 1;
        }
        retr = rem_hook(argv[4]);
    }
    if(retr == 0)
        printf("Applied hooks updated succefully\n");
    else if(retr == 2)
        printf("No such hook\n");
    else if(retr == 3)
        printf("This hook has been already active\n");
    else if(retr == 4)
        printf("This hook has'nt been already active\n");
    return retr;
}