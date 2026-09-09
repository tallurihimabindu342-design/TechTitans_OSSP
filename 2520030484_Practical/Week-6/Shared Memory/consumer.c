#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int shmid = shmget(1234, 1024, 0666);
    char *str = (char *)shmat(shmid, NULL, 0);

    printf("Message from shared memory: %s", str);

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
