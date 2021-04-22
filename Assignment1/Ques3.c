#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int signum)
{
    if (signum == SIGTERM)
    {
        fprintf(stderr, "I've been killed");
        exit(0);
    }


    if (signum == SIGPIPE)
    {
        fprintf(stderr, "I've been killed because my pipe reader died!");
        exit(2);
    }
}


int main(void)
{
    signal(SIGPIPE, sig_handler);
    signal(SIGTERM, sig_handler);

    char c[BUFSIZ] = { 0 };
    int n = read(STDIN_FILENO, c, BUFSIZ);

    int i = 0;

    for (i = 0; i < n; i++)
    {

        //printf("%c", c[i]);

        if (c[i] <= 'z' && c[i] >= 'a')
        {
            c[i] -= 'a' - 'A';
        }
        else if (c[i] <= 'Z' && c[i] >= 'A')
        {
            c[i] += 'a' - 'A';
        }

    }

    write(STDOUT_FILENO, c, n);

    exit(0);
}