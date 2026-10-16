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
    char msg[100];

    mkfifo(A2B, 0666);
    mkfifo(B2A, 0666);

    int pid = fork();

    if (pid < 0) {
        perror("Failed to create child");
        return 1;
    }
    else if (pid == 0) {
        int fd = open(A2B, O_RDONLY);

        while (1) {
            ssize_t bytes = read(fd, msg, sizeof(msg));

            if (bytes <= 0)
                break;

            printf("\nUser A: %s\n", msg);

            if (strcmp(msg, "bye") == 0)
                break;
        }

        close(fd);
        exit(0);
    }
    else {
        int fd = open(B2A, O_WRONLY);

        while (1) {
            printf("\nUser B: ");

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
