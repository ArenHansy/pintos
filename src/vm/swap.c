#include "vm/swap.h"
#include "devices/block.h"
#include "threads/vaddr.h"

void
init_swap_table ()
{
  swap_block = block_get_role (BLOCK_SWAP);
  if (!swap_block)
    return;
  swap_table = bitmap_create (block_size(swap_block) / (PGSIZE/BLOCK_SECTOR_SIZE));
  if (!swap_table)
    return;
  bitmap_set_all(swap_table, true);
}

size_t swap_out (void *kpage)
{
  lock_acquire(&swap_lock);
  size_t swap_index = bitmap_scan_and_flip(swap_table, 0, 1, true);
  
  size_t i;
  for (i = 0; i < (PGSIZE/BLOCK_SECTOR_SIZE); i++)
  {
    block_write(swap_block, swap_index*(PGSIZE/BLOCK_SECTOR_SIZE) + i, kpage + (BLOCK_SECTOR_SIZE*i));
  }
  lock_release(&swap_lock);
  return swap_index;
}

void swap_in (int swap_index, void *kpage)
{
   lock_acquire(&swap_lock);
   size_t i;
   for (i = 0; i < (PGSIZE/BLOCK_SECTOR_SIZE); i++)
   {
     block_read(swap_block, swap_index*(PGSIZE/BLOCK_SECTOR_SIZE) + i, kpage + (BLOCK_SECTOR_SIZE*i));
   }
   bitmap_set(swap_table, swap_index, true);
   lock_release(&swap_lock);
}


