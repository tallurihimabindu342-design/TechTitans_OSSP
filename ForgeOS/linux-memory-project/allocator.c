#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* getpid() for dashboard liveness check */
#include <fcntl.h>    /* open() for silencing benchmark output */

#include "allocator.h"


/* =========================================================
   GLOBAL VARIABLES
   ========================================================= */

static unsigned char *memory_pool = NULL;

static size_t memory_pool_size = 0;

static MemoryBlock *block_list = NULL;

static AllocationStrategy current_strategy = FIRST_FIT;

static AllocationMode current_mode = CONTIGUOUS_MODE;

/*
 * Unique logical allocation ID.
 *
 * IDs are never reused during one program execution.
 */
static int next_allocation_id = 1;


/* =========================================================
   INITIALIZE MEMORY
   ========================================================= */

int memory_init(size_t total_size)
{
    if (total_size == 0)
    {
        return 0;
    }

    /*
     * User enters memory in KB.
     * Convert KB to bytes.
     */
    memory_pool = malloc(total_size * 1024);

    if (memory_pool == NULL)
    {
        return 0;
    }

    memory_pool_size = total_size;

    /*
     * Clear memory pool.
     */
    memset(
        memory_pool,
        0,
        total_size * 1024
    );

    /*
     * Initially the entire memory pool
     * is one FREE block.
     */
    block_list = malloc(sizeof(MemoryBlock));

    if (block_list == NULL)
    {
        free(memory_pool);

        memory_pool = NULL;

        return 0;
    }

    block_list->offset = 0;
    block_list->size = total_size;
    block_list->requested_size = 0;
    block_list->free = 1;
    block_list->allocation_id = 0;
    block_list->next = NULL;

    next_allocation_id = 1;

    return 1;
}


/* =========================================================
   ROUND TO ALLOCATION UNIT
   ========================================================= */

/*
 * Round requested memory to the next 10 KB allocation unit.
 */
static size_t round_to_allocation_unit(size_t size)
{
    const size_t ALLOCATION_UNIT = 10;

    if (size % ALLOCATION_UNIT == 0)
    {
        return size;
    }

    return ((size / ALLOCATION_UNIT) + 1) * ALLOCATION_UNIT;
}


/* =========================================================
   SET ALLOCATION STRATEGY / MODE
   ========================================================= */

void set_allocation_strategy(
    AllocationStrategy strategy)
{
    current_strategy = strategy;
}

void set_allocation_mode(AllocationMode mode)
{
    current_mode = mode;
}

const char *get_allocation_mode_name(void)
{
    switch (current_mode)
    {
        case CONTIGUOUS_MODE:
            return "Contiguous";

        case NON_CONTIGUOUS_MODE:
            return "Non-Contiguous";

        default:
            return "Unknown";
    }
}


/* =========================================================
   GET STRATEGY NAME
   ========================================================= */

const char *get_strategy_name(void)
{
    switch (current_strategy)
    {
        case FIRST_FIT:
            return "First Fit";

        case BEST_FIT:
            return "Best Fit";

        case WORST_FIT:
            return "Worst Fit";

        default:
            return "Unknown";
    }
}


/* =========================================================
   FIND CONTIGUOUS BLOCK
   ========================================================= */

static MemoryBlock *find_contiguous_block(
    size_t size)
{
    MemoryBlock *current = block_list;

    MemoryBlock *selected = NULL;


    /*
     * FIRST FIT
     *
     * Select the first free block
     * large enough for the request.
     */
    if (current_strategy == FIRST_FIT)
    {
        while (current != NULL)
        {
            if (current->free &&
                current->size >= size)
            {
                return current;
            }

            current = current->next;
        }

        return NULL;
    }


    /*
     * BEST FIT
     *
     * Select the smallest free block
     * that can satisfy the request.
     */
    if (current_strategy == BEST_FIT)
    {
        while (current != NULL)
        {
            if (current->free &&
                current->size >= size)
            {
                if (selected == NULL ||
                    current->size < selected->size)
                {
                    selected = current;
                }
            }

            current = current->next;
        }

        return selected;
    }


    /*
     * WORST FIT
     *
     * Select the largest free block
     * that can satisfy the request.
     */
    if (current_strategy == WORST_FIT)
    {
        while (current != NULL)
        {
            if (current->free &&
                current->size >= size)
            {
                if (selected == NULL ||
                    current->size > selected->size)
                {
                    selected = current;
                }
            }

            current = current->next;
        }

        return selected;
    }

    return NULL;
}


/* =========================================================
   SPLIT A MEMORY BLOCK
   ========================================================= */

static MemoryBlock *split_block(
    MemoryBlock *block,
    size_t requested_size)
{
    /*
     * Exact-size block does not need splitting.
     */
    if (block->size == requested_size)
    {
        return block;
    }


    /*
     * Create a new block for remaining free memory.
     */
    MemoryBlock *new_block =
        malloc(sizeof(MemoryBlock));

    if (new_block == NULL)
    {
        return NULL;
    }


    new_block->offset =
        block->offset + requested_size;

    new_block->size =
        block->size - requested_size;

    new_block->requested_size = 0;

    new_block->free = 1;

    new_block->allocation_id = 0;

    new_block->next =
        block->next;


    /*
     * Resize current block.
     */
    block->size =
        requested_size;

    block->next =
        new_block;


    return block;
}


/* =========================================================
   TOTAL FREE MEMORY
   ========================================================= */

static size_t get_total_free_memory(void)
{
    MemoryBlock *current =
        block_list;

    size_t total_free = 0;


    while (current != NULL)
    {
        if (current->free)
        {
            total_free += current->size;
        }

        current = current->next;
    }


    return total_free;
}


/* =========================================================
   COUNT SEGMENTS OF AN ALLOCATION
   ========================================================= */

static int count_allocation_segments(
    int allocation_id)
{
    MemoryBlock *current =
        block_list;

    int count = 0;


    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                allocation_id)
        {
            count++;
        }

        current = current->next;
    }


    return count;
}


/* =========================================================
   FIND FREE BLOCK FOR NON-CONTIGUOUS ALLOCATION
   ========================================================= */

static MemoryBlock *find_best_candidate(void)
{
    MemoryBlock *current =
        block_list;

    MemoryBlock *selected =
        NULL;


    /*
     * FIRST FIT
     */
    if (current_strategy == FIRST_FIT)
    {
        while (current != NULL)
        {
            if (current->free)
            {
                return current;
            }

            current = current->next;
        }

        return NULL;
    }


    /*
     * BEST FIT
     */
    if (current_strategy == BEST_FIT)
    {
        while (current != NULL)
        {
            if (current->free)
            {
                if (selected == NULL ||
                    current->size < selected->size)
                {
                    selected = current;
                }
            }

            current = current->next;
        }

        return selected;
    }


    /*
     * WORST FIT
     */
    if (current_strategy == WORST_FIT)
    {
        while (current != NULL)
        {
            if (current->free)
            {
                if (selected == NULL ||
                    current->size > selected->size)
                {
                    selected = current;
                }
            }

            current = current->next;
        }

        return selected;
    }


    return NULL;
}


