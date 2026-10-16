// Dynamic memory allocation using realloc()

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, *p;

    printf("Enter the size of array:\n");
    scanf("%d", &n);

    if (n < 4) {
        printf("Enter a size of at least 4.\n");
        return 1;
    }

    p = (int *)malloc(n * sizeof(int));

    if (p == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Enter array elements:\n");
    for (int i = 0; i < n; i++) {
        printf("%d element:\n", i + 1);
        scanf("%d", &p[i]);
    }

    int *temp = (int *)realloc(p, (n + 3) * sizeof(int));

    if (temp == NULL) {
        free(p);
        printf("Memory reallocation failed!\n");
        return 1;
    }

    p = temp;
    p[n] = 5;
    p[n + 1] = 10;
    p[n + 2] = 15;

    printf("\nAfter increasing memory:\n");
    for (int i = 0; i < n + 3; i++) {
        printf("%d element is: %d\n", i + 1, p[i]);
    }

    temp = (int *)realloc(p, (n - 3) * sizeof(int));

    if (temp == NULL) {
        free(p);
        printf("Memory reallocation failed!\n");
        return 1;
    }

    p = temp;

    printf("\nAfter decreasing memory:\n");
    for (int i = 0; i < n - 3; i++) {
        printf("%d element is: %d\n", i + 1, p[i]);
    }

    free(p);

    return 0;
}
