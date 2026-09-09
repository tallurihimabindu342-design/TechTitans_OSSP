#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    char msg[100], reply[100];

    int wfd = open("client2_to_server", O_WRONLY);
    int rfd = open("server_to_client2", O_RDONLY);

    printf("Client 2\n");
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
