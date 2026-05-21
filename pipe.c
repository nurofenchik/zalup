#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void)
{
    int filedes[2];
    int pid;
    int input_fd;
    int output_fd;
    int n;
    char buffer[256];

    if (pipe(filedes) < 0) {
        printf("Father: can't create pipe\n");
        return 1;
    }

    printf("Father: pipe is created\n");

    pid = fork();

    if (pid < 0) {
        printf("Father: can't fork child\n");
        return 1;
    }

    if (pid == 0) {
        close(filedes[0]); //закрытие чтения

        input_fd = open("input.txt", O_RDONLY);
        if (input_fd < 0) {
            printf("Child: can't open input.txt\n");
            close(filedes[1]);
            return 1;
        }

        printf("Child: input file is opened\n");

        while ((n = read(input_fd, buffer, sizeof(buffer))) > 0) {
            if (write(filedes[1], buffer, n) != n) {
                printf("Child: can't write all data to pipe\n");
                close(input_fd);
                close(filedes[1]);
                return 1;
            }
        }

        if (n < 0) {
            printf("Child: can't read input file\n");
            close(input_fd);
            close(filedes[1]);
            return 1;
        }

        printf("Child: data is sent to pipe\n");

        close(input_fd);
        close(filedes[1]);
        return 0;
    } else {
        close(filedes[1]);//закрывает запись

        output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output_fd < 0) {
            printf("Father: can't open output.txt\n");
            close(filedes[0]);
            return 1;
        }

        printf("Father: output file is opened\n");
        printf("Father: data from pipe:\n");

        while ((n = read(filedes[0], buffer, sizeof(buffer))) > 0) {
            if (write(1, buffer, n) != n) {
                printf("Father: can't write data to stdout\n");
                close(output_fd);
                close(filedes[0]);
                return 1;
            }

            if (write(output_fd, buffer, n) != n) {
                printf("Father: can't write data to output.txt\n");
                close(output_fd);
                close(filedes[0]);
                return 1;
            }
        }

        if (n < 0) {
            printf("Father: can't read data from pipe\n");
            close(output_fd);
            close(filedes[0]);
            return 1;
        }

        printf("\nFather: data is written to output.txt\n");

        close(output_fd);
        close(filedes[0]);
        wait(0);

        return 0;
    }
}