/* =========================================================
   COALESCE ADJACENT FREE BLOCKS
   ========================================================= */

static void coalesce_free_blocks(void)
{
    MemoryBlock *current =
        block_list;


    while (current != NULL &&
           current->next != NULL)
    {
        if (current->free &&
            current->next->free)
        {
            MemoryBlock *next_block =
                current->next;


            /*
             * Merge adjacent free blocks.
             */
            current->size +=
                next_block->size;

            current->next =
                next_block->next;


            free(next_block);
        }
        else
        {
            current =
                current->next;
        }
    }
}


/* =========================================================
   CONTIGUOUS ALLOCATION
   ========================================================= */

static void *allocate_contiguous(
    size_t size,
    size_t requested_size,
    int allocation_id)
{
    MemoryBlock *block =
        find_contiguous_block(size);


    /*
     * No suitable contiguous block.
     */
    if (block == NULL)
    {
        return NULL;
    }


    /*
     * Split if block is larger than request.
     */
    if (block->size > size)
    {
        if (split_block(
                block,
                size) == NULL)
        {
            return NULL;
        }
    }


    /*
     * Mark block as allocated.
     */
    block->free = 0;

    block->requested_size =
        requested_size;

    block->allocation_id =
        allocation_id;


    /*
     * Return address of allocated region.
     */
    return (void *)(
        memory_pool +
        block->offset * 1024
    );
}


/* =========================================================
   ROLLBACK FAILED ALLOCATION
   ========================================================= */

static void rollback_allocation(
    int allocation_id)
{
    MemoryBlock *current =
        block_list;


    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                allocation_id)
        {
            current->free = 1;

            current->requested_size = 0;

            current->allocation_id = 0;
        }

        current = current->next;
    }


    coalesce_free_blocks();
}


/* =========================================================
   GET PHYSICAL BLOCK NUMBER
   ========================================================= */

static int get_block_number(MemoryBlock *target)
{
    MemoryBlock *current = block_list;
    int block_number = 1;

    while (current != NULL)
    {
        if (current == target)
        {
            return block_number;
        }

        current = current->next;
        block_number++;
    }

    return -1;
}


/* =========================================================
   NON-CONTIGUOUS ALLOCATION
   ========================================================= */

static void *allocate_non_contiguous(
    size_t size,
    size_t requested_size,
    int allocation_id)
{
    size_t remaining =
        size;

    MemoryBlock *first_segment =
        NULL;


    while (remaining > 0)
    {
        /*
         * Find a free physical block.
         */
        MemoryBlock *block =
            find_best_candidate();


        if (block == NULL)
        {
            /*
             * Roll back if allocation
             * cannot be completed.
             */
            rollback_allocation(
                allocation_id
            );

            return NULL;
        }


        size_t segment_size;


        /*
         * Entire free block can be used.
         */
        if (block->size <= remaining)
        {
            segment_size =
                block->size;

            block->free = 0;

            block->allocation_id =
                allocation_id;
        }
        else
        {
            /*
             * Only part of the free block
             * is required.
             */
            segment_size =
                remaining;


            if (split_block(
                    block,
                    segment_size) == NULL)
            {
                rollback_allocation(
                    allocation_id
                );

                return NULL;
            }


            block->free = 0;

            block->allocation_id =
                allocation_id;
        }


        /*
         * Store the first physical segment.
         */
        if (first_segment == NULL)
        {
            first_segment = block;

            /*
             * Store the logical requested size
             * only on the first segment.
             */
            block->requested_size = requested_size;
        }

        /*
         * Display the physical block selected
         * for this non-contiguous allocation.
         */
        printf(
            "  -> Using Physical Block %d | Size: %zu KB\n",
            get_block_number(block),
            segment_size
        );


        remaining -=
            segment_size;
    }


    /*
     * Return address of first segment.
     */
    if (first_segment != NULL)
    {
        return (void *)(
            memory_pool +
            first_segment->offset * 1024
        );
    }


    return NULL;
}


/* =========================================================
   MAIN ALLOCATION FUNCTION
   ========================================================= */

void *my_malloc(size_t size)
{
    if (size == 0 ||
        memory_pool == NULL)
    {
        return NULL;
    }

    /*
     * Generate a new unique logical ID.
     */
    int allocation_id =
        next_allocation_id++;

    /*
     * Keep the original requested size.
     */
    size_t requested_size = size;

    /*
     * Round allocation to the 10 KB allocation unit.
     */
    size_t allocation_size =
        round_to_allocation_unit(size);


    /*
     * =====================================================
     * CONTIGUOUS ALLOCATION MODE
     * =====================================================
     *
     * The complete request must fit inside ONE
     * physical free block.
     *
     * No automatic fallback to non-contiguous allocation.
     */
    if (current_mode == CONTIGUOUS_MODE)
    {
        void *ptr =
            allocate_contiguous(
                allocation_size,
                requested_size,
                allocation_id
            );

        if (ptr == NULL)
        {
            size_t total_free =
                get_total_free_memory();

            printf("\n");
            printf(
                "Memory allocation failed!\n"
            );

            printf(
                "Allocation Mode : CONTIGUOUS\n"
            );

            printf(
                "Requested Memory : %zu KB\n",
                requested_size
            );

            printf(
                "Total Free Memory: %zu KB\n",
                total_free
            );

            printf(
                "Reason: No single contiguous block is large enough.\n"
            );

            return NULL;
        }

        printf("\n");
        printf(
            "Memory allocated successfully!\n"
        );

        printf(
            "Allocation ID : A%03d\n",
            allocation_id
        );

        printf(
            "Requested Size : %zu KB\n",
            requested_size
        );

        printf(
            "Allocated Size : %zu KB\n",
            allocation_size
        );

        printf(
            "Internal Waste : %zu KB\n",
            allocation_size - requested_size
        );

        printf(
            "Allocation Type: CONTIGUOUS\n"
        );

        printf(
            "Strategy      : %s\n",
            get_strategy_name()
        );

        return ptr;
    }


    /*
     * =====================================================
     * NON-CONTIGUOUS ALLOCATION MODE
     * =====================================================
     *
     * The request may use multiple separate physical
     * free blocks.
     *
     * The blocks remain physically separate.
     * They are NOT merged and NOT compacted.
     */
    if (current_mode == NON_CONTIGUOUS_MODE)
    {
        size_t total_free =
            get_total_free_memory();

        /*
         * Even the combined free memory is insufficient.
         */
        if (total_free < allocation_size)
        {
            printf("\n");
            printf(
                "Memory allocation failed!\n"
            );

            printf(
                "Allocation Mode : NON-CONTIGUOUS\n"
            );

            printf(
                "Requested Memory : %zu KB\n",
                requested_size
            );

            printf(
                "Total Free Memory: %zu KB\n",
                total_free
            );

            printf(
                "Reason: Insufficient total free memory.\n"
            );

            return NULL;
        }

        /*
         * Allocate the request using multiple
         * physical free blocks if necessary.
         */
        void *ptr =
            allocate_non_contiguous(
                allocation_size,
                requested_size,
                allocation_id
            );

        if (ptr == NULL)
        {
            printf("\n");
            printf(
                "Memory allocation failed!\n"
            );

            printf(
                "Allocation Mode : NON-CONTIGUOUS\n"
            );

            printf(
                "Reason: Non-contiguous allocation could not be completed.\n"
            );

            return NULL;
        }

        int segment_count =
            count_allocation_segments(
                allocation_id
            );

        printf("\n");
        printf(
            "Memory allocated successfully!\n"
        );

        printf(
            "Allocation ID : A%03d\n",
            allocation_id
        );

        printf(
            "Requested Size : %zu KB\n",
            requested_size
        );

        printf(
            "Allocated Size : %zu KB\n",
            allocation_size
        );

        printf(
            "Internal Waste : %zu KB\n",
            allocation_size - requested_size
        );

        printf(
            "Allocation Type: NON-CONTIGUOUS\n"
        );

        printf(
            "Segments Used : %d\n",
            segment_count
        );

        printf(
            "Strategy      : %s\n",
            get_strategy_name()
        );

        return ptr;
    }


    return NULL;
}


