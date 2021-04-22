#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>


jmp_buf env;

void handler(int signum) {
    fprintf(stderr, "read timeout in second child, killing both children.\n");
    longjmp(env, 1);
    exit(1);
}



int main(int argc, char* argv[]) {

    signal(SIGALRM, handler);


    pid_t p[2];

    int fid[2];
    pipe(fid);

    if (argc == 1) {
        if ((p[0] = fork()) == 0) {
            dup2(fid[1], STDOUT_FILENO);
            close(fid[1]);
            fprintf(stderr, "Child 1 process id : %d\n", getpid());
            execl("q2", "q2", NULL);
            exit(0);
        }
        if ((p[1] = fork()) == 0) {
            close(fid[1]);
            dup2(fid[0], STDIN_FILENO);
            close(fid[0]);
            fprintf(stderr, "Child 2 process id : %d\n", getpid());
            execl("q3", "q3", NULL);
            exit(0);
        }
    }
    else if (argc == 2 && argv[1][0] == '-') {
        if ((p[0] = fork()) == 0) {
            dup2(fid[1], STDOUT_FILENO);
            close(fid[1]);
            fprintf(stderr, "Child 1 process id : %d\n", getpid());
            execl("q2", "q2", argv[1], NULL);
            exit(0);
        }
        if ((p[1] = fork()) == 0) {
            close(fid[1]);
            dup2(fid[0], STDIN_FILENO);
            close(fid[0]);
            fprintf(stderr, "Child 2 process id : %d\n", getpid());
            execl("q3", "q3", NULL);
            exit(0);
        }
    }

    else if (argc == 2) {
        int inRed = open(argv[1], O_RDONLY);
        if (inRed == -1) {
            fprintf(stderr, "Input file does not exist !!\n");
            exit(-1);
        }
        dup2(inRed, STDIN_FILENO);
        close(inRed);

        if ((p[0] = fork()) == 0) {
            dup2(fid[1], STDOUT_FILENO);
            close(fid[1]);
            fprintf(stderr, "Child 1 process id : %d\n", getpid());
            execl("q2", "q2", NULL);
            exit(0);
        }
        if ((p[1] = fork()) == 0) {
            close(fid[1]);
            dup2(fid[0], STDIN_FILENO);
            close(fid[0]);
            fprintf(stderr, "Child 2 process id : %d\n", getpid());
            execl("q3", "q3", NULL);
            exit(0);
        }
    }
    else if (argc == 3 && argv[1][0] == '-') {
        int inRed = open(argv[1], O_RDONLY);
        if (inRed == -1) {
            fprintf(stderr, "Input file does not exist !!\n");
            exit(-1);
        }
        dup2(inRed, STDIN_FILENO);
        close(inRed);

        if ((p[0] = fork()) == 0) {
            dup2(fid[1], STDOUT_FILENO);
            close(fid[1]);
            fprintf(stderr, "Child 1 process id : %d\n", getpid());
            execl("q2", "q2", argv[1], NULL);
            exit(0);
        }
        if ((p[1] = fork()) == 0) {
            close(fid[1]);
            dup2(fid[0], STDIN_FILENO);
            close(fid[0]);
            fprintf(stderr, "Child 2 process id : %d\n", getpid());
            execl("q3", "q3", NULL);
            exit(0);
        }
    }

    else if (argc == 3) {
        int inRed = open(argv[1], O_RDONLY);
        if (inRed == -1) {
            fprintf(stderr, "Input file does not exist !!\n");
            exit(-1);
        }
        int outRed = creat(argv[2], 0644);
        dup2(inRed, STDIN_FILENO);
        dup2(outRed, STDOUT_FILENO);
        close(inRed);
        close(outRed);

        if ((p[0] = fork()) == 0) {
            dup2(fid[1], STDOUT_FILENO);
            close(fid[1]);
            fprintf(stderr, "Child 1 process id : %d\n", getpid());
            execl("q2", "q2", NULL);
            exit(0);
        }
        if ((p[1] = fork()) == 0) {
            close(fid[1]);
            dup2(fid[0], STDIN_FILENO);
            close(fid[0]);
            fprintf(stderr, "Child 2 process id : %d\n", getpid());
            execl("q3", "q3", NULL);
            exit(0);
        }
    }



    else if (argc == 4) {
    int inRed = open(argv[2], O_RDONLY);
    if (inRed == -1) {
        fprintf(stderr, "Input file does not exist !!");
        exit(-1);
    }
    int outRed = creat(argv[3], 0644);
    dup2(inRed, STDIN_FILENO);
    dup2(outRed, STDOUT_FILENO);
    close(inRed);
    close(outRed);

    if ((p[0] = fork()) == 0) {
        dup2(fid[1], STDOUT_FILENO);
        close(fid[1]);
        fprintf(stderr, "Child 1 process id : %d\n", getpid());
        execl("q2", "q2", argv[1], NULL);
        exit(0);
    }
    if ((p[1] = fork()) == 0) {
        close(fid[1]);
        dup2(fid[0], STDIN_FILENO);
        close(fid[0]);
        fprintf(stderr, "Child 2 process id : %d\n", getpid());
        execl("q3", "q3", NULL);
        exit(0);
    }

        }

        if (setjmp(env) != 0) {

            kill(p[0], SIGTERM);
            kill(p[1], SIGTERM);

            wait(NULL);
            wait(NULL);

            exit(1);
        }


        alarm(15);

        close(fid[1]);
        close(fid[0]);

        wait(NULL);
        wait(NULL);

        fprintf(stderr, "Normal children exit.\n");

        alarm(0);

        exit(0);
}