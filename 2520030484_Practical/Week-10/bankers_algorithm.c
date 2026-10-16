#include <stdio.h>

int n, m;

// Function to read Allocated or Maximum matrix
void read_allocated_or_max(int arr[10][10]) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            scanf("%d", &arr[i][j]);
        }
    }
}

// Function to display the entered values
void write(int available[10], int allocated[10][10],
           int maximum[10][10], int need[10][10]) {

    printf("\nDisplay the entered values as table\n");

    printf("\nAvailable:\t");

    for(int i = 0; i < m; i++) {
        printf("%d\t", available[i]);
    }

    printf("\n\nProcess\tAllocated\tMaximum\t\tNeed\n");

    for(int i = 0; i < n; i++) {

        printf("P%d\t", i);

        for(int j = 0; j < m; j++) {
            printf("%d ", allocated[i][j]);
        }

        printf("\t\t");

        for(int j = 0; j < m; j++) {
            printf("%d ", maximum[i][j]);
        }

        printf("\t\t");

        for(int j = 0; j < m; j++) {
            printf("%d ", need[i][j]);
        }

        printf("\n");
    }
}

int main() {

    int available[10];
    int allocated[10][10];
    int maximum[10][10];
    int need[10][10];

    int work[10];
    int finish[10] = {0};
    int safe_sequence[10];

    int count = 0;

    printf("enter the number of process\n");
    scanf("%d", &n);

    printf("enter the number of resources\n");
    scanf("%d", &m);

    printf("enter number of resources\n");

    for(int i = 0; i < m; i++) {
        scanf("%d", &available[i]);
    }

    printf("enter allocated resources for each process\n");
    read_allocated_or_max(allocated);

    printf("enter maximum no.of resources needed for each process\n");
    read_allocated_or_max(maximum);

    // Calculate Need = Maximum - Allocated
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            need[i][j] = maximum[i][j] - allocated[i][j];
        }
    }

    // Display the values
    write(available, allocated, maximum, need);

    // Work = Available
    for(int j = 0; j < m; j++) {
        work[j] = available[j];
    }

    // Banker's Algorithm
    while(count < n) {

        int found = 0;

        for(int i = 0; i < n; i++) {

            if(finish[i] == 0) {

                int possible = 1;

                // Check Need <= Work
                for(int j = 0; j < m; j++) {

                    if(need[i][j] > work[j]) {
                        possible = 0;
                        break;
                    }
                }

                if(possible) {

                    // Release allocated resources
                    for(int j = 0; j < m; j++) {
                        work[j] += allocated[i][j];
                    }

                    finish[i] = 1;

                    // Store process in safe sequence
                    safe_sequence[count] = i;

                    count++;

                    found = 1;
                }
            }
        }

        // No process can proceed
        if(found == 0) {
            break;
        }
    }

    // Display result
    if(count == n) {

        printf("\nSystem is in SAFE state\n");

        printf("Safe sequence: ");

        for(int i = 0; i < n; i++) {
            printf("P%d", safe_sequence[i]);

            if(i != n - 1)
                printf(" -> ");
        }

        printf("\n");
    }
    else {

        printf("\nSystem is NOT in SAFE state\n");
    }

    return 0;
}
