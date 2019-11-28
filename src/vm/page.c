#include <hash.h>
#include "vm/page.h"
#include "vm/swap.h"
#include "vm/frame.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "threads/vaddr.h"

unsigned
hash_func (const struct hash_elem *elem, void *aux)
{
  struct spte *spte = hash_entry(elem, struct spte, hash_elem);
  return hash_int((int) spte->upage);
}

bool
less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
  struct spte *sa = hash_entry (a, struct spte, hash_elem);
  struct spte *sb = hash_entry (b, struct spte, hash_elem);

  if(sa->upage < sb->upage)
    return true;
  return false;
}

struct spte*
get_spte(void *upage)
{
  struct spte *spte_tmp;
  struct thread *t = thread_current();
  spte_tmp->upage = pg_round_down(upage);
  struct hash_elem *e = hash_find(&t->spt, &spte_tmp->hash_elem);
  if(!e)
    return NULL;
  return hash_entry(e, struct spte, hash_elem);
}


bool 
load_frame(struct spte *spte)
{
  void *kpage = frame_alloc(PAL_USER | PAL_ZERO, spte);
  if(!kpage)
    return false;
  if(!install_page(spte->upage, kpage, spte->writable))
  {
    frame_free(kpage);
    return false;
  }
  return true;
}

bool
load_swap(struct spte *spte)
{
  void *kpage = frame_alloc(PAL_USER, spte);
  if(!kpage)
    return false;
  if(!install_page(spte->upage, kpage, spte->writable))
  {
    frame_free(kpage);
    return false;
  }
  swap_in(spte->swap_index, spte->upage);
  return true;
}

bool
load_filesys(struct spte *spte)
{
  void *kpage = frame_alloc(PAL_USER, spte);
  file_seek(spte->file, spte->offset);
  if((int)spte->read_bytes != file_read(spte->file, kpage, spte->read_bytes))
  {
    frame_free(kpage);
    return false;
  }
  memset(kpage + (spte->read_bytes), 0, spte->zero_bytes);
  if(install_page(spte->upage, kpage, spte->writable))
  {
    frame_free(kpage);
    return false;
  }
  return true;
}

bool
grow_stack(void *upage)
{
  if(PHYS_BASE - pg_round_down(upage) > 0x800000)
    return false;

  struct spte *spte = malloc(sizeof(struct spte));

  if(!spte)
  {
    free(spte);
    return false;
  } 

  spte->upage = pg_round_down(upage);
  spte->writable = true;
  spte->pin = true;

  void *kpage = frame_alloc(PAL_USER, spte);
  if(!kpage)
  {
    free(spte);
    return false;
  }
  
  if(!install_page(spte->upage, kpage, spte->writable))
  {
    free(spte);
    frame_free(kpage);
    return false;
  }

  if (intr_context())
    spte->pin = false;
  
  struct hash_elem *e = hash_insert(&thread_current()->spt, &spte->hash_elem);

  if(!e)
    return true;
  return false;
}

