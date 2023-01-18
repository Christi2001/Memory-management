#include "memory_management.h"

block *heap_bottom = NULL;

// Function for allocating memory for blocks surrounded by occupied blocks
block* find_surrounded_free_block(block* current, int size) {
  block *b = current->next;
  int bytes_from_end = current->size - size;
  while(b != NULL) {
    bytes_from_end += b->size + sizeof(block);
    b = b->next;
  }
  sbrk(-bytes_from_end);
  // (reusing block b instead of creating a new one)
  b = (block*) sbrk(0);
  sbrk(bytes_from_end);
  return b;
}

// Function for finding first available block
block *find_first_free_block(block **last, int size) {
  block *current = heap_bottom;

  while(current != NULL && !(current->free == true && current->size >= size)) {
    *last = current;
    current = current->next;
  }

  if(current == NULL || current->size - size < sizeof(block) + sizeof(int))
    return current;
  
  block* unused_space = find_surrounded_free_block(current, size);
  unused_space->size = current->size - size - sizeof(block);
  current->next->prev = unused_space;
  unused_space->free = true;
  unused_space->prev = current;
  unused_space->next = current->next;

  current->size -= unused_space->size + sizeof(block);
  current->next = unused_space;

  return current;
}

// Function for adding a new block of data in the heap
block *add_new_block(block *last, int size) {
  block *new = (block*) sbrk(0);

  void *request = sbrk(size + sizeof(block));
  if(request == (void*) - 1) // sbrk failed.
    return NULL;

  if(last != NULL)
    last->next = new;

  new->size = size;
  new->free = false;
  new->next = NULL;
  new->prev = last;

  return new;
}

void *_malloc(int size) {
  block *current;

  if (size <= 0)
    return NULL;

  if (heap_bottom == NULL) {
    current = add_new_block(NULL, size);
    if (current == NULL)
      return NULL;
        
    heap_bottom = current;
    current++;
    return current;
  }
    
  block *last = heap_bottom;
  current = find_first_free_block(&last, size);

  if (current == NULL) {
    current = add_new_block(last, size);
    if (current == NULL)
      return 0;
  } else
      current->free = false;

  current++;
  return current;
}

void _free(void *pointer) {
  if (pointer == NULL)
    return;

  block* block_pointer = pointer - sizeof(block);
  block_pointer->free = true;

  // if prev block is free, merge it with the current block
  if (block_pointer->prev->free == true) {
    block_pointer->prev->size += block_pointer->size + sizeof(block);
    block_pointer->next->prev = block_pointer->prev;
    block_pointer->prev->next = block_pointer->next;
    block_pointer = block_pointer->prev;
  }

  // if next block is free, merge it with the current block
  if (block_pointer->next->free == true) {
    block_pointer->size += block_pointer->next->size + sizeof(block);
    block_pointer->next->next->prev = block_pointer;
    block_pointer->next = block_pointer->next->next;
  }

  // Check if the given block is the last block in the list
  if (block_pointer->next == NULL) {
    sbrk( -(block_pointer->size + sizeof(block)));
    // Check if it is also the first
    if (block_pointer->prev == NULL) {
      heap_bottom = NULL;
      return;
    }
    else
      block_pointer->prev->next = NULL;
  }
}
