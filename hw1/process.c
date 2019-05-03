#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int resolve_path(char *cmd, char *res) {

    char *paths = (char *)malloc(4096);
    strcpy(paths, getenv("PATH"));

    char *path;

    while ((path = strsep(&paths, ":")) != NULL) {

        DIR *dir;
        struct dirent *dp;

        if ((dir = opendir(path)) == NULL) {
            perror("can not open the directory");
        }

        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(cmd, dp->d_name) == 0) {
                strcpy(res, path);
                strcat(res, "/");
                strcat(res, cmd);
                return 0;
            }
        }
    }
    free(paths);
    return -1;
}

int launch_process(char *cmd, char *args[]) {
    char full_cmd[4096];
    int fds[2];
    pipe(fds);

    if (access(cmd, F_OK) != -1){
        strcpy(full_cmd, cmd);
    }
    else{
        if (resolve_path(cmd, full_cmd) == -1)
            return -1;
    }

    int pid = fork();

    if (pid == 0) {
        close(fds[0]);
        dup2(fds[1], 1);
        close(fds[1]);
        execv(full_cmd, args);
        exit(-1);
    }

    int status;

    wait(&status);

    close(fds[1]);
    char buf[4096];
    int n = read(fds[0], buf, sizeof(buf));
    close(fds[0]);
    write(1, buf, n);
    return 0;
}

int launch_process2(char *cmd, char *args[], char *to, char *file) {
    char full_cmd[4096];
    int fds[2];
    pipe(fds);

    if (access(cmd, F_OK) != -1){
        strcpy(full_cmd, cmd);
    }
    else{
        if (resolve_path(cmd, full_cmd) == -1)
            return -1;
    }

    int pid = fork();

    if (pid == 0) {
        int fn;

        if (strcmp(to,"<") == 0) {
            fn = fileno(fopen(file, "r"));
            dup2(fn, 0);
            dup2(fds[1], 1);
        }

        if (strcmp(to,">") == 0) {
            fn = fileno(fopen(file, "w"));
            dup2(fn, 1);
        }

        close(fds[0]);
        close(fds[1]);
        close(fn);
        execv(full_cmd, args);
        exit(-1);
    }

    int status;

    wait(&status);

    close(fds[1]);
    char buf[4096];
    int n = read(fds[0], buf, sizeof(buf));
    close(fds[0]);
    write(1, buf, n);
    printf("end of the program");
    return 0;
}