/* =========================================================
   REALLOCATE ALLOCATION
   ========================================================= */

void *my_realloc(
    int allocation_id,
    size_t new_size)
{
    /*
     * Invalid request.
     */
    if (memory_pool == NULL ||
        new_size == 0)
    {
        if (new_size == 0)
        {
            my_free_block(allocation_id);
        }

        return NULL;
    }


    /*
     * Find all physical segments
     * belonging to this allocation.
     */
    MemoryBlock *current =
        block_list;

    MemoryBlock *first_segment =
        NULL;

    size_t current_size = 0;

    int segment_count = 0;


    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                allocation_id)
        {
            if (first_segment == NULL)
            {
                first_segment = current;
            }

            current_size +=
                current->size;

            segment_count++;
        }

        current =
            current->next;
    }


    /*
     * Allocation ID was not found.
     */
    if (first_segment == NULL)
    {
        printf("\n");

        printf(
            "Allocation A%03d not found.\n",
            allocation_id
        );

        return NULL;
    }


    /*
     * Round the new requested size
     * using the same 10 KB simulation
     * unit used by my_malloc().
     */
    size_t new_allocation_size =
        round_to_allocation_unit(
            new_size
        );


    /*
     * -----------------------------------------------------
     * CASE 1: Same allocated size
     * -----------------------------------------------------
     */
    if (new_allocation_size ==
        current_size)
    {
        first_segment->requested_size =
            new_size;

        printf("\n");

        printf(
            "Allocation A%03d resized successfully.\n",
            allocation_id
        );

        printf(
            "Old Allocated Size : %zu KB\n",
            current_size
        );

        printf(
            "New Requested Size : %zu KB\n",
            new_size
        );

        printf(
            "New Allocated Size : %zu KB\n",
            new_allocation_size
        );

        printf(
            "Internal Waste     : %zu KB\n",
            new_allocation_size -
                new_size
        );

        return (void *)(
            memory_pool +
            first_segment->offset * 1024
        );
    }


    /*
     * -----------------------------------------------------
     * CASE 2: Single contiguous segment
     * -----------------------------------------------------
     */
    if (segment_count == 1)
    {
        /*
         * SHRINK
         */
        if (new_allocation_size <
            first_segment->size)
        {
            if (split_block(
                    first_segment,
                    new_allocation_size) == NULL)
            {
                printf("\n");

                printf(
                    "Reallocation failed.\n"
                );

                return NULL;
            }

            first_segment->free = 0;

            first_segment->requested_size =
                new_size;

            first_segment->allocation_id =
                allocation_id;

            /*
             * Merge the newly created
             * free space with adjacent
             * free blocks if possible.
             */
            coalesce_free_blocks();

            printf("\n");

            printf(
                "Allocation A%03d resized successfully.\n",
                allocation_id
            );

            printf(
                "Operation         : SHRINK\n"
            );

            printf(
                "Old Allocated Size: %zu KB\n",
                current_size
            );

            printf(
                "New Requested Size: %zu KB\n",
                new_size
            );

            printf(
                "New Allocated Size: %zu KB\n",
                new_allocation_size
            );

            printf(
                "Internal Waste    : %zu KB\n",
                new_allocation_size -
                    new_size
            );

            return (void *)(
                memory_pool +
                first_segment->offset * 1024
            );
        }


        /*
         * EXPAND IN PLACE
         *
         * Check whether the block immediately
         * after the allocation is free.
         */
        MemoryBlock *next_block =
            first_segment->next;

        if (next_block != NULL &&
            next_block->free &&
            first_segment->size +
                next_block->size >=
                new_allocation_size)
        {
            /*
             * Absorb the adjacent free block.
             */
            first_segment->size +=
                next_block->size;

            first_segment->next =
                next_block->next;

            free(next_block);


            /*
             * Split off any unused remainder.
             */
            if (first_segment->size >
                new_allocation_size)
            {
                if (split_block(
                        first_segment,
                        new_allocation_size) == NULL)
                {
                    return NULL;
                }
            }


            first_segment->free = 0;

            first_segment->requested_size =
                new_size;

            first_segment->allocation_id =
                allocation_id;


            printf("\n");

            printf(
                "Allocation A%03d resized successfully.\n",
                allocation_id
            );

            printf(
                "Operation         : EXPAND IN PLACE\n"
            );

            printf(
                "Old Allocated Size: %zu KB\n",
                current_size
            );

            printf(
                "New Requested Size: %zu KB\n",
                new_size
            );

            printf(
                "New Allocated Size: %zu KB\n",
                new_allocation_size
            );

            printf(
                "Internal Waste    : %zu KB\n",
                new_allocation_size -
                    new_size
            );

            return (void *)(
                memory_pool +
                first_segment->offset * 1024
            );
        }
    }


    /*
     * -----------------------------------------------------
     * CASE 3:
     * Relocation required.
     *
     * A temporary negative ID is used so that
     * the original allocation ID can be preserved.
     * -----------------------------------------------------
     */

    int temporary_id =
        -allocation_id;


    /*
     * First try contiguous allocation.
     */
    void *new_ptr =
        allocate_contiguous(
            new_allocation_size,
            new_size,
            temporary_id
        );


    /*
     * If contiguous allocation is impossible,
     * try non-contiguous allocation.
     */
    if (new_ptr == NULL)
    {
        new_ptr =
            allocate_non_contiguous(
                new_allocation_size,
                new_size,
                temporary_id
            );
    }


    /*
     * New allocation could not be created.
     *
     * Original allocation is left untouched.
     */
    if (new_ptr == NULL)
    {
        printf("\n");

        printf(
            "Reallocation failed.\n"
        );

        printf(
            "Original allocation A%03d "
            "was preserved.\n",
            allocation_id
        );

        return NULL;
    }


    /*
     * Release the old physical segments.
     */
    current =
        block_list;

    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                allocation_id)
        {
            current->free = 1;

            current->requested_size = 0;

            current->allocation_id = 0;
        }

        current =
            current->next;
    }


    /*
     * Change the temporary allocation
     * back to the original logical ID.
     */
    current =
        block_list;

    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                temporary_id)
        {
            current->allocation_id =
                allocation_id;
        }

        current =
            current->next;
    }


    /*
     * Merge adjacent free blocks.
     */
    coalesce_free_blocks();


    /*
     * Count the new number of segments.
     */
    int new_segment_count =
        count_allocation_segments(
            allocation_id
        );


    printf("\n");

    printf(
        "Allocation A%03d resized successfully.\n",
        allocation_id
    );

    printf(
        "Operation         : RELOCATED\n"
    );

    printf(
        "Old Allocated Size: %zu KB\n",
        current_size
    );

    printf(
        "New Requested Size: %zu KB\n",
        new_size
    );

    printf(
        "New Allocated Size: %zu KB\n",
        new_allocation_size
    );

    printf(
        "Internal Waste    : %zu KB\n",
        new_allocation_size -
            new_size
    );

    if (new_segment_count > 1)
    {
        printf(
            "Allocation Type   : NON-CONTIGUOUS\n"
        );

        printf(
            "Segments Used     : %d\n",
            new_segment_count
        );
    }
    else
    {
        printf(
            "Allocation Type   : CONTIGUOUS\n"
        );
    }

    return new_ptr;
}


