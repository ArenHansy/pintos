#include "userprog/signal.h"
#include "threads/malloc.h"
#include "threads/thread.h"

#include <stdio.h>

static struct lock sig_lock;

void signal_init(void) {
  lock_init(&sig_lock);
}

/* Send signal from current process to parent process, with content and sig type */
int send_signal(int content, enum signal_type sig_type) {
  struct thread *cur = thread_current();
  struct thread *dest_thread = cur->parent;
  struct signal *sig;
  struct list_elem *e;
  
  if(dest_thread == NULL) return -1;
  
  //printf("sending signal from %d to %d, %d, %d\n", cur->tid, dest_thread->tid, content, sig_type);
  
  lock_acquire(&sig_lock);
  
  for (e = list_begin (&dest_thread->signal_list);
       e != list_end (&dest_thread->signal_list);
       e = list_next (e)) {
    sig = list_entry (e, struct signal, elem);
    if(sig->source == cur->tid && sig->dest == dest_thread->tid && 
       sig->sig_type == sig_type) {
      if(sig->sig_status != SIG_RECV) {
        lock_release(&sig_lock);
        thread_yield();
        return -1;
      }
      sig->content = content;
      sig->sig_status = SIG_DONE;
      lock_release(&sig_lock);
      thread_yield();
      return 0;
    }
  }
  sig = (struct signal*)malloc(sizeof(struct signal));
  sig->source = cur->tid;
  sig->dest = dest_thread->tid;
  sig->content = content;
  sig->sig_type = sig_type;
  sig->sig_status = SIG_WRITE;
  list_push_back(&dest_thread->signal_list, &sig->elem);
  
  lock_release(&sig_lock);
  thread_yield();
  return 0;
}

/* Receive signal from specific source and sig_type */
int get_signal(pid_t source, enum signal_type sig_type) {
  struct thread *cur = thread_current();
  int result = -1;
  struct signal *sig;
  struct thread *t;
  struct list_elem *e;
  
  //printf("%d receiving signal from %d, %d\n", cur->tid, source, sig_type);
  
  lock_acquire(&sig_lock);
  
  for (e = list_begin (&cur->signal_list); e != list_end (&cur->signal_list);
       e = list_next (e)) {
    sig = list_entry (e, struct signal, elem);
    if(sig->source == source && sig->dest == cur->tid &&
       sig->sig_type == sig_type) {
      ASSERT(sig->sig_status == SIG_WRITE);
      result = sig->content;
      list_remove(&sig->elem);
      free(sig);
      lock_release(&sig_lock);
      thread_yield();
      return result;
    }
  }
  
  for (e = list_begin (&cur->child_list); e != list_end (&cur->child_list); 
       e = list_next(e))
  { 
    t = list_entry (e, struct thread, p_elem);
    if(t->tid == source) {
      sig = (struct signal*)malloc(sizeof(struct signal));
      sig->source = source;
      sig->dest = cur->tid;
      sig->content = 0;
      sig->sig_type = sig_type;
      sig->sig_status = SIG_RECV;
      list_push_back(&cur->signal_list, &sig->elem);
      
      lock_release(&sig_lock);
      while(sig->sig_status != SIG_DONE) {
        thread_yield();
      }
      lock_acquire(&sig_lock);
      
      result = sig->content;
      list_remove(&sig->elem);
      free(sig);
      break;
    }
  }
  
  lock_release(&sig_lock);
  thread_yield();
  return result;
}
