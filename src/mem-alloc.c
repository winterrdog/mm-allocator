#include "./utils.h"

// NOTE:
// this program is built with x86-64 in mind. for x86, it might not b as
// efficient as x86-64. It's also not so thread-safe

// global singly linked-list trackers
header_info_t *sl_head = NULL, *sl_tail = NULL;

// global mutex - forget spinlock on this 1
pthread_mutex_t global_malloc_lock = PTHREAD_MUTEX_INITIALIZER;

void *malloc(size_t size)
{
	if (!size)
		return NULL;

	void *block = NULL;
	size_t real_size = 0;
	header_info_t *header = NULL;
	pthread_mutex_lock(&global_malloc_lock);

	// is there some already allocate memory?
	header = get_free_block(size);
	if (header) {
		header->is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);

		// return the real data address. it's just after the header
		return (void *)(header + 1);
	}
	// if there was no allocated memory already, allocate some with 'sbrk'
	real_size = size + sizeof(header_info_t);
	block = sbrk(real_size);
	if (block == (void *)-1) {
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}
	// update our global memory linked list
	header = block;
	header->is_free = 0, header->size = size, header->next = NULL;

	if (!sl_head)
		sl_head = header;
	if (sl_tail)
		sl_tail->next = header;

	sl_tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
	return (void *)(header + 1);
}

void *calloc(size_t num, size_t nsize)
{
	if (num <= 0 || nsize <= 0)
		return NULL;

	void *block = NULL;
	size_t size = num * nsize;

	// check multiplication overflow
	if (size % nsize != 0)
		return NULL;

	block = malloc(size);
	if (!block)
		return NULL;

	mm_mem_set(block, 0, size);
	return block;
}

void *realloc(void *block, size_t size)
{
	if (!block || !size)
		return malloc(size);

	void *ret = NULL;
	header_info_t *header = (header_info_t *) block - 1;
	if (header->size >= size)
		return block;

	ret = malloc(size);
	if (ret) {
		mm_mem_copy(ret, block, header->size);
		free(block);
	}
	return ret;
}

void free(void *block)
{
	if (!block)
		return;

	void *program_break = NULL;
	header_info_t *header = NULL, *tmp = NULL;

	pthread_mutex_lock(&global_malloc_lock);
	header = (header_info_t *) block - 1;

	program_break = sbrk(0);	// currrent heap ceiling

	// is the block the last one in the heap
	if (program_break == ((byte_t *) block + header->size)) {
		// update linked list
		if (sl_head == sl_tail) {
			sl_head = sl_tail = NULL;
		} else {
			for (tmp = sl_head; tmp; tmp = tmp->next) {
				if (tmp->next == sl_tail) {
					// if we hit the end of list
					tmp->next = NULL, sl_tail = tmp;
				}
			}
		}
		sbrk(0 - (header->size + sizeof(header_info_t)));	// release memory
		goto clean_up;
	}
	// if the block is "not" the last one in the heap
	header->is_free = 1;	// mark 'free'

 clean_up:
	pthread_mutex_unlock(&global_malloc_lock);
	return;
}

header_info_t *get_free_block(size_t size)
{
	for (header_info_t * curr = sl_head; curr; curr = curr->next) {
		if (curr->is_free && curr->size >= size)
			return curr;
	}
	return NULL;
}
