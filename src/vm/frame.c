#include "vm/frame.h"
#include "vm/page.h"


void*
frame_alloc (enum palloc_flags flags, struct spte *spte)
{
  void *kpage = palloc_get_page(flags);
  if(kapge)
  {
    struct frame *f = malloc(sizeof(struct frame));
    f->kapge = kpage;
    f->thread = thread_current();
    f->spte = spte;
    list_push_back(&frame_table, &f->elem);
  }
  else
  {
    //eviction
  }
  return kpage;
}

struct frame*
frame_evict ()
{
  struct list_elem *e;
  for(e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e))
  {
    struct frame *f = list_entry(e, struct frame, elem);
   // Implementing
  }
}

void
frame_free(void *kapge)
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
