/*
    This program does stress test for the proxy server. 
    It creates set of processes, each of which connects via
    psql utility and does set of simple random queries.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define N_CMDS 2
#define N_TIMES 100
#define N_PROCESSES 100

#define STDIN  0
#define STDOUT 1
#define STDERR 2

void process() {
    int fd[2];
    if (pipe(fd)) {
        fprintf(stderr, "Could not open pipe\n");
        exit(1);
    }
    int id = fork();
    if (id == 0) {
        dup2(fd[0], STDIN);
        close(fd[0]);
        close(fd[1]);
        char* args[] = {"psql", "--host=127.0.0.1", "--port=1234", "--dbname=testdb", NULL};
        execvp("psql", args);
        fprintf(stderr, "Could not exec the psql utility\n");
    }
    else {
        close(fd[0]);
        const char *commands[N_CMDS] = {
            "select * from users;\n",
            "insert into users (name) values ('igor');\n"
        };
        for (int i = 0; i < N_TIMES; i++) {
            usleep(rand() % 100 * 1000); // 0.05 seconds in average
            const char *cmd = commands[rand() % N_CMDS];
            write(fd[1], cmd, strlen(cmd));
        }
        close(fd[1]);
        wait(NULL);
    }
    exit(0);
}

int main(int argc, char **argv) {
    for (int i = 0; i < N_PROCESSES; i++) {
        usleep(rand() % 1000 * 1000); // 0.5 second in average
        if (fork() == 0) {
            process();
        }
    }
    while (wait(NULL) > 0);
    
    return 0;
}