#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include "kernel/types.h"
#include "user/user.h"

#define false 0
#define true 1
#define NULL (void*) 0

struct block {
    unsigned size;  // size of block (in bytes)
    int free; // acts like a boolean telling if the block is free or not
    struct block *next;
    struct block *prev;
};
typedef struct block block;

void *_malloc(int size);

void _free(void *ptr);

#endif // MEMORY_MANAGEMENT_H
