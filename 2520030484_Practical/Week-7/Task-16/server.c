#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

void handler(int sig) {
    printf("\nSignal received. Server shutting down.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handler);
    signal(SIGCHLD, SIG_IGN);

    char client_fifo[30], server_fifo[30];
    char msg[100];

    printf("Server started...\n");

    for (int i = 1; i <= 3; i++) {
        sprintf(client_fifo, "client%d_to_server", i);
        sprintf(server_fifo, "server_to_client%d", i);

        mkfifo(client_fifo, 0666);
        mkfifo(server_fifo, 0666);

        if (fork() == 0) {
            int rfd = open(client_fifo, O_RDONLY);
            int wfd = open(server_fifo, O_WRONLY);

            read(rfd, msg, sizeof(msg));
            printf("Client %d: %s\n", i, msg);

            sprintf(msg, "Hello Client %d, message received!", i);
            write(wfd, msg, strlen(msg) + 1);

            close(rfd);
            close(wfd);
            exit(0);
        }
    }

    while (1)
        pause();

    return 0;
}
