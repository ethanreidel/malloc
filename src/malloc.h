#ifndef NEW_MALLOC_H
#define NEW_MALLOC_H

#include <stddef.h>

#define MAX_MALLOC_SIZE (1000)

struct mem_block {
    size_t size;
    struct mem_block *prev;
    struct mem_block *next;
    char* buffer;
    int free;
};

extern struct mem_block* free_list_head;
extern size_t total_allocated;

void print_mem_block(struct mem_block* block);
void init_malloc();
struct mem_block *find_free_block(struct mem_block **last, size_t size);
void remake_free_list();
void print_free();
size_t round_to_nearest_eighth(size_t x);
void insert_block(struct mem_block* block);
void merge_free_blocks(struct mem_block* free_block);
void* new_malloc(size_t malloc_size);
void new_free(void* buffer);

#endif
