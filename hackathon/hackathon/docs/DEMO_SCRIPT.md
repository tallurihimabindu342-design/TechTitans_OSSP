# Live Demo Script (5 minutes)

0. Before: `make clean && make && make test` - all PASS.
1. Intro (30s): problem statement, repo structure.
2. `./pagesim -d 250` (60s): show a HIT, a cold-start FAULT, an eviction (red frame).
3. `./pagesim -q` (45s): FIFO 15, LRU 12, OPT 9; OPT is the lower bound.
4. `./pagesim -q -f 4 -r "1 2 3 4 1 2 5 1 2 3 4 5"` (45s): custom input.
5. `./pagesim -q -b` (60s): Belady's anomaly, FIFO 9 faults (3 frames) vs 10 (4 frames).
6. `./pagesim -q -n 40 -m 8 -f 4` (30s): random stress run.
7. `make test` (30s).
8. Code walkthrough: simulate() and the victim selection per algorithm.

Backup: screen-record steps 2-7 beforehand.

## Likely viva questions
- Why can't OPT be implemented in a real OS? (needs future knowledge)
- What is Belady's anomaly? Which algorithms are immune? (stack algorithms: LRU, OPT)
- How does Linux approximate LRU? (active/inactive lists, referenced bits)
- Complexity of each algorithm?
- What happens with 1 frame? With frames >= distinct pages?
