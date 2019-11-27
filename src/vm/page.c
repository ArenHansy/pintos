#include <hash.h>


static unsigned
hash_func (const struct hash_elem *elem, void *aux UNUSED)
{
  struct spte *spte = hash_entry(elem, struct spte, hash_elem);
  return hash_int((int) spte->upage);
}

static bool
less_func (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  struct spte *sa = hash_entry (a, struct spte, hash_elem);
  struct spte *sb = hash_entry (b, struct spte, hash_elem);

  if(sa->upage < sb->upage)
    return true;
  return false;
}

bool 
load_frame()
{

}

bool
load_swap()
{

}

bool
load_filesys()
{

}
