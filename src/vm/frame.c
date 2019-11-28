#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "threads/palloc.h"

void*
frame_alloc (enum palloc_flags flags, struct spte *spte)
{
  void *kpage = palloc_get_page(flags);
  if(kpage)
  {
    struct frame *f = malloc(sizeof(struct frame));
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
  struct thread *t = thread_current();
  struct list_elem *e = list_begin(&frame_table);

  while(pagedir_is_accessed(t->pagedir, list_entry(e, struct frame, elem)->spte->upage))
  {
    if(list_entry(e, struct frame, elem)->spte->pin == false)
      pagedir_set_accessed (t->pagedir, list_entry(e, struct frame, elem)->spte->upage, false);

    e = list_next(e);

    if (e == list_end(&frame_table))
      e = list_begin(&frame_table);
  }

  struct frame *f = list_entry(e, struct frame, elem);
  f->spte->type = T_SWAP;
  f->spte->swap_index = swap_out(f->kpage);
  pagedir_clear_page (t->pagedir, f->spte->upage);
  palloc_free_page(f->kpage);
  free(f);

  return palloc_get_page(flags);
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
      palloc_free_page(f);
      free(f);
      return;
    }
  }
}
