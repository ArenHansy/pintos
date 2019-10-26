#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

struct argv_elem
{
    char *value;
    uint32_t address;
    struct list_elem elem;
};

static const int WORD_SIZE = 4;

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
