#include <pthread.h> // pthread_mutex_unlock,pthread_mutex_lock,pthread_mutex_t
#include <stddef.h> // size_t
#include <unistd.h> // sbrk

typedef void* void_ptr_t;
typedef unsigned char byte_t;
typedef unsigned int uint_t;

// this struct is already memory-aligned for x86-64 processors
typedef struct header_t {
    size_t size;
    byte_t is_free; // status of mem block
    struct header_t* next;
} header_info_t;

// prototypes
header_info_t* get_free_block(size_t size);
void_ptr_t malloc(size_t size);
void_ptr_t calloc(size_t num, size_t nsize);
void_ptr_t realloc(void* block, size_t size);
void free(void_ptr_t block);
void_ptr_t mm_mem_set(void_ptr_t src, int c, size_t n);
void_ptr_t mm_mem_copy(void_ptr_t s1, void_ptr_t s2, size_t n);
void print_mem_list();

void_ptr_t mm_mem_set(void_ptr_t src, int c, size_t n)
{
    byte_t* tmp_src = (byte_t*)src;

    int i;
    for (i = 0; i != n; tmp_src[i++] = c)
        ;

    return src;
}

void_ptr_t mm_mem_copy(void_ptr_t s1, void_ptr_t s2, size_t n)
{
    uint_t *l_dest = (uint_t*)s1, *l_src = (uint_t*)s2;
    byte_t *c_dest = NULL, *c_src = NULL;

LOOP:
    // first copy over data of 4-byte chunks
    if (n >= sizeof(uint_t)) {
        *l_dest++ = *l_src++;
        n -= sizeof(uint_t);
        goto LOOP;
    }

    if (n) {
        // copy over the remainder data, 1-byte at a time
        c_dest = (byte_t*)l_dest, c_src = (byte_t*)l_src;
        for (; n; --n) {
            *c_dest++ = *c_src++;
        }
    }

    return s1;
}
