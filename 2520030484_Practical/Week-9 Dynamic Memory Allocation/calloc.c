// Dynamic memory allocation using calloc()

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, *p;

    printf("Enter the size of array:\n");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Invalid array size!\n");
        return 1;
    }

    p = (int *)calloc(n, sizeof(int));

    if (p == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Initial values using calloc:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", p[i]);
    }

    printf("\nEnter array elements:\n");
    for (int i = 0; i < n; i++) {
        printf("%d element:\n", i + 1);
        scanf("%d", &p[i]);
    }

    printf("Array elements are:\n");
    for (int i = 0; i < n; i++) {
        printf("%d element is: %d\n", i + 1, p[i]);
    }

    free(p);

    return 0;
}
