#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include <stdlib.h>
#include <stdio.h>
#include "threads/malloc.h"
#include "userprog/syscall.h"

void*
frame_alloc (enum palloc_flags flags, struct spte *spte)
{
  if((flags & PAL_USER) == 0)
    return NULL;
  void *kpage = palloc_get_page(flags);
  if(kpage)
  {
    struct frame *f = malloc(sizeof(struct frame));
    if(!f)
      return NULL;
    f->kpage = kpage;
    f->thread = thread_current();
    f->spte = spte;
    list_push_back(&frame_table, &f->elem);
  }
  else
  {
    while (!kpage)
    {
      kpage = frame_evict(flags);
      lock_release(&ft_lock);
    }
    struct frame *f = malloc(sizeof(struct frame));
    if(!f)
      return NULL;
    f->kpage = kpage;
    f->spte = spte;
    f->thread = thread_current();
    list_push_back(&frame_table, &f->elem);
  }
  return kpage;
}

struct frame*
frame_evict (enum palloc_flags flags)
{
  lock_acquire(&ft_lock);
  struct list_elem *e = list_begin(&frame_table);
  
  while(true)
  {
    struct frame *f = list_entry(e, struct frame, elem);
    if(f->spte->pin == false)
    {
      struct thread *t = f->thread;
      if(pagedir_is_accessed (t->pagedir, f->spte->upage))
        pagedir_set_accessed(t->pagedir, f->spte->upage, false);

      else
      {
	if(f->spte->type == T_SWAP)
	{	  
 	  f->spte->swap_index = swap_out(f->kpage);
	}
	else if(f->spte->type == T_FRAME)
	{
	  f->spte->type = T_SWAP;	
	  f->spte->swap_index = swap_out(f->kpage);
	}
	else if(f->spte->type == T_FILESYS)
	{
          if(pagedir_is_dirty(t->pagedir,f->spte->upage))
	  {
	    lock_acquire(&file_lock);
	    file_write_at(f->spte->file, f->kpage, f->spte->read_bytes, f->spte->offset);
	    lock_release(&file_lock);
	  }
	}
	f->spte->loaded = false;
	list_remove(&f->elem);
	pagedir_clear_page(t->pagedir, f->spte->upage);
	palloc_free_page(f->kpage);
   	free(f);
	return palloc_get_page(flags);
      }
    }
    e = list_next(e);
    if (e == list_end(&frame_table))
      e = list_begin(&frame_table);
  }
}

void
frame_free(void *kpage)
{
  lock_acquire(&ft_lock);
  struct list_elem *e;
  for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e))
  {
    struct frame *f = list_entry(e, struct frame, elem);
    if(f->kpage == kpage)
    {
      list_remove(e);
      palloc_free_page(f->kpage);
      free(f);
      break;
    }
  }
  lock_release(&ft_lock);
}
