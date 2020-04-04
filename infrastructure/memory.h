#ifndef MEMORY_FREE_H
#define MEMORY_FREE_H

// MemoryFree library based on code posted here:
// https://forum.arduino.cc/index.php?topic=27536.msg204024#msg204024
// Extended by Matthew Murdoch to include walking of the free list.

#ifdef __cplusplus
extern "C" {
#endif

int freeMemory();
static inline size_t stack_size();

#ifdef  __cplusplus
}
#endif

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

extern unsigned int __heap_start;
extern void *__brkval;

/*
 * The free list structure as maintained by the
 * avr-libc memory allocation routines.
 */
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;


/* Calculates the size of the free list */
int freeListSize() {
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  return total;
}

int freeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}


static inline size_t stack_size()
{
    return RAMEND - SP;
}
#endif // MEMORY_FREE_H
