#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("Signal %d received.\n", sig);
}

int main() {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    printf("Waiting for signals...\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
