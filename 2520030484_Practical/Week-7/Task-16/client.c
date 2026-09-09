#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int n;
    char msg[100], reply[100];
    char client_fifo[30], server_fifo[30];

    printf("Enter client number (1-3): ");
    scanf("%d", &n);
    getchar();

    sprintf(client_fifo, "client%d_to_server", n);
    sprintf(server_fifo, "server_to_client%d", n);

    int wfd = open(client_fifo, O_WRONLY);
    int rfd = open(server_fifo, O_RDONLY);

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = '\0';

    write(wfd, msg, strlen(msg) + 1);
    read(rfd, reply, sizeof(reply));

    printf("Server: %s\n", reply);

    close(wfd);
    close(rfd);

    return 0;
}
