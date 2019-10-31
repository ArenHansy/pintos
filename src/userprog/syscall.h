#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <lib/kernel/list.h>
#include "user/syscall.h"

void syscall_init (void);

struct process_info {
    pid_t pid;
    int status;
    bool wait;
    bool exit;
    struct list_elem elem;
};

struct process_info* add_child_process(pid_t);
struct process_info* get_child_process(pid_t);
void remove_child_process_by_pid(pid_t);
void remove_child_process_by_info(struct process_info*);
void remove_all_child_process();

#endif /* userprog/syscall.h */
