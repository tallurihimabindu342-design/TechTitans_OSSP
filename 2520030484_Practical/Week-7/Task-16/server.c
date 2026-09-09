#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

void handler(int sig) {
    printf("\nServer shutting down...\n");
    exit(0);
}

void client_handler(int n) {
    char in[30], out[30], msg[100];

    sprintf(in, "client%d_to_server", n);
    sprintf(out, "server_to_client%d", n);

    mkfifo(in, 0666);
    mkfifo(out, 0666);

    int rfd = open(in, O_RDONLY);
    int wfd = open(out, O_WRONLY);

    read(rfd, msg, sizeof(msg));

    printf("Client %d: %s\n", n, msg);

    sprintf(msg, "Message received from Server for Client %d", n);
    write(wfd, msg, strlen(msg) + 1);

    close(rfd);
    close(wfd);
    exit(0);
}

int main() {
    signal(SIGINT, handler);
    signal(SIGCHLD, SIG_IGN);

    printf("Server started...\n");

    if (fork() == 0)
        client_handler(1);

    if (fork() == 0)
        client_handler(2);

    while (1)
        sleep(1);

    return 0;
}
