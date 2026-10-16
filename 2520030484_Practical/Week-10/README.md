# Week-10: Banker's Algorithm

## Aim
To implement Banker's Algorithm in C to check whether the system is in a safe state and find the safe sequence.

## Concepts Used
- Deadlock avoidance
- Available resources
- Allocated resources
- Maximum resources
- Need matrix
- Safe state and safe sequence

## Formula
**Need = Maximum - Allocated**

## Requirements
- Ubuntu/Linux
- GCC compiler
- C programming language

## Compilation
```bash
gcc bankers_algorithm.c -o bankers_algorithm
```

## Execution
```bash
./bankers_algorithm
```

Enter the number of processes, number of resource types, available resources, allocated resources, and maximum resources when prompted.

## Expected Result
The program displays the resource tables, calculates the Need matrix, and reports whether the system is in a safe state. If safe, it displays the safe sequence.

## Conclusion
Banker's Algorithm was implemented in C to check system safety and determine a safe sequence of processes.
