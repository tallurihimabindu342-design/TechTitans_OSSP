#include <stdio.h>
#include <signal.h>

void handler(int sig) {
    printf("Signal received successfully!\n");
}

int main() {
    signal(SIGINT, handler);

    printf("Raising SIGINT...\n");
    raise(SIGINT);

    return 0;
}
