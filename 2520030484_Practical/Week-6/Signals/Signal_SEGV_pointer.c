#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler(int sig) {
    printf("SIGSEGV caught: Invalid memory access!\n");
    exit(0);
}

int main() {
    signal(SIGSEGV, handler);

    int *ptr = NULL;

    printf("Accessing NULL pointer...\n");
    *ptr = 100;

    return 0;
}
