#include <assert.h>
#include <stdint.h>
#include "fifo.h"
#include "app_debug.h"

void fifo_malloc_init(fifo *cb, const char *id, size_t capacity, size_t elementSize)
{
  char *pIdIn = (char*) id;
  char *pIdOut = (char*) cb->id;
  while (*pIdIn != '\0') {
    *pIdOut = *pIdIn;
    pIdIn++;
    pIdOut++;
  }
  *pIdOut = '\0';
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

void fifo_nomalloc_init(fifo *cb, const char *id, void *buf, size_t bufSize, size_t elementSize)
{
  char *pIdIn = (char*) id;
  char *pIdOut = (char*) cb->id;
  while (*pIdIn != '\0') {
    *pIdOut = *pIdIn;
    pIdIn++;
    pIdOut++;
  }
  *pIdOut = '\0';
  
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
  if (cb->count >= cb->capacity) {
    app_debug_printf("fifo_push: %s overflow capacity:%d!\n", cb->id, cb->capacity);
    return;
  }
  /* if (cb->capacity == 256) { */
  /*   app_debug_printf("fifo_push: count: %d item:%x\n", cb->count, *((uint8_t*)(item))); */
  /* } */
                          
  memcpy(cb->head, item, cb->elementSize);
  cb->head = (char*)cb->head + cb->elementSize;
  if(cb->head == cb->bufferEnd) {
    cb->head = cb->buffer;
  }
  cb->count++;
}

void fifo_pop(fifo *cb, void *item)
{
  if (cb->count <= 0) {
    app_debug_printf("fifo_pop: %s underflow!\n", cb->id);
  }

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
