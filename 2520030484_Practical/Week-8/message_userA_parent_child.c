#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define A2B "fifoA"
#define B2A "fifoB"

int main() {
    char user;
    char msg[100];

    printf("Enter user (A/B): ");
    scanf(" %c", &user);
    getchar();

    if (user != 'A' && user != 'B') {
        printf("Invalid user!\n");
        return 1;
    }

    mkfifo(A2B, 0666);
    mkfifo(B2A, 0666);

    int pid = fork();

    if (pid < 0) {
        perror("Failed to create child");
        return 1;
    }
    else if (pid == 0) {
        int fd;

        if (user == 'A')
            fd = open(B2A, O_RDONLY);
        else
            fd = open(A2B, O_RDONLY);

        while (1) {
            ssize_t bytes = read(fd, msg, sizeof(msg));

            if (bytes <= 0)
                break;

            printf("\nReceived: %s\n", msg);

            if (strcmp(msg, "bye") == 0)
                break;
        }

        close(fd);
        exit(0);
    }
    else {
        int fd;

        if (user == 'A')
            fd = open(A2B, O_WRONLY);
        else
            fd = open(B2A, O_WRONLY);

        while (1) {
            printf("\nSend: ");
            if (fgets(msg, sizeof(msg), stdin) == NULL)
                break;

            msg[strcspn(msg, "\n")] = '\0';

            write(fd, msg, strlen(msg) + 1);

            if (strcmp(msg, "bye") == 0)
                break;
        }

        close(fd);
        wait(NULL);
    }

    return 0;
}
