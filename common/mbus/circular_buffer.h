#ifndef FILE_MBUS_PROTO_SEEN
#define FILE_MBUS_PROTO_SEEN
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
} circular_buffer;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern void circular_buffer_malloc_init(circular_buffer *cb, size_t capacity, size_t elementSize);
  extern void circular_buffer_malloc_free(circular_buffer *cb);
  extern void circular_buffer_nomalloc_init(circular_buffer *cb, void *buf, size_t capacity, size_t elementSize);
  extern void circular_buffer_reset(circular_buffer *cb);
  extern void circular_buffer_push(circular_buffer *cb, const void *item);
  extern void circular_buffer_pop(circular_buffer *cb, void *item);
  extern bool circular_buffer_is_full(circular_buffer *cb);
  extern bool circular_buffer_is_empty(circular_buffer *cb);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_MBUS_PROTO_SEEN

