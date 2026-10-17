
#include <stdio.h>

int main() {
    int n, bt[10], p[10], wt[10], tat[10], rt[10], temp;
    float awt = 0, atat = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        printf("Enter burst time for P%d: ", i);
        scanf("%d", &bt[i]);
        p[i] = i;
    }

    // Sort by burst time
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (bt[i] > bt[j]) {
                temp = bt[i]; bt[i] = bt[j]; bt[j] = temp;
                temp = p[i]; p[i] = p[j]; p[j] = temp;
            }

    wt[0] = 0;
    rt[0] = 0;
    tat[0] = bt[0];

    for (int i = 1; i < n; i++) {
        wt[i] = wt[i-1] + bt[i-1];
        rt[i] = wt[i];
        tat[i] = wt[i] + bt[i];
    }

    printf("\nProcess\tBT\tWT\tTAT\tRT\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\n",
               p[i], bt[i], wt[i], tat[i], rt[i]);
        awt += wt[i];
        atat += tat[i];
    }

    printf("\nAverage WT = %.2f", awt/n);
    printf("\nAverage TAT = %.2f\n", atat/n);

    return 0;
}
