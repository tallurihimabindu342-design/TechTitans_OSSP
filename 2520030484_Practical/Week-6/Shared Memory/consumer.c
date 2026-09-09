#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int student = atoi(argv[1]);
    char name[30];

    sprintf(name, "/student%d_marks", student);

    int fd = shm_open(name, O_RDONLY, 0666);

    int *marks = mmap(NULL, 5 * sizeof(int),
                      PROT_READ, MAP_SHARED, fd, 0);

    int total = 0;

    printf("\nStudent %d\n", student);
    printf("--------------------------------\n");

    for (int i = 0; i < 5; i++) {
        printf("Subject %d : %d\n", i + 1, marks[i]);
        total += marks[i];
    }

    printf("\nTotal   : %d\n", total);
    printf("Average : %.2f\n", total / 5.0);

    munmap(marks, 5 * sizeof(int));
    close(fd);

    return 0;
}