/* =========================================================
   FREE ALLOCATION
   ========================================================= */

void my_free_block(int allocation_id)
{
    MemoryBlock *current =
        block_list;

    int found = 0;

    int segments_freed = 0;


    /*
     * Find every physical segment
     * belonging to this allocation.
     */
    while (current != NULL)
    {
        if (!current->free &&
            current->allocation_id ==
                allocation_id)
        {
            current->free = 1;

            current->requested_size = 0;

            current->allocation_id = 0;

            found = 1;

            segments_freed++;
        }

        current =
            current->next;
    }


    if (!found)
    {
        printf("\n");

        printf(
            "Allocation A%03d not found.\n",
            allocation_id
        );

        return;
    }


    /*
     * Merge adjacent free blocks.
     */
    coalesce_free_blocks();


    printf("\n");

    printf(
        "Allocation A%03d freed successfully.\n",
        allocation_id
    );

    printf(
        "Segments released: %d\n",
        segments_freed
    );
}


/* =========================================================
   DISPLAY MEMORY STATUS
   ========================================================= */

void display_memory(void)
{
    MemoryBlock *current =
        block_list;

    int block_number = 1;

    size_t used_memory = 0;

    size_t free_memory = 0;


    printf("\n");

    printf(
        "========================================================\n"
    );

    printf(
        "                 MEMORY STATUS\n"
    );

    printf(
        "========================================================\n"
    );


    printf(
        "Current Strategy: %s\n",
        get_strategy_name()
    );


    printf(
        "--------------------------------------------------------\n"
    );


    while (current != NULL)
    {
        /*
         * FREE BLOCK
         */
        if (current->free)
        {
            printf(
                "Block %d | Offset: %4zu KB | Size: %4zu KB | FREE\n",
                block_number,
                current->offset,
                current->size
            );


            free_memory +=
                current->size;
        }


        /*
         * USED BLOCK
         */
        else
        {
            int segment_count =
                count_allocation_segments(
                    current->allocation_id
                );


            /*
             * One segment = contiguous.
             * Multiple segments = non-contiguous.
             */
            const char *type =
                segment_count > 1
                ? "NON-CONTIGUOUS"
                : "CONTIGUOUS";


            /*
             * Determine segment number.
             */
            int segment_number = 0;

            MemoryBlock *scan =
                block_list;


            while (scan != current)
            {
                if (!scan->free &&
                    scan->allocation_id ==
                        current->allocation_id)
                {
                    segment_number++;
                }

                scan =
                    scan->next;
            }


            segment_number++;


            /*
             * Display non-contiguous allocation.
             */
            if (segment_count > 1)
            {
                printf(
                    "Block %d | Offset: %4zu KB | Size: %4zu KB | USED | ID: A%03d | %s | Segment %d/%d\n",
                    block_number,
                    current->offset,
                    current->size,
                    current->allocation_id,
                    type,
                    segment_number,
                    segment_count
                );
            }


            /*
             * Display contiguous allocation.
             */
            else
            {
                printf(
                    "Block %d | Offset: %4zu KB | Size: %4zu KB | USED | ID: A%03d | %s\n",
                    block_number,
                    current->offset,
                    current->size,
                    current->allocation_id,
                    type
                );
            }


            used_memory +=
                current->size;
        }


        current =
            current->next;

        block_number++;
    }


    printf(
        "--------------------------------------------------------\n"
    );


    printf(
        "Used Memory : %zu KB\n",
        used_memory
    );


    printf(
        "Free Memory : %zu KB\n",
        free_memory
    );


    printf(
        "Total Memory: %zu KB\n",
        memory_pool_size
    );


    /*
     * Memory utilization.
     */
    if (memory_pool_size > 0)
    {
        double utilization =
            ((double)used_memory /
             memory_pool_size) * 100.0;


        printf(
            "Utilization : %.2f%%\n",
            utilization
        );
    }


    printf(
        "========================================================\n"
    );
}

/* =========================================================
   MEMORY MONITORING
   ========================================================= */

