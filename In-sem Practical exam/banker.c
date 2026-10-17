
#include <stdio.h>

int n, m, alloc[10][10], need[10][10], max[10][10];
int avail[10], finish[10], seq[10], count = 0;

void safe(int work[], int depth)
{
    if (depth == n) {
        count++;
        printf("Safe Sequence %d: ", count);
        for (int i = 0; i < n; i++)
            printf("P%d ", seq[i]);
        printf("\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        if (!finish[i]) {
            int j, ok = 1;

            for (j = 0; j < m; j++)
                if (need[i][j] > work[j])
                    ok = 0;

            if (ok) {
                for (j = 0; j < m; j++)
                    work[j] += alloc[i][j];

                finish[i] = 1;
                seq[depth] = i;
                safe(work, depth + 1);
                finish[i] = 0;

                for (j = 0; j < m; j++)
                    work[j] -= alloc[i][j];
            }
        }
    }
}

int main()
{
    int work[10];

    printf("Enter processes and resources: ");
    scanf("%d%d", &n, &m);

    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &alloc[i][j]);

    printf("Enter Maximum Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            scanf("%d", &max[i][j]);
            need[i][j] = max[i][j] - alloc[i][j];
        }

    printf("Enter Available Resources:\n");
    for (int j = 0; j < m; j++) {
        scanf("%d", &avail[j]);
        work[j] = avail[j];
    }

    for (int i = 0; i < n; i++)
        finish[i] = 0;

    printf("\nNeed Matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            printf("%d ", need[i][j]);
        printf("\n");
    }

    safe(work, 0);

    if (count > 0)
        printf("\nSystem is in SAFE state.\n");
    else
        printf("\nSystem is in UNSAFE state; no safe sequence exists.\n");

    return 0;
}
