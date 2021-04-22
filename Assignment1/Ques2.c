#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sig_handler(int signum)
{
    fprintf(stderr, "I've been killed\n");
    exit(1);
}


int main(int argc, char* argv[])

{
    signal(SIGTERM, sig_handler);

    char c;
    int j = 0;
    int n = 0;

    if (argc == 2) {
        n = atoi(argv[1]);
        //n *= -1;

        if (n >= 0)
        {
            fprintf(stderr, "Invalid number in the argument !!\n");
            exit(2);
        }

        while ((n != 0 && (c = getchar()) != EOF))
        {
            putchar(c);
            if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'));
            else
            {
                j++;
            }
            n++;
        }
    }

    else
    {
        while ((c = getchar()) != EOF)
        {
            putchar(c);
            if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'));
            else
            {
                j++;
            }
        }
    }

    fprintf(stderr, "%d\n", j);

    exit(0);
}
