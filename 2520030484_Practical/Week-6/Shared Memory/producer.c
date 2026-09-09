#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int shmid = shmget(1234, 1024, 0666 | IPC_CREAT);
    char *str = (char *)shmat(shmid, NULL, 0);

    printf("Enter message: ");
    fgets(str, 1024, stdin);

    printf("Message written to shared memory.\n");

    shmdt(str);
    return 0;
}
