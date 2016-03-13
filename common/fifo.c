#include "fifo.h"
#include <assert.h>

void fifo_malloc_init(fifo *cb, size_t capacity, size_t elementSize)
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

void fifo_malloc_free(fifo *cb)
{
    free(cb->buffer);
}

void fifo_nomalloc_init(fifo *cb, void *buf, size_t bufSize, size_t elementSize)
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

void fifo_reset(fifo *cb)
{
    cb->count = 0;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}


void fifo_push(fifo *cb, const void *item)
{
  assert(cb->count < cb->capacity);
  memcpy(cb->head, item, cb->elementSize);
  cb->head = (char*)cb->head + cb->elementSize;
  if(cb->head == cb->bufferEnd) {
    cb->head = cb->buffer;
  }
  cb->count++;
}

void fifo_pop(fifo *cb, void *item)
{
  assert(cb->count > 0);
  memcpy(item, cb->tail, cb->elementSize);
  cb->tail = (char*)cb->tail + cb->elementSize;
  if(cb->tail == cb->bufferEnd) {
    cb->tail = cb->buffer;
  }
  cb->count--;
}

bool fifo_is_full(fifo *cb)
{
  return cb->count == cb->capacity;
}

bool fifo_is_empty(fifo *cb)
{
  return cb->count == 0;
}
