# mm-allocator

a simple memory allocator written in pure C with reimplementations of `malloc`,`calloc`,`realloc` and `free` using _singly linked lists_

## How to use

1. Clone the repository
2. Compile the source code using the following command

```bash
bash compile.sh
```

3. Run the compiled executable. I'll test it with the `ls` command

```bash
LD_PRELOAD=./mm.so /usr/bin/ls # or just ls or any other program
```

The way this works is that the `LD_PRELOAD` environment variable is used to load the shared library `mm.so` before the `ls` command is executed. This will cause the `malloc`, `calloc`, `realloc` and `free` functions to be redirected to the ones in our custom memory allocator. By default, `LD_PRELOAD` points to the standard C library, so we are essentially replacing the standard memory allocation functions with our custom ones.

You can see it in action [here](https://asciinema.org/a/gsY8AQNKLgzAmSdu7F1f24kdT)

## NOTE

This program is quite limited in functionality and is not meant to be used in production. It is just a simple demonstration of how memory allocation works under the hood. The program is not thread-safe and does not handle memory fragmentation. It is just a simple implementation to understand the basic concepts of memory allocation. Therefore it can seriously segfault if used inappropriately.
