#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void server(int fdr, int fdw)
{
    char filename[] = "fifo_input.txt";
    char buffer[256];
    int n;

    if (write(fdw, filename, strlen(filename) + 1) < 0) {
        printf("Server: can't write filename to chan1\n");
        return;
    }

    printf("Server: filename \"%s\" is sent to client\n", filename);
    printf("Server: waiting for data from client...\n");

    while ((n = read(fdr, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, n);
    }

    if (n < 0) {
        printf("Server: can't read data from chan2\n");
        return;
    }

    printf("\nServer: data transfer is complete\n");
}

int main(void)
{
    int fdw;
    int fdr;

    if (mknod("chan1", S_IFIFO | 0666, 0) < 0) {
        printf("Server: can't create chan1\n");
        return 1;
    }

    if (mknod("chan2", S_IFIFO | 0666, 0) < 0) {
        printf("Server: can't create chan2\n");
        unlink("chan1");
        return 1;
    }

    printf("Server: FIFO channels are created\n");

    fdw = open("chan1", O_WRONLY);
    if (fdw < 0) {
        printf("Server: can't open chan1 for writing\n");
        unlink("chan1");
        unlink("chan2");
        return 1;
    }

    fdr = open("chan2", O_RDONLY);
    if (fdr < 0) {
        printf("Server: can't open chan2 for reading\n");
        close(fdw);
        unlink("chan1");
        unlink("chan2");
        return 1;
    }

    server(fdr, fdw);

    close(fdw);
    close(fdr);

    return 0;
}
