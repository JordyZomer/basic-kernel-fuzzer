CC=gcc

fuzzer: basic_fuzzer.c
     $(CC) -o basic_fuzzer basic_fuzzer.c
