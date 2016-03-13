#ifndef FILE_FIFO_SEEN
#define FILE_FIFO_SEEN
#include <stdbool.h>
#include <stddef.h> // for size_t
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy

typedef struct
{
    void *buffer;       // data buffer
    void *bufferEnd;   // end of data buffer
    size_t capacity;    // maximum number of items in the buffer
    size_t count;       // number of items in the buffer
    size_t elementSize; // size of each item in the buffer
    void *head;         // pointer to head
    void *tail;         // pointer to tail
} fifo;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern void fifo_malloc_init(fifo *cb, size_t capacity, size_t elementSize);
  extern void fifo_malloc_free(fifo *cb);
  extern void fifo_nomalloc_init(fifo *cb, void *buf, size_t bufSize, size_t elementSize);
  extern void fifo_reset(fifo *cb);
  extern void fifo_push(fifo *cb, const void *item);
  extern void fifo_pop(fifo *cb, void *item);
  extern bool fifo_is_full(fifo *cb);
  extern bool fifo_is_empty(fifo *cb);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_FIFO_SEEN

