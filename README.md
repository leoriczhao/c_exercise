# C Exercise Collection

A curated collection of C programming exercises covering various fundamental concepts.

## Table of Contents

| Exercise | File | Topic |
|----------|------|-------|
| 1 | ex1.c | Basic I/O and ANSI color codes |
| 3 | ex3.c | Basic output |
| 4 | ex4.c | String manipulation |
| 5 | ex5.c | Simple expressions |
| 6 | ex6.c | Control structures |
| 7 | ex7.c | Arrays |
| 8 | ex8.c | More arrays |
| 9 | ex9.c | Pointers |
| 10 | ex10.c | More pointers |
| 11 | ex11.c | Structures |
| 12 | ex12.c | More structures |
| 13 | ex13.c | File I/O |
| 14 | ex14.c | Preprocessor |
| 15 | ex15.c | Arrays and pointers together |
| 16 | ex16.c | Multidimensional arrays |
| 17 | ex17.c | Advanced data structures |
| 18 | ex18.c | Memory management |

## Quick Start

```bash
# Clone the repository
git clone https://github.com/leoriczhao/c_exercise.git
cd c_exercise

# Build all exercises (requires clang/gcc + make)
make

# Run a specific exercise
./ex1

# Clean all built executables
make clean
```

## Build Individual Exercises

If you want to compile and run a single exercise quickly:

```bash
# Using cc/clang
cc -O2 -Wall -Wextra ex1.c -o ex1
./ex1

# Using gcc
gcc -O2 -Wall -Wextra ex1.c -o ex1
./ex1
```

## Project Structure

- `ex*.c` - Individual exercise source files
- `Makefile` - Build configuration for all exercises
- `test17.py` - Test script for exercise 17
- `md/` - Additional documentation or markdown files

## Author

Created by Zhao Zelong (leoriczhao) as a learning resource for C programming.

## License

Check the repository for licensing information.
