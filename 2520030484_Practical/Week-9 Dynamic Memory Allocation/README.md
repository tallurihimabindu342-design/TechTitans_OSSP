# Week-9: Dynamic Memory Allocation in C

## Aim
To understand and implement memory allocation using `malloc()`, `calloc()`, `realloc()`, and `free()` in C.

## Programs

| File | Description |
|---|---|
| `malloc.c` | Allocates memory dynamically at runtime using `malloc()`. |
| `array.c` | Demonstrates a fixed-size array. |
| `calloc.c` | Allocates memory dynamically and initializes it to zero. |
| `realloc.c` | Changes the size of previously allocated memory. |

## Functions Used

- **malloc():** Allocates a block of memory.
- **calloc():** Allocates memory for multiple elements and initializes it to zero.
- **realloc():** Changes the size of an allocated memory block.
- **free():** Releases dynamically allocated memory.

## Requirements
- Ubuntu or Linux
- GCC compiler
- C programming knowledge

## Compilation and Execution

### 1. malloc()
```bash
gcc malloc.c -o malloc
./malloc
```

### 2. Fixed-size array
```bash
gcc array.c -o array
./array
```

### 3. calloc()
```bash
gcc calloc.c -o calloc
./calloc
```

### 4. realloc()
```bash
gcc realloc.c -o realloc
./realloc
```

## Expected Result
All four programs compile and run successfully, demonstrating fixed-size arrays and dynamic memory allocation in C.

## Conclusion
The programs demonstrate how memory can be allocated, initialized, resized, and released using C memory management functions.
