// Array memory is allocated at compile time

#include <stdio.h>

int main() {
    int n, ar[10];

    printf("Enter the size of array:\n");
    scanf("%d", &n);

    if (n < 1 || n > 10) {
        printf("Enter a size between 1 and 10.\n");
        return 1;
    }

    printf("Enter array elements:\n");

    for (int i = 0; i < n; i++) {
        printf("%d element:\n", i + 1);
        scanf("%d", &ar[i]);
    }

    for (int i = 0; i < n; i++) {
        printf("%d element is: %d\n", i + 1, ar[i]);
    }

    return 0;
}
