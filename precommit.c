#include "tag.c"
char res_hook_check[10000];
#define rhc res_hook_check+strlen(res_hook_check)

int todo_check(char *path, char *real_path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(real_path);
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
            while(slsh<(int)(strlen(ln)-1)){
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

int eof_blank_space(char *path, char *real_path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(real_path);
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

int format_check(char *path, char *real_path){
    return 3;
}

int balance_braces(char *path, char *real_path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(real_path);
    if(strcmp(ff, "txt") == 0 || strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        char ch, st[10000];
        int sz=0;
        while((ch = getc(f)) != EOF){
            if(strchr("()[]{}", ch) != NULL){
                if(sz == 0 || (ch - st[sz-1] != 2 && ch - st[sz-1] != 1)){
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

int indentation_check(char *path, char *real_path){
    return 3;
}

int static_error_check(char *path, char *real_path){
    int ret = 0;
    char *ff=frmt(real_path);
    char command[2 * MAX_ADR_NAME];
    int should_rem=0;
    if(strcmp(ff, "cpp") == 0 || strcmp(ff, "c") == 0){
        if(strlen(frmt(path)) == 0){
            flecpy_path(cnct(path, ff-1), path);
            path = cnct(path, ff-1);
        }
        if(strcmp(ff, "cpp") == 0)
            strcnct(command, "g++ ", path);
        else 
            strcnct(command, "gcc ", path);
        strcnct(command, command, " -o ");
        strcnct(command, command, repo_path);
        strcnct(command, command, "/tmp-cmp.exe");
        strcnct(command, command, " -w 2> ");
        strcnct(command, command, repo_path);
        strcnct(command, command, "/tmp-cmp-err");
        system(command);
        FILE *f=fopen(cnct(repo_path, "/tmp-cmp-err"), "r");
        if(getc(f) != EOF) ret = 2; 
        fclose(f);
        delete(cnct(repo_path, "/tmp-cmp-err"));
        delete(cnct(repo_path, "/tmp-cmp.exe"));
        if(should_rem) remove(path);
    }
    else ret = 3;
    return ret;// 0 if pass, 2 if fail, 3 if skip
}

int file_size_check(char *path, char *real_path){
    struct stat st;
    if(stat(path, &st) == 0){
        if(st.st_size > 5 * 1024 * 1024L) return 2;
        return 0;
    }
    return 1;
}

int character_limit(char *path, char *real_path){
    int ret = 0;
    FILE *f=fopen(path, "r");
    char *ff=frmt(real_path);
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

int time_limit(char *path, char *real_path){
    int ret = 0;
    char *ff=frmt(real_path);
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

int add_hook(char *name){ //cwd is .targit
    chdir(repo_path);
    chdir("Hooks");
    if(access(name, F_OK)) return 2;
    if(contains_line(name, "ON")) return 3;
    FILE *f=fopen(name, "w");
    fprintf(f, "ON");
    fclose(f);
    chdir("..");
    return 0;
}

int rem_hook(char *name){ //cwd is .targit
    chdir(repo_path);
    chdir("Hooks");
    if(access(name, F_OK)) return 2;
    if(contains_line(name, "OFF")) return 4;
    FILE *f=fopen(name, "w");
    fprintf(f, "OFF");
    fclose(f);
    chdir("..");
    return 0;
}

int lst_hook(int all, int chap){ //cwd could be anywhere
    char cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    chdir(repo_path);
    chdir("Hooks");
    DIR *dir=opendir(".");
    struct dirent *entry;
    int ret = 2;
    while((entry = readdir(dir)) != NULL){
        if(!is_dir(entry->d_name)){
            if(all || contains_line(entry->d_name, "ON")){
                if(chap)
                    printf("%s\n", entry->d_name);
                ret = 0;
            }
        }
    }
    closedir(dir);
    chdir(cwd);
    return ret;
}

void prnt_natije(char *prnt, char *hook_name, int ret){
    sprintf(prnt, "\"");
    sprintf(prnt+1, "%s", hook_name);
    sprintf(prnt+strlen(prnt), "\"");
    char retr[20];
    if(ret == 0) memcpy(retr, "\033[0;32mPASSED\033[0m", 18);
    else if(ret == 2) memcpy(retr, "\033[0;31mFAILED\033[0m", 18);
    else if(ret == 3) memcpy(retr, "\033[0;38mSKIPPED\033[0m", 19);
    for(int i = strlen(hook_name) + strlen(retr) ; i < 84 ; ++i)
        sprintf(prnt+strlen(prnt), ".");
    sprintf(prnt+strlen(prnt), "%s\n", retr);
}

int hook_check(char *path, char *real_path, int stg){ //cwd is anywhere and path is not absolute
    char abs_P[MAX_ADR_NAME], abs_P_r[MAX_ADR_NAME], cwd[MAX_ADR_NAME];
    if(getcwd(cwd, MAX_ADR_NAME) == NULL) return 1;
    if(lst_hook(0, 0))  return 5;
    if((path = abs_path(abs_P, path)) == NULL) return 1;
    if((real_path = abs_path(abs_P_r, real_path)) == NULL) return 1;
    if(access(path, F_OK) || is_dir(path)) return 2;
    if(!stg){
        if(!in_repo(path)) return 3;
        if(!is_staged(path)) return 4;
    }
    if(is_staged(real_path)) path = real_path;
    chdir(repo_path);
    chdir("Hooks");
    memset(res_hook_check, 0, 10000);
    if(contains_line("todo-check", "ON"))
        prnt_natije(rhc, "todo-check", todo_check(path, real_path));
    if(contains_line("eof-blank-space", "ON"))
        prnt_natije(rhc, "eof-blank-space", eof_blank_space(path, real_path));
    if(contains_line("format-check", "ON"))
        prnt_natije(rhc, "format-check", format_check(path, real_path));
    if(contains_line("balance-braces", "ON"))
        prnt_natije(rhc, "balance-braces", balance_braces(path, real_path));
    if(contains_line("indentation-check", "ON"))
        prnt_natije(rhc, "indentation-check", indentation_check(path, real_path));
    if(contains_line("static-error-check", "ON"))
        prnt_natije(rhc, "static-error-check", static_error_check(path, real_path));
    if(contains_line("file-size-check", "ON"))
        prnt_natije(rhc, "file-size-check", file_size_check(path, real_path));
    if(contains_line("character-limit", "ON"))
        prnt_natije(rhc, "character-limit", character_limit(path, real_path));
    if(contains_line("time-limit", "ON"))
        prnt_natije(rhc, "time-limit", time_limit(path, real_path));
    chdir(cwd);
    return 0; // 0 if ok, 2 if doesn't exit, 3 if out of repo, 4 if not staged
}

int precommit(int argc, char *argv[]){
    if(where_is_inited() == NULL){
        printf("The repo is not initialized\n");
        return 1;
    }
    if(argc == 2){
        chdir(repo_path);
        if(stage_empty()){
            printf("There is no staged file to be checked\n");
            return 0;
        }
        if(lst_hook(0, 0)){
            printf("No applied hook\n");
            return 0;
        }
        chdir("stage");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(is_ok_dir(entry->d_name)){
                chdir(entry->d_name);
                char path_repo[MAX_ADR_NAME];
                FILE *f=fopen("file_path", "r");
                fgetS(path_repo, MAX_ADR_NAME, f);
                fclose(f);
                hook_check("file", path_repo, 1);
                if(strlen(res_hook_check)){
                    printf("\"%s\":\n%s\n", path_repo+strlen(repo_path)-7, res_hook_check);
                }
                chdir("..");
            }
        }
        return 0;
    }
    int retr;
    if(strcmp(argv[2], "-u") == 0){
        return 0;
    }
    else if(strcmp(argv[2], "hooks") == 0){
        if(argc != 4 || strcmp(argv[3], "list")){
            printf("Invalid command\n");
            return 1;
        }
        return lst_hook(1, 1);
    }
    else if(strcmp(argv[2], "applied") == 0){
        if(argc != 4 || strcmp(argv[3], "hooks")){
            printf("Invalid command\n");
            return 1;
        }
        if(lst_hook(0, 1))
            printf("No applied hook\n");
        return 0;
    }
    else if(strcmp(argv[2], "add") == 0){
        chdir(repo_path);
        if(argc != 5 || strcmp(argv[3], "hook")){
            printf("Invalid command\n");
            return 1;
        }
        retr = add_hook(argv[4]);
    }
    else if(strcmp(argv[2], "remove") == 0){
        chdir(repo_path);
        if(argc != 5 || strcmp(argv[3], "hook")){
            printf("Invalid command\n");
            return 1;
        }
        retr = rem_hook(argv[4]);
    }
    else if(strcmp(argv[2], "-f") == 0){
        if(argc == 3){
            printf("Please enter the file names\n");
            return 1;
        }
        if(lst_hook(0, 0)){
            printf("No applied hook\n");
            return 0;
        }
        for(int i = 3 ; i < argc ; ++i){
            retr = hook_check(argv[i], argv[i], 0);
            if(retr == 0)
                printf("\"%s\":\n%s\n", argv[i], res_hook_check);
            else if(retr == 2)
                printf("No file which path is %s\n", argv[i]);
            else if(retr == 3)
                printf("%s is out of repository\n", argv[i]);
            else if(retr == 4)
                printf("%s has changes that are not staged\n", argv[i]);
            else
                exit(1);
        }
        return 0;
    }
    else{
        printf("Invalid command\n");
        return 0;
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