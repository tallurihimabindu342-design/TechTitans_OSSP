#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int shmid = shmget(1234, 1024, 0666 | IPC_CREAT);

    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    printf("Shared memory created successfully.\n");
    printf("Shared Memory ID: %d\n", shmid);

    return 0;
}