void display_memory_monitor(void)
{
    MemoryBlock *current =
        block_list;

    size_t used_memory = 0;
    size_t free_memory = 0;

    size_t largest_free_block = 0;

    int active_allocations = 0;
    int free_blocks = 0;

    int seen_ids[1000];
    int seen_count = 0;


    /*
     * Scan all physical memory blocks.
     */
    while (current != NULL)
    {
        /*
         * FREE BLOCK
         */
        if (current->free)
        {
            free_memory +=
                current->size;

            free_blocks++;

            if (current->size >
                largest_free_block)
            {
                largest_free_block =
                    current->size;
            }
        }

        /*
         * USED BLOCK
         */
        else
        {
            used_memory +=
                current->size;

            /*
             * Count each logical allocation
             * only once.
             */
            int already_seen = 0;

            for (int i = 0;
                 i < seen_count;
                 i++)
            {
                if (seen_ids[i] ==
                    current->allocation_id)
                {
                    already_seen = 1;
                    break;
                }
            }

            if (!already_seen &&
                seen_count < 1000)
            {
                seen_ids[seen_count] =
                    current->allocation_id;

                seen_count++;

                active_allocations++;
            }
        }

        current =
            current->next;
    }


    /*
     * Calculate utilization.
     */
    double utilization = 0.0;

    if (memory_pool_size > 0)
    {
        utilization =
            ((double)used_memory /
             memory_pool_size) * 100.0;
    }


    /*
     * Display monitoring information.
     */
    printf("\n");

    printf(
        "========================================================\n"
    );

    printf(
        "                 MEMORY MONITORING\n"
    );

    printf(
        "========================================================\n"
    );

    printf(
        "Total Memory       : %zu KB\n",
        memory_pool_size
    );

    printf(
        "Used Memory        : %zu KB\n",
        used_memory
    );

    printf(
        "Free Memory        : %zu KB\n",
        free_memory
    );

    printf(
        "Utilization        : %.2f%%\n",
        utilization
    );

    printf(
        "--------------------------------------------------------\n"
    );

    printf(
        "Active Allocations : %d\n",
        active_allocations
    );

    printf(
        "Free Blocks        : %d\n",
        free_blocks
    );

    printf(
        "Largest Free Block : %zu KB\n",
        largest_free_block
    );

    printf(
        "Current Strategy   : %s\n",
        get_strategy_name()
    );

    printf(
        "--------------------------------------------------------\n"
    );

    /*
     * Memory condition.
     */
    if (free_memory == 0)
    {
        printf(
            "Memory Status      : FULL\n"
        );
    }
    else if (utilization >= 80.0)
    {
        printf(
            "Memory Status      : HIGH USAGE\n"
        );
    }
    else
    {
        printf(
            "Memory Status      : NORMAL\n"
        );
    }

    printf(
        "========================================================\n"
    );
}


/* =========================================================
   CLEAR MEMORY VISUALIZATION
   ========================================================= */

/* Width of the text area inside one block box. */
#define BOX_TEXT_WIDTH 27


/* Print the top / bottom border of a block box. */
static void print_box_border(void)
{
    printf("+");

    for (int i = 0; i < BOX_TEXT_WIDTH + 2; i++)
    {
        printf("-");
    }

    printf("+\n");
}


/* Print one text line inside a block box. */
static void print_box_line(const char *text)
{
    printf("| %-*.*s |\n",
           BOX_TEXT_WIDTH,
           BOX_TEXT_WIDTH,
           text);
}


void display_memory_visual(void)
{
    MemoryBlock *current;
    int block_number;

    size_t assigned_memory = 0;
    size_t free_memory = 0;

    printf("\n");
    printf("==============================================================\n");
    printf("                  MEMORY VISUALIZATION\n");
    printf("==============================================================\n");

    printf("\n");
    printf("Total Memory: %zu KB\n", memory_pool_size);

    printf("\n");
    printf("MEMORY MAP\n");
    printf("--------------------------------------------------------------\n");

    /*
     * Every PHYSICAL block is drawn as its own box.
     * Blocks of the same allocation are NOT merged;
     * they are linked only by the same allocation ID.
     */
    current = block_list;
    block_number = 1;

    while (current != NULL)
    {
        char line[64];

        printf("\n");
        printf("Block %d\n", block_number);

        print_box_border();

        /*
         * FREE BLOCK
         */
        if (current->free)
        {
            print_box_line("FREE");
            print_box_line("AVAILABLE");

            snprintf(line, sizeof(line), "%zu KB", current->size);
            print_box_line(line);

            free_memory += current->size;
        }

        /*
         * USED BLOCK
         */
        else
        {
            int segment_count =
                count_allocation_segments(
                    current->allocation_id
                );

            int segment_number = 1;

            MemoryBlock *scan = block_list;

            /*
             * Find this segment's position
             * within the same allocation.
             */
            while (scan != current)
            {
                if (!scan->free &&
                    scan->allocation_id ==
                        current->allocation_id)
                {
                    segment_number++;
                }

                scan = scan->next;
            }

            snprintf(line, sizeof(line),
                     "A%03d", current->allocation_id);
            print_box_line(line);

            print_box_line("ALLOCATED");

            snprintf(line, sizeof(line), "%zu KB", current->size);
            print_box_line(line);

            if (segment_count == 1)
            {
                print_box_line("CONTIGUOUS");
            }
            else
            {
                snprintf(line, sizeof(line),
                         "NON-CONTIGUOUS - PART %d",
                         segment_number);
                print_box_line(line);
            }

            assigned_memory += current->size;
        }

        print_box_border();

        current = current->next;
        block_number++;
    }



    /*
     * ---------------------------------------------------------
     * MEMORY SUMMARY
     * ---------------------------------------------------------
     */

    printf("\n");
    printf("==============================================================\n");
    printf("                    MEMORY SUMMARY\n");
    printf("==============================================================\n");

    printf("Assigned Memory : %zu KB\n", assigned_memory);
    printf("Free Memory     : %zu KB\n", free_memory);
    printf("Total Memory    : %zu KB\n", memory_pool_size);

    if (memory_pool_size > 0)
    {
        double assigned_percentage =
            ((double)assigned_memory /
             memory_pool_size) * 100.0;

        double free_percentage =
            ((double)free_memory /
             memory_pool_size) * 100.0;

        printf("Assigned        : %.2f%%\n",
               assigned_percentage);

        printf("Free            : %.2f%%\n",
               free_percentage);
    }

    printf("==============================================================\n");


    /*
     * ---------------------------------------------------------
     * NON-CONTIGUOUS ALLOCATIONS
     *
     * Each allocation split over several physical blocks is
     * listed once, with the blocks that belong to it.
     * ---------------------------------------------------------
     */

    printf("\n");
    printf("NON-CONTIGUOUS ALLOCATIONS\n");
    printf("--------------------------------------------------------------\n");

    int found_non_contiguous = 0;

    current = block_list;

    while (current != NULL)
    {
        if (!current->free)
        {
            int segment_count =
                count_allocation_segments(
                    current->allocation_id
                );

            /*
             * Is this the first segment of the allocation?
             */
            int is_first = 1;

            MemoryBlock *scan = block_list;

            while (scan != current)
            {
                if (!scan->free &&
                    scan->allocation_id ==
                        current->allocation_id)
                {
                    is_first = 0;
                    break;
                }

                scan = scan->next;
            }

            if (segment_count > 1 && is_first)
            {
                size_t total = 0;
                int index = 1;
                int printed = 0;

                found_non_contiguous = 1;

                printf("A%03d -> ", current->allocation_id);

                scan = block_list;

                while (scan != NULL)
                {
                    if (!scan->free &&
                        scan->allocation_id ==
                            current->allocation_id)
                    {
                        if (printed > 0)
                        {
                            printf(" + ");
                        }

                        printf("Block %d (%zu KB)",
                               index,
                               scan->size);

                        total += scan->size;
                        printed++;
                    }

                    scan = scan->next;
                    index++;
                }

                printf("\n");
                printf("       Total = %zu KB\n", total);
            }
        }

        current = current->next;
    }

    if (!found_non_contiguous)
    {
        printf("None\n");
    }


    /*
     * ---------------------------------------------------------
     * EXPLANATION
     * ---------------------------------------------------------
     */

    printf("\n");
    printf("HOW TO READ THE MEMORY MAP\n");
    printf("--------------------------------------------------------------\n");

    printf("ALLOCATED      = Memory currently assigned to an allocation\n");
    printf("FREE           = Memory currently available for allocation\n");
    printf("CONTIGUOUS     = Allocation occupies one physical block\n");
    printf("NON-CONTIGUOUS = One allocation occupies multiple physical blocks\n");

    printf("\n");
    printf("IMPORTANT:\n");
    printf("Physical blocks are NOT merged during non-contiguous allocation.\n");
    printf("The same Allocation ID identifies the different physical segments.\n");

    printf("==============================================================\n");
}


