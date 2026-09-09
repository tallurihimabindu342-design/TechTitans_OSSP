#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("\nSIGINT received! Signal handled.\n");
}

int main() {
    signal(SIGINT, handler);

    printf("Press Ctrl+C to send SIGINT...\n");

    while (1)
        pause();

    return 0;
}
