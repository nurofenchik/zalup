#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void client(int fdr, int fdw)
{
    char filename[256];
    char buffer[256];
    int file_fd;
    int n;

    if (read(fdr, filename, sizeof(filename)) <= 0) {
        printf("Client: can't read filename from chan1\n");
        return;
    }

    printf("Client: filename \"%s\" is received from server\n", filename);
    printf("Client: start writing file data to chan2\n");

    file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        printf("Client: can't open file %s\n", filename);
        return;
    }

    while ((n = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(fdw, buffer, n) != n) {
            printf("Client: can't write data to chan2\n");
            close(file_fd);
            return;
        }
    }

    if (n < 0) {
        printf("Client: can't read data from file\n");
        close(file_fd);
        return;
    }

    close(file_fd);
    printf("Client: data transfer is complete\n");
}

int main(void)
{
    int fdr;
    int fdw;

    fdr = open("chan1", O_RDONLY);
    if (fdr < 0) {
        printf("Client: can't open chan1 for reading\n");
        return 1;
    }

    fdw = open("chan2", O_WRONLY);
    if (fdw < 0) {
        printf("Client: can't open chan2 for writing\n");
        close(fdr);
        return 1;
    }

    client(fdr, fdw);

    close(fdr);
    close(fdw);

    unlink("chan1");
    unlink("chan2");

    return 0;
}
