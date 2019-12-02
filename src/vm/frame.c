#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include <stdlib.h>
#include <stdio.h>

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
      kpage = frame_evict(flags);

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
	//page dirty일 경우 구현 필요
	//
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
  struct list_elem *e;
  for (e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e))
  {
    struct frame *f = list_entry(e, struct frame, elem);
    if(f->kpage == kpage)
    {
      list_remove(e);
      printf("rrrrrrrrr");
      palloc_free_page(f);
      free(f);
      return;
    }
  }
}
