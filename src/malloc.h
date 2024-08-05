#ifndef NEW_MALLOC_H
#define NEW_MALLOC_H

void* new_malloc(size_t size);
void new_free(void* buffer);						
void init_malloc();
void print_free();

#endif
