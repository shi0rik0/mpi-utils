# mpi-utils

This repository contains some utilities of MPI. C++17 is required.

## Test

Run `make test` to test the utilities. The following output is expected (order may vary):

```
Rank 0: local data: 1 2
Rank 1: local data: 3 4 5 6
Rank 2: local data: 7 8 9
Rank 3: local data:
Rank 0: gathered data: {{2, 4}, {6, 8, 10, 12}, {14, 16, 18}, {}}
```