#include <stdio.h>

#include "allocator.h"


int main(void)
{
    int choice;

    size_t total_memory;

    size_t allocation_size;

    int strategy_choice;

    int mode_choice;

    printf("\n");
    printf(
        "================================================\n"
    );

    printf(
        "     LINUX DYNAMIC MEMORY ALLOCATION SYSTEM\n"
    );

    printf(
        "================================================\n"
    );


    /*
     * Ask user for total memory.
     */
    printf(
        "\nEnter total memory size (KB): "
    );

    scanf(
        "%zu",
        &total_memory
    );


    /*
     * Initialize memory.
     */
    if (!memory_init(total_memory))
    {
        printf(
            "\nFailed to create memory pool.\n"
        );

        return 1;
    }


    printf(
        "\nMemory pool created successfully!\n"
    );

    printf(
        "Total Memory: %zu KB\n",
        total_memory
    );


    /*
     * Select allocation strategy.
     */
    printf("\n");

    printf(
        "Select Allocation Strategy\n"
    );

    printf(
        "--------------------------\n"
    );

    printf(
        "1. First Fit\n"
    );

    printf(
        "2. Best Fit\n"
    );

    printf(
        "3. Worst Fit\n"
    );


    printf(
        "\nEnter choice: "
    );

    scanf(
        "%d",
        &strategy_choice
    );


    switch (strategy_choice)
    {
        case 1:

            set_allocation_strategy(
                FIRST_FIT
            );

            break;


        case 2:

            set_allocation_strategy(
                BEST_FIT
            );

            break;


        case 3:

            set_allocation_strategy(
                WORST_FIT
            );

            break;


        default:

            printf(
                "\nInvalid strategy.\n"
            );

            printf(
                "Using First Fit by default.\n"
            );


            set_allocation_strategy(
                FIRST_FIT
            );

            break;
    }


    printf(
        "\nSelected Strategy: %s\n",
        get_strategy_name()
    );

    /*
 * Select allocation mode.
 */
printf("\n");

printf(
    "Select Allocation Mode\n"
);

printf(
    "----------------------\n"
);

printf(
    "1. Contiguous\n"
);

printf(
    "2. Non-Contiguous\n"
);

printf(
    "\nEnter choice: "
);

scanf(
    "%d",
    &mode_choice
);

switch (mode_choice)
{
    case 1:

        set_allocation_mode(
            CONTIGUOUS_MODE
        );

        break;


    case 2:

        set_allocation_mode(
            NON_CONTIGUOUS_MODE
        );

        break;


    default:

        printf(
            "\nInvalid allocation mode.\n"
        );

        printf(
            "Using Contiguous mode by default.\n"
        );

        set_allocation_mode(
            CONTIGUOUS_MODE
        );

        break;
}

printf(
    "\nSelected Mode: %s\n",
    get_allocation_mode_name()
);

/*
 * Publish the empty pool so the dashboard goes LIVE
 * immediately, before the first allocation.
 */
export_memory_status();


    /*
     * Main menu.
     */
    while (1)
    {
        printf("\n");

        printf(
            "================ MENU ================\n"
        );

        printf(
    "1. Allocate Memory\n"
);

printf(
    "2. Free Allocation\n"
);

printf(
    "3. Display Memory Status\n"
);

printf(
    "4. Fragmentation Analysis\n"
);

printf(
    "5. Reallocate Memory\n"
);

printf(
    "6. Visualize Memory\n"
);

printf(
    "7. Memory Monitoring\n"
);

printf(
    "8. Exit\n"
);

printf(
    "9. Run Allocation Benchmark\n"
);

        printf(
            "======================================\n"
        );


        printf(
            "Enter your choice: "
        );

        scanf(
            "%d",
            &choice
        );


        switch (choice)
        {
            /*
             * -----------------------------------------
             * ALLOCATE
             * -----------------------------------------
             */
            case 1:
            {
                printf(
                    "\nEnter memory size (KB): "
                );


                scanf(
                    "%zu",
                    &allocation_size
                );


                /*
                 * my_malloc() automatically decides
                 * whether contiguous or non-contiguous
                 * allocation is required.
                 */
                my_malloc(
    		  allocation_size
		);

		export_memory_status();

		break;
            }


            /*
             * -----------------------------------------
             * FREE
             * -----------------------------------------
             */
            case 2:
            {
                int allocation_id;


                printf(
                    "\nEnter Allocation ID number:\n"
                );


                printf(
                    "Example: A001 -> enter 1\n"
                );


                printf(
                    "ID: "
                );


                scanf(
                    "%d",
                    &allocation_id
                );


                my_free_block(
    		   allocation_id
                 );

		   export_memory_status();

		break;
            }


            /*
             * -----------------------------------------
             * DISPLAY MEMORY
             * -----------------------------------------
             */
            case 3:
            {
                display_memory();

                break;
            }


            /*
             * -----------------------------------------
             * FRAGMENTATION
             * -----------------------------------------
             */
            case 4:
            {
                display_fragmentation();

                break;
            }


            /*
             * -----------------------------------------
             * EXIT
             * -----------------------------------------
             */
            /*
 * -----------------------------------------
 * REALLOCATE
 * -----------------------------------------
 */
case 5:
{
    int allocation_id;
    size_t new_size;

    printf(
        "\nEnter Allocation ID number:\n"
    );

    printf(
        "Example: A001 -> enter 1\n"
    );

    printf(
        "ID: "
    );

    scanf(
        "%d",
        &allocation_id
    );

    printf(
        "Enter new memory size (KB): "
    );

    scanf(
        "%zu",
        &new_size
    );

    my_realloc(
    allocation_id,
    new_size
   );

    export_memory_status();

     break;
   }


/*
 * -----------------------------------------
 * VISUALIZE MEMORY
 * -----------------------------------------
 */
case 6:
{
    display_memory_visual();
    break;
}


case 7:
{
    display_memory_monitor();
    break;
}

case 8:
{
    memory_cleanup();
    return 0;
}

/*
 * -----------------------------------------
 * ALLOCATION BENCHMARK
 * (runs on a scratch pool; real memory
 * state is not modified)
 * -----------------------------------------
 */
case 9:
{
    run_allocation_benchmark();
    break;
}

            /*
             * -----------------------------------------
             * INVALID OPTION
             * -----------------------------------------
             */
            default:
            {
                printf(
                    "\nInvalid choice. Please try again.\n"
                );

                break;
            }
        }
    }


    return 0;
}
