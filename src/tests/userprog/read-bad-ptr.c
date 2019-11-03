/* Passes an invalid pointer to the read system call.
   The process must be terminated with -1 exit code. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  int handle;
  printf("ggggggggggggggggggggggggggggg");
  CHECK ((handle = open ("sample.txt")) > 1, "open \"sample.txt\"");
  printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  read (handle, (char *) 0xc0100000, 123);
  printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  fail ("should not have survived read()");
}
