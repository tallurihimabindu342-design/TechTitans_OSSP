#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int marks1[5], marks2[5];

    printf("Enter marks for Student 1:\n");
    for (int i = 0; i < 5; i++)
        scanf("%d", &marks1[i]);

    printf("Enter marks for Student 2:\n");
    for (int i = 0; i < 5; i++)
        scanf("%d", &marks2[i]);

    int fd1 = shm_open("student1_marks", O_CREAT | O_RDWR, 0666);
    int fd2 = shm_open("student2_marks", O_CREAT | O_RDWR, 0666);

    ftruncate(fd1, sizeof(marks1));
    ftruncate(fd2, sizeof(marks2));

    int *shm1 = mmap(NULL, sizeof(marks1),
                     PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    int *shm2 = mmap(NULL, sizeof(marks2),
                     PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    memcpy(shm1, marks1, sizeof(marks1));
    memcpy(shm2, marks2, sizeof(marks2));

    printf("\nTeacher: Marks written successfully.\n");

    pid_t s1 = fork();

    if (s1 == 0) {
        execl("./consumer", "consumer", "1", NULL);
        return 1;
    }

    pid_t s2 = fork();

    if (s2 == 0) {
        execl("./consumer", "consumer", "2", NULL);
        return 1;
    }

    wait(NULL);
    wait(NULL);

    munmap(shm1, sizeof(marks1));
    munmap(shm2, sizeof(marks2));

    shm_unlink("student1_marks");
    shm_unlink("student2_marks");

    close(fd1);
    close(fd2);

    printf("\nTeacher: Both students completed. Shared memory cleaned up.\n");

    return 0;
}
