# Week-8: FIFO Communication in C

## Aim
To implement inter-process communication (IPC) using named pipes (FIFOs) in C, with and without the `fork()` system call.

## Programs

| Program | Description |
|---|---|
| `message_userA.c` | User A sends messages through FIFO communication. |
| `message_userB.c` | User B receives and replies to User A. |
| `message_userA_parent_child.c` | Demonstrates FIFO communication using parent and child processes for User A. |
| `message_userB_parent_child.c` | Demonstrates FIFO communication using parent and child processes for User B. |

## Concepts Used
- Named pipes (FIFOs)
- Inter-process communication (IPC)
- `mkfifo()`, `open()`, `read()`, `write()`, and `close()`
- `fork()` for process creation
- `wait()` for process synchronization

## Requirements
- Linux or Ubuntu
- GCC compiler
- Terminal

## Compilation

Compile each program separately:

```bash
gcc message_userA.c -o message_userA
gcc message_userB.c -o message_userB
gcc message_userA_parent_child.c -o message_userA_parent_child
gcc message_userB_parent_child.c -o message_userB_parent_child
```

## Execution

### Programs 1 and 2: Basic FIFO Communication

1. Open two terminals in this folder.
2. In Terminal 1, run:

   ```bash
   ./message_userA
   ```

3. In Terminal 2, run:

   ```bash
   ./message_userB
   ```

4. Exchange messages between the users.
5. Type `bye` to end the conversation.

### Programs 3 and 4: FIFO Communication Using `fork()`

1. Open two terminals in this folder.
2. In Terminal 1, run:

   ```bash
   ./message_userA_parent_child
   ```

   Enter `A` when prompted.

3. In Terminal 2, run:

   ```bash
   ./message_userA_parent_child
   ```

   Enter `B` when prompted.

4. Exchange messages between the two instances.
5. Type `bye` to end the conversation.

**Note:** Programs 3 and 4 are alternative implementations of the two sides of the parent-child FIFO chat. For this version, run the same program in both terminals, choosing opposite users.

## Expected Result
Successful exchange of messages between processes using named FIFOs, demonstrating IPC in Linux.

## Conclusion
FIFO communication was implemented in C using named pipes. The programs demonstrate message transfer between processes, including a version using `fork()`.
