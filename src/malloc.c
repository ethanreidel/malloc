#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include "malloc.h"
#define MAX_MALLOC_SIZE (1000)


static unsigned char mem_buffer[MAX_MALLOC_SIZE]; //create buffer of 1000 bytes (1 char burger 1 byte)
//one burger in one byte yummy


struct mem_block {
	int size; //size of memory to be allocated
	struct mem_block *prev; //points to previous mem block
	struct mem_block *next; //points to next mem block
	char* buffer; //points to start of this memory block
	int free;
};

struct mem_block* free_list_head; //create list for
size_t total_allocated = 0;

void print_mem_block(struct mem_block* block) {
	printf("block: %p\n", block);
	printf("\tsize: %d\n", block->size);
	printf("\tprev: %p\n",(void*)block->prev);
	printf("\tnext: %p\n", (void*)block->next);
	printf("\tbuffer: %p\n", (void*)block->buffer);
	printf("\tfree: %d\n", block->free);
}

void init_malloc() {
	//this function initializes the mem_block struct for the whole 0->max_malloc_size buffer
	//char is used here because it is equal to 1 byte
	

	//casts the buffer to a pointer to the struct mem_block
	free_list_head = (struct mem_block *)mem_buffer;

	//since this is just initialization, we don't have a next or prev (should be dealt with in new_malloc)
	free_list_head->next = NULL;
	free_list_head->prev = NULL;
	free_list_head->free = 1;

	printf("sizeof membuffer: %ld", sizeof(mem_buffer));
	//removes the space that is taken by the size of the record (32 bytes in theory)
	free_list_head->size = sizeof(mem_buffer) - sizeof(struct mem_block); 
	
	//buffer points to memory spot exaclty after the record in this case
	//mem_buffer == 0th pos of 1000 byte buffer + offset of 32 bytes to get next position
	//printf("initial memory position before offset: %p\n", (char*)(mem_buffer));
	free_list_head->buffer = (char*)(mem_buffer + sizeof(struct mem_block));
	//print_mem_block(free_list_head);
	//after initialization memory block should look like this
	//[32 b	with buffer pointing to to the 33rd byte| 968 byte free memory						]
	//printf("free list size: %d\n", free_list_head->size);
}

struct mem_block *find_free_block(struct mem_block **last, size_t size) {
	struct mem_block* curr = free_list_head;
	while (curr && !(curr->free && curr->size >= size+sizeof(struct mem_block))) {
		*last = curr;
		curr = curr->next;
	}
	return curr;
	//TODO add functionality here

}


void print_free() {
	struct mem_block* curr = free_list_head;
	//print_mem_block(curr);
	//printf("curr location: %d\n", curr->size);
	int c = 0;
	while (curr) {
		if (curr->free == 1) {
			print_mem_block(curr);
			c+=1;
		}
		curr = curr->next;
	}
	printf("total free memory in %d free blocks: %ld bytes\n", c, MAX_MALLOC_SIZE - total_allocated);

}

size_t round_to_nearest_eighth(size_t x) {
	return ((x + 7) & (-8));


}


void add_to_free_list(struct mem_block* block) {
	block->next = free_list_head;
	block->prev = NULL;
	if (free_list_head != NULL) {
		free_list_head->prev = block;
	}

	//free_list_head->prev = block;
	free_list_head = block;
}

void* new_malloc(size_t malloc_size) { //void* so we can cast malloc to specific type e.g. (char*)new_malloc(104)
	//size is in bytes so new_malloc(10) points to the beginning of a 10 byte location in mem_buffer
	//create mem_block/record at beginning of each malloc call
	//total memory required = size + sizeof(mem_block) (usually 32 bytes)
	//
	//
	struct mem_block* last = NULL;	
		
//
	size_t size = round_to_nearest_eighth(malloc_size);	
	if (size != malloc_size) {
		printf("malloc amount not multiple of 8, rounding %ld to nearest 8th: %ld\n", malloc_size, size);
	}
	
	size_t remaining_memory = MAX_MALLOC_SIZE - total_allocated;
	if (size + sizeof(struct mem_block) > remaining_memory) {
		printf("blah blah blah u stupid malloc\n");
		exit(0);
	}


	struct mem_block* free_space = find_free_block(&last, malloc_size);
	printf("free space pointer: %p\n", free_space);
	
	
	//creates record after previous 32 block record.
	//does this by allocating a mem block at (buffer + size) location
	//struct mem_block* malloc_struct = (struct mem_block*)(free_list_head->buffer+size);
	/

	struct mem_block* malloc_struct = (struct mem_block*)(free_space->buffer+size);

	//make malloc_struct part of free_list
	
	malloc_struct->size = free_space->size-size-sizeof(struct mem_block);
	//malloc_struct->size = remaining_memory-size-sizeof(struct mem_block);
	
	malloc_struct->buffer = (char*)(malloc_struct+1);
	printf("free block buffer: %p\n", malloc_struct->buffer);

	malloc_struct->free = 1;

	//temp work here
	//
	




	//below is previous work above 
	//add new record to free list

	malloc_struct->prev = free_list_head;
	malloc_struct->next = NULL;
	
	free_list_head->next = malloc_struct;	
	free_list_head->size = size;

	void* return_buffer = (void*)free_space->buffer;
	free_list_head = malloc_struct;


	total_allocated += size + sizeof(struct mem_block);

	return return_buffer;
	
}


void* merge_free_blocks(struct mem_block* free_block) {
	
	if (free_block->next && free_block->next->free == 1) {
		free_block->size += sizeof(struct mem_block) + free_block->size;
		free_block->next = free_block->next->next;
	}

	if (free_block->next != NULL) {
		free_block->next->prev = free_block;
	}
	return free_block;


}

void insert_block(struct mem_block* block) {
	struct mem_block* current = free_list_head;
	while (current != NULL && current < block) {
		current = current->next;
	}
	block->next = current;
	block->prev = current->prev;

	if (current->prev != NULL) {
		current->prev->next = block;
	}
	if (current != NULL) {
		current->prev = block;
	}

}


void new_free(void* buffer) { //free buffer created from new_malloc
	

	if (buffer == NULL) {
		printf("nothing to free.\n");
		return;
	}


	//points to block of buffer passed in from function
	struct mem_block* buffer_block = (struct mem_block*)((char *)buffer - sizeof(struct mem_block));

	
	total_allocated-= buffer_block->size + sizeof(struct mem_block);

	buffer_block->free = 1;	
	//if buffer block is further back than the head, we just do normal insertion.
	if (buffer_block < free_list_head) {
		
		buffer_block->next = free_list_head;
		free_list_head->prev = buffer_block;
		free_list_head = buffer_block;
	} else {
		//else we need to insert block into middle of free list
		//
		insert_block(buffer_block);
	}

		
	//add_to_free_list(buffer_block);








	



}
int main(){ 
	
	init_malloc();

	char* arr = (char*)new_malloc(64);
	char* arr1 = (char*)new_malloc(32);
	char* arr2 = (char*)new_malloc(128);
	char* arr3 = (char*)new_malloc(256);
	
	new_free(arr);
	new_free(arr1);
	new_free(arr3);
	new_free(arr2);
	print_free();	

	return 0;
}