/* =========================================================
   FRAGMENTATION ANALYSIS
   ========================================================= */

void display_fragmentation(void)
{
    MemoryBlock *current = block_list;

    size_t total_free = 0;
    size_t largest_free = 0;

    int free_blocks = 0;

    /*
     * Internal fragmentation:
     * Allocated memory - actually requested memory.
     */
    size_t internal_fragmentation = 0;

    /*
     * Calculate free memory and internal fragmentation.
     */
    while (current != NULL)
    {
        /*
         * FREE BLOCK
         */
        if (current->free)
        {
            total_free += current->size;
            free_blocks++;

            if (current->size > largest_free)
            {
                largest_free = current->size;
            }
        }

        /*
         * USED BLOCK
         */
        else
        {
            /*
             * Only the first physical segment of a logical
             * allocation stores requested_size.
             *
             * Therefore, calculate internal fragmentation
             * once per allocation.
             */
            if (current->requested_size > 0)
            {
                /*
                 * Find all physical segments belonging
                 * to this logical allocation.
                 */
                size_t allocation_total = 0;

                MemoryBlock *inner = block_list;

                while (inner != NULL)
                {
                    if (!inner->free &&
                        inner->allocation_id ==
                            current->allocation_id)
                    {
                        allocation_total += inner->size;
                    }

                    inner = inner->next;
                }

                /*
                 * Allocated physical memory minus the
                 * memory actually requested by the user.
                 */
                if (allocation_total >=
                    current->requested_size)
                {
                    internal_fragmentation +=
                        allocation_total -
                        current->requested_size;
                }
            }
        }

        current = current->next;
    }


    /*
     * External fragmentation.
     *
     * All free memory exists outside allocations.
     * The largest free block is the largest contiguous
     * free region that can satisfy a single contiguous
     * request.
     *
     * Therefore:
     *
     * External fragmentation =
     * Total free memory - Largest free block
     */
    size_t external_fragmentation = 0;

    if (total_free > largest_free)
    {
        external_fragmentation =
            total_free - largest_free;
    }


    /*
     * External fragmentation percentage.
     */
    double external_percentage = 0.0;

    if (total_free > 0)
    {
        external_percentage =
            ((double)external_fragmentation /
             total_free) * 100.0;
    }


    /*
     * Display results.
     */
    printf("\n");
    printf(
        "========================================================\n"
    );

    printf(
        "              FRAGMENTATION ANALYSIS\n"
    );

    printf(
        "========================================================\n"
    );

    printf(
        "Total Free Memory        : %zu KB\n",
        total_free
    );

    printf(
        "Number of Free Blocks    : %d\n",
        free_blocks
    );

    printf(
        "Largest Free Block       : %zu KB\n",
        largest_free
    );

    printf(
        "--------------------------------------------------------\n"
    );

    printf(
        "Internal Fragmentation   : %zu KB\n",
        internal_fragmentation
    );

    printf(
        "External Fragmentation   : %zu KB\n",
        external_fragmentation
    );

    printf(
        "External Fragmentation %% : %.2f%%\n",
        external_percentage
    );

    printf(
        "========================================================\n"
    );
}


/* =========================================================
   CLEANUP
   ========================================================= */

void memory_cleanup(void)
{
    MemoryBlock *current =
        block_list;


    while (current != NULL)
    {
        MemoryBlock *next =
            current->next;


        free(current);

        current =
            next;
    }


    free(memory_pool);

    memory_pool = NULL;

    block_list = NULL;

    memory_pool_size = 0;

    next_allocation_id = 1;
}

/* =========================================================
   ALLOCATION BENCHMARK

   Compares First Fit, Best Fit and Worst Fit by replaying ONE
   fixed request sequence through the real allocator code
   (my_malloc / my_free_block) on a separate scratch pool.

   The real pool, block list, strategy, mode and ID counter are
   saved first and restored exactly afterwards.
   ========================================================= */

#define BENCH_POOL_KB 100

typedef struct
{
    int    is_alloc;   /* 1 = allocate, 0 = free             */
    size_t size;       /* KB requested (allocate only)       */
    int    target;     /* allocation number to free (free)   */
} BenchOp;

