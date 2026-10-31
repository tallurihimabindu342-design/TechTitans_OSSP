#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

/*
 * Allocation strategies
 */
typedef enum
{
    FIRST_FIT = 1,
    BEST_FIT,
    WORST_FIT
} AllocationStrategy;

/*
 * Allocation modes
 */
typedef enum {
    CONTIGUOUS_MODE = 1,
    NON_CONTIGUOUS_MODE
} AllocationMode;

/*
 * Represents one physical memory block.
 *
 * A logical allocation may own one block
 * (contiguous) or multiple blocks
 * (non-contiguous).
 */
typedef struct MemoryBlock
{
    size_t offset;

    /*
     * Actual memory occupied by this block.
     * This may be larger than requested_size
     * because of internal fragmentation.
     */
    size_t size;

    /*
     * Memory actually requested by the user.
     */
    size_t requested_size;

    int free;

    /*
     * 0 = FREE
     * >0 = Allocation ID
     */
    int allocation_id;

    struct MemoryBlock *next;

} MemoryBlock;


/*
 * Memory management functions
 */
int memory_init(size_t total_size);

void memory_cleanup(void);

void set_allocation_strategy(
    AllocationStrategy strategy
);

void set_allocation_mode(AllocationMode mode);

void *my_malloc(size_t size);

void *my_realloc(int allocation_id, size_t new_size);

void my_free_block(int allocation_id);

void display_memory(void);

void display_fragmentation(void);

void display_memory_monitor(void);

void display_memory_visual(void);

const char *get_strategy_name(void);

const char *get_allocation_mode_name(void);

void export_memory_status(void);

/*
 * Runs First/Best/Worst Fit on a separate scratch pool using one
 * fixed request sequence. The real allocator state is untouched.
 * Returns 1 on success, 0 on failure.
 */
int run_allocation_benchmark(void);

#endif
