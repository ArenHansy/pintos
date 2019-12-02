#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <bitmap.h>
#include "devices/block.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

struct lock swap_lock;

struct block *swap_block;
struct bitmap *swap_table;

void init_swap_table(void);
size_t swap_out (void *kpage);
void swap_in (int swap_index, void *kpage);

#endif