/*
 * Fixed test workload (never changes between strategies or runs).
 *
 * Allocation numbers are 1, 2, 3 ... in the order the allocate
 * operations appear. A free refers to that number.
 *
 * It fills the pool, frees blocks to leave free holes of different
 * sizes, then makes requests that fit the holes differently.
 * The counts are measured by running the real allocator, not typed in.
 */
static const BenchOp bench_workload[] =
{
    {1, 30, 0},  /* alloc  1 */
    {1, 10, 0},  /* alloc  2 */
    {1, 20, 0},  /* alloc  3 */
    {1, 10, 0},  /* alloc  4 */
    {1, 20, 0},  /* alloc  5 */
    {1, 10, 0},  /* alloc  6 */
    {0,  0, 1},  /* free   1 */
    {0,  0, 3},  /* free   3 */
    {0,  0, 5},  /* free   5 */
    {1, 15, 0},  /* alloc  7 */
    {1, 20, 0},  /* alloc  8 */
    {1, 30, 0}   /* alloc  9 */
};

#define BENCH_OPS ((int)(sizeof(bench_workload) / sizeof(bench_workload[0])))
#define BENCH_MAX_ALLOCS 64

typedef struct
{
    int    allocations;
    int    failures;
    size_t internal_fragmentation;
    size_t external_fragmentation;
} BenchResult;

static BenchResult bench_results[3];   /* First, Best, Worst */
static int  bench_valid = 0;
static int  bench_requests = 0;
static char bench_mode_name[32] = "";


/*
 * Redirect stdout to /dev/null so the existing my_malloc() /
 * my_free_block() messages do not flood the menu.
 */
static int bench_silence_stdout(void)
{
    fflush(stdout);

    int saved = dup(STDOUT_FILENO);

    if (saved < 0)
    {
        return -1;
    }

    int devnull = open("/dev/null", O_WRONLY);

    if (devnull < 0)
    {
        close(saved);
        return -1;
    }

    dup2(devnull, STDOUT_FILENO);
    close(devnull);

    return saved;
}

static void bench_restore_stdout(int saved)
{
    if (saved < 0)
    {
        return;
    }

    fflush(stdout);
    dup2(saved, STDOUT_FILENO);
    close(saved);
}


/*
 * Internal / external fragmentation of the CURRENT block list.
 * Same definitions as the dashboard export.
 */
static void bench_measure_fragmentation(
    size_t *internal,
    size_t *external)
{
    size_t total_free = 0;
    size_t largest_free = 0;

    *internal = 0;

    MemoryBlock *current = block_list;

    while (current != NULL)
    {
        if (current->free)
        {
            total_free += current->size;

            if (current->size > largest_free)
            {
                largest_free = current->size;
            }
        }
        else if (current->requested_size > 0)
        {
            size_t allocation_total = 0;

            MemoryBlock *inner = block_list;

            while (inner != NULL)
            {
                if (!inner->free &&
                    inner->allocation_id == current->allocation_id)
                {
                    allocation_total += inner->size;
                }

                inner = inner->next;
            }

            if (allocation_total >= current->requested_size)
            {
                *internal +=
                    allocation_total - current->requested_size;
            }
        }

        current = current->next;
    }

    *external =
        total_free > largest_free
            ? total_free - largest_free
            : 0;
}


int run_allocation_benchmark(void)
{
    if (memory_pool == NULL)
    {
        printf("\nNo memory pool exists.\n");
        return 0;
    }

    static const AllocationStrategy strategies[3] =
    {
        FIRST_FIT,
        BEST_FIT,
        WORST_FIT
    };

    /*
     * ---- Save the REAL allocator state ----
     */
    unsigned char      *saved_pool      = memory_pool;
    size_t              saved_pool_size = memory_pool_size;
    MemoryBlock        *saved_list      = block_list;
    AllocationStrategy  saved_strategy  = current_strategy;
    AllocationMode      saved_mode      = current_mode;
    int                 saved_next_id   = next_allocation_id;

    BenchResult results[3];
    int ok = 1;
    int request_count = 0;

    int saved_out = bench_silence_stdout();

    for (int s = 0; s < 3 && ok; s++)
    {
        /*
         * Fresh scratch pool for this strategy.
         * (Globals are detached first so the real pool is
         * never freed or modified.)
         */
        memory_pool      = NULL;
        memory_pool_size = 0;
        block_list       = NULL;

        if (!memory_init(BENCH_POOL_KB))
        {
            ok = 0;
            break;
        }

        current_strategy = strategies[s];
        current_mode     = saved_mode;

        results[s].allocations = 0;
        results[s].failures    = 0;

        int succeeded[BENCH_MAX_ALLOCS + 1] = {0};
        int alloc_number = 0;

        for (int i = 0; i < BENCH_OPS; i++)
        {
            const BenchOp *op = &bench_workload[i];

            if (op->is_alloc)
            {
                alloc_number++;

                /*
                 * memory_init() reset the ID counter to 1 and
                 * my_malloc() consumes one ID per request, so
                 * request N always receives allocation ID N.
                 */
                void *ptr = my_malloc(op->size);

                if (ptr != NULL)
                {
                    results[s].allocations++;
                    succeeded[alloc_number] = 1;
                }
                else
                {
                    results[s].failures++;
                }
            }
            else if (succeeded[op->target])
            {
                my_free_block(op->target);
                succeeded[op->target] = 0;
            }
        }

        request_count = alloc_number;

        bench_measure_fragmentation(
            &results[s].internal_fragmentation,
            &results[s].external_fragmentation
        );

        /*
         * Release the scratch pool (acts on scratch globals only).
         */
        memory_cleanup();
    }

    /*
     * ---- Restore the REAL allocator state exactly ----
     */
    memory_pool        = saved_pool;
    memory_pool_size   = saved_pool_size;
    block_list         = saved_list;
    current_strategy   = saved_strategy;
    current_mode       = saved_mode;
    next_allocation_id = saved_next_id;

    bench_restore_stdout(saved_out);

    if (!ok)
    {
        printf("\nBenchmark failed: could not create scratch pool.\n");
        return 0;
    }

    for (int s = 0; s < 3; s++)
    {
        bench_results[s] = results[s];
    }

    bench_requests = request_count;
    bench_valid = 1;

    snprintf(
        bench_mode_name,
        sizeof(bench_mode_name),
        "%s",
        get_allocation_mode_name()
    );

    /*
     * ---- Show results ----
     */
    printf("\n");
    printf("========================================================\n");
    printf("                 ALLOCATION BENCHMARK\n");
    printf("========================================================\n");
    printf("Mode            : %s\n", bench_mode_name);
    printf("Scratch pool    : %d KB (real pool untouched)\n",
           BENCH_POOL_KB);
    printf("Requests        : %d allocation requests, same sequence\n",
           bench_requests);
    printf("--------------------------------------------------------\n");
    printf("%-11s %9s %8s %10s %10s\n",
           "Strategy", "Success", "Failed", "Internal", "External");

    static const char *names[3] =
        { "First Fit", "Best Fit", "Worst Fit" };

    for (int s = 0; s < 3; s++)
    {
        printf("%-11s %9d %8d %7zu KB %7zu KB\n",
               names[s],
               bench_results[s].allocations,
               bench_results[s].failures,
               bench_results[s].internal_fragmentation,
               bench_results[s].external_fragmentation);
    }

    printf("========================================================\n");
    printf("Fragmentation is measured after the last request.\n");

    /*
     * Publish to the dashboard.
     */
    export_memory_status();

    return 1;
}


