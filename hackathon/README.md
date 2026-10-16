# Virtual Memory Page Replacement Visualizer

Linux / C simulator that visually compares **FIFO**, **LRU** and **Optimal (OPT)**
page replacement algorithms by number of page faults.

**Team:** TechTitans_OSSP

| Roll No | Name | Role |
|---------|------|------|
| 2520030601 | Billakanti Siri | FIFO + LRU implementation |
| 2520030082 | Bhavana Kondakrindi | Optimal algorithm + testing |
| 2520030484 | T. HimaBindu Sree | Visualization + documentation |

**Hackathon:** 17.10.2026

## Prerequisites
- Linux (Ubuntu), WSL Ubuntu, or a Linux VM
- `gcc` and `make`: `sudo apt update && sudo apt install build-essential`

## Build

make

(or manually: `gcc -Wall -Wextra -O2 -o pagesim pagesim.c`)

## Run

./pagesim # default textbook string, 3 frames
./pagesim -f 4 -r “1 2 3 4 1 2 5 1 2 3 4 5” # custom frames and reference string
./pagesim -n 30 -m 8 -f 3 # 30 random references, pages 0..7
./pagesim -d 300 # animated, 300 ms per step
./pagesim -q -b # summary only + Belady’s anomaly demo


| Flag | Meaning |
|------|---------|
| `-f N` | number of frames (1-10, default 3) |
| `-r "..."` | reference string (space or comma separated, max 200) |
| `-n N -m M` | generate N random references over pages 0..M-1 |
| `-d MS` | delay per step in ms (animation) |
| `-b` | Belady's anomaly demo (FIFO, 1-7 frames) |
| `-q` | quiet: summary and chart only |

## Test

make test

Runs 12 test cases: textbook results, edge cases, an OPT invariant and error handling.

## Sample output (3 frames, default string)

Algo Faults Hits Hit-rate Faults
FIFO 15 5 25.0% ##############################
LRU 12 8 40.0% ########################
OPTIMAL 9 11 55.0% ##################


## Safety
Pure user-space simulation. It does not touch real page tables, kill processes
or use shared resources, so it is safe to run on any machine.

## References
- Silberschatz, Galvin, Gagne - *Operating System Concepts*, Virtual Memory chapter
- Belady, Nelson, Shedler (1969) - anomaly in paging algorithms
./pagesim -i                                   # interactive: asks for frames and string
