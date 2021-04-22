# include<stdio.h>
# include<stdlib.h>
# include<fcntl.h>
# include<unistd.h>
# include<wait.h>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Argument is missing !!\n");
        exit(1);
    }

    int i;
    pid_t pid[2];

    int fid[2];

    pipe(fid);


    int inRed = open(argv[1], O_RDONLY);
    int outRed = creat(argv[2], 0644);

    if (inRed == -1) {
        fprintf(stderr, "Input file does not exist !!");
        exit(-1);
    }

    dup2(inRed, STDIN_FILENO);
    dup2(outRed, STDOUT_FILENO);

    close(inRed);
    close(outRed);

    if (fork() == 0) {
        dup2(fid[1], STDOUT_FILENO);
        close(fid[1]);
        execl("q2", "q2", NULL);
        exit(0);
    }

    if (fork() == 0) {
        close(fid[1]);
        dup2(fid[0], STDIN_FILENO);
        close(fid[0]);
        execl("q3", "q3", NULL);
        exit(0);
    }

    close(fid[1]);
    close(fid[0]);

    int status;
    wait(&status);
    wait(&status);


    exit(0);
}