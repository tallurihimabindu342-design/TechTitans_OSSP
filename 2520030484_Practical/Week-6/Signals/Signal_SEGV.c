#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig) {
    printf("SIGSEGV caught successfully!\n");
    exit(0);
}

int main() {
    signal(SIGSEGV, handler);

    printf("Generating segmentation fault...\n");

    int *p = NULL;
    *p = 10;

    return 0;
}

