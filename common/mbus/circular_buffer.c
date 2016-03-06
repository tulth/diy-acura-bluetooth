#include "circular_buffer.h"
#include <assert.h>

void circular_buffer_malloc_init(circular_buffer *cb, size_t capacity, size_t elementSize)
{
    cb->buffer = malloc(capacity * elementSize);
    assert(cb->buffer != NULL);
    cb->bufferEnd = (char *)cb->buffer + capacity * elementSize;
    cb->capacity = capacity;
    cb->count = 0;
    cb->elementSize = elementSize;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

void circular_buffer_malloc_free(circular_buffer *cb)
{
    free(cb->buffer);
}

void circular_buffer_nomalloc_init(circular_buffer *cb, void *buf, size_t bufSize, size_t elementSize)
{
    cb->buffer = buf;
    assert(cb->buffer != NULL);
    cb->capacity = bufSize / elementSize;
    cb->bufferEnd = (char *)cb->buffer + cb->capacity * elementSize;
    cb->count = 0;
    cb->elementSize = elementSize;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

void circular_buffer_reset(circular_buffer *cb)
{
    cb->count = 0;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}


void circular_buffer_push(circular_buffer *cb, const void *item)
{
  assert(cb->count < cb->capacity);
  memcpy(cb->head, item, cb->elementSize);
  cb->head = (char*)cb->head + cb->elementSize;
  if(cb->head == cb->bufferEnd) {
    cb->head = cb->buffer;
  }
  cb->count++;
}

void circular_buffer_pop(circular_buffer *cb, void *item)
{
  assert(cb->count > 0);
  memcpy(item, cb->tail, cb->elementSize);
  cb->tail = (char*)cb->tail + cb->elementSize;
  if(cb->tail == cb->bufferEnd) {
    cb->tail = cb->buffer;
  }
  cb->count--;
}

bool circular_buffer_is_full(circular_buffer *cb)
{
  return cb->count == cb->capacity;
}

bool circular_buffer_is_empty(circular_buffer *cb)
{
  return cb->count == 0;
}
