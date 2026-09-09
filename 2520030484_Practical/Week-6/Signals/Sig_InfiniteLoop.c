#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("\nSIGINT received. Stopping loop.\n");
    _exit(0);
}

int main() {
    signal(SIGINT, handler);

    printf("Infinite loop running. Press Ctrl+C to stop.\n");

    while (1) {
        printf("Running...\n");
        sleep(1);
    }

    return 0;
}
