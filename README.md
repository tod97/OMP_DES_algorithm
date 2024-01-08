# Mid Term Parallel Computing

This project aims to compare the performance of sequential and parallel Data Encryption Standard (DES) algorithms using the OpenMP library.

## Installation

To be able to run this code, you need to install OpenMP and use, preferably, g++ as C++ compiler.

### Things to consider

- Before running the main.cpp, be sure to create **words.txt** file with utils/word_gen.py and put it in the root folder
- Compiling, be sure to use the flag **-fopenmp** to enable OpenMP directives
- The OpenMP directive *pragma omp cancel parallel* needs as environment variable **OMP_CANCELLATION=true** otherwise threads won't stop when they find the searched item
