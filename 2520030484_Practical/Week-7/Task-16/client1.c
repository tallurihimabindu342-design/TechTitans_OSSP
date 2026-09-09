#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    char msg[100], reply[100];

    int wfd = open("client1_to_server", O_WRONLY);
    int rfd = open("server_to_client1", O_RDONLY);

    printf("Client 1\n");
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
