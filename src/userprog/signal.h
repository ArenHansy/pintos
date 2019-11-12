#ifndef USERPROG_SIGNAL_H
#define USERPROG_SIGNAL_H

#include "threads/synch.h"
#include <list.h>

typedef int pid_t;

enum signal_type {
  SIG_WAIT,
  SIG_EXEC
};

enum signal_status {
  SIG_RECV,
  SIG_WRITE,
  SIG_DONE
};

struct signal {
  pid_t source; // source pid
  pid_t dest; // destination pid
  int content; // content of signal
  enum signal_type sig_type;
  enum signal_status sig_status;
  struct list_elem elem;
};

void signal_init(void);
int send_signal(int content, enum signal_type sig_type);
int get_signal(pid_t source, enum signal_type sig_type);

#endif /* userprog/signal.h */