/* =========================================================
   DASHBOARD DATA EXPORT
   ========================================================= */

void export_memory_status(void)
{
    FILE *file = fopen("memory_status.json.tmp", "w");

    if (file == NULL)
    {
        printf("\nError: Could not create memory_status.json\n");
        return;
    }

    size_t used_memory = 0;
    size_t free_memory = 0;

    size_t internal_fragmentation = 0;
    size_t largest_free = 0;

    int free_blocks = 0;
    int active_allocations = 0;

    /*
     * -----------------------------------------------------
     * Calculate memory and fragmentation information
     * -----------------------------------------------------
     */

    MemoryBlock *current = block_list;

    while (current != NULL)
    {
        if (current->free)
        {
            free_memory += current->size;
            free_blocks++;

            if (current->size > largest_free)
            {
                largest_free = current->size;
            }
        }
        else
        {
            used_memory += current->size;

            /*
             * requested_size is stored only on the
             * first physical segment of an allocation.
             */
            if (current->requested_size > 0)
            {
                active_allocations++;

                size_t allocation_total = 0;

                MemoryBlock *inner = block_list;

                while (inner != NULL)
                {
                    if (!inner->free &&
                        inner->allocation_id ==
                            current->allocation_id)
                    {
                        allocation_total += inner->size;
                    }

                    inner = inner->next;
                }

                if (allocation_total >=
                    current->requested_size)
                {
                    internal_fragmentation +=
                        allocation_total -
                        current->requested_size;
                }
            }
        }

        current = current->next;
    }

    /*
     * External fragmentation:
     *
     * Total free memory - largest free block
     */
    size_t external_fragmentation = 0;

    if (free_memory > largest_free)
    {
        external_fragmentation =
            free_memory - largest_free;
    }

    /*
     * -----------------------------------------------------
     * JSON HEADER
     * -----------------------------------------------------
     */

    fprintf(file, "{\n");

    fprintf(
        file,
        "  \"connected\": true,\n"
    );

    /*
     * Process ID lets the Node server confirm this allocator
     * is still running (checks /proc/<pid>).
     */
    fprintf(
        file,
        "  \"pid\": %d,\n",
        (int)getpid()
    );

    fprintf(
        file,
        "  \"totalMemory\": %zu,\n",
        memory_pool_size
    );

    fprintf(
        file,
        "  \"usedMemory\": %zu,\n",
        used_memory
    );

    fprintf(
        file,
        "  \"freeMemory\": %zu,\n",
        free_memory
    );

    fprintf(
        file,
        "  \"utilization\": %.2f,\n",
        memory_pool_size > 0
            ? ((double)used_memory /
               memory_pool_size) * 100.0
            : 0.0
    );

    fprintf(
        file,
        "  \"strategy\": \"%s\",\n",
        get_strategy_name()
    );

    fprintf(
        file,
        "  \"allocationMode\": \"%s\",\n",
        get_allocation_mode_name()
    );

    fprintf(
        file,
        "  \"activeAllocations\": %d,\n",
        active_allocations
    );

    fprintf(
        file,
        "  \"freeBlocks\": %d,\n",
        free_blocks
    );

    fprintf(
        file,
        "  \"largestFree\": %zu,\n",
        largest_free
    );

    fprintf(
        file,
        "  \"internalFragmentation\": %zu,\n",
        internal_fragmentation
    );

    fprintf(
        file,
        "  \"externalFragmentation\": %zu,\n",
        external_fragmentation
    );

    /*
     * -----------------------------------------------------
     * MEMORY BLOCKS
     * -----------------------------------------------------
     */

    fprintf(file, "  \"blocks\": [\n");

    current = block_list;

    int first_block = 1;

    while (current != NULL)
    {
        if (!first_block)
        {
            fprintf(file, ",\n");
        }

        first_block = 0;

        fprintf(file, "    {\n");

        fprintf(
            file,
            "      \"offset\": %zu,\n",
            current->offset
        );

        fprintf(
            file,
            "      \"size\": %zu,\n",
            current->size
        );

        fprintf(
            file,
            "      \"requestedSize\": %zu,\n",
            current->requested_size
        );

        fprintf(
            file,
            "      \"free\": %s,\n",
            current->free ? "true" : "false"
        );

        fprintf(
            file,
            "      \"allocationId\": %d\n",
            current->allocation_id
        );

        fprintf(file, "    }");

        current = current->next;
    }

    fprintf(file, "\n  ]");

    /*
     * -----------------------------------------------------
     * BENCHMARK RESULTS (only after the benchmark has run)
     * -----------------------------------------------------
     */
    if (bench_valid)
    {
        static const char *keys[3] =
            { "firstFit", "bestFit", "worstFit" };

        fprintf(file, ",\n  \"performance\": {\n");

        fprintf(file, "    \"mode\": \"%s\",\n", bench_mode_name);
        fprintf(file, "    \"poolSize\": %d,\n", BENCH_POOL_KB);
        fprintf(file, "    \"requests\": %d,\n", bench_requests);

        for (int s = 0; s < 3; s++)
        {
            fprintf(
                file,
                "    \"%s\": {\"allocations\": %d, "
                "\"failures\": %d, "
                "\"internalFragmentation\": %zu, "
                "\"externalFragmentation\": %zu, "
                "\"fragmentation\": %zu}%s\n",
                keys[s],
                bench_results[s].allocations,
                bench_results[s].failures,
                bench_results[s].internal_fragmentation,
                bench_results[s].external_fragmentation,
                bench_results[s].internal_fragmentation +
                    bench_results[s].external_fragmentation,
                s < 2 ? "," : ""
            );
        }

        fprintf(file, "  }");
    }

    fprintf(file, "\n}\n");

    fclose(file);

    /*
     * Atomic swap: the dashboard never reads a half-written file.
     */
    rename(
        "memory_status.json.tmp",
        "memory_status.json"
    );
}
