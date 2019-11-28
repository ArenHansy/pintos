#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "vm/frame.h"

#define T_FRAME 1
#define T_SWAP 2
#define T_FILESYS 3

struct spte {
  int type;
  void *upage;
  struct frame *frame;
  bool pin;
  bool writable;

  struct file *file;
  size_t offset;
  size_t read_bytes;
  size_t zero_bytes;

  int swap_index;
  
  struct hash_elem hash_elem;
};

unsigned hash_func(const struct hash_elem *elem, void *aux);
bool less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux);
struct spte* get_spte(void *upage);
bool load_frame(struct spte *spte);
bool load_swap(struct spte *spte);
bool load_filesys(struct spte *spte);
bool grow_stack(void *upage);


#endif

