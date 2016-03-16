#include <stdarg.h>
#include <stdio.h>
#include "app_debug.h"

char gPrintfBuf[256];

void app_debug_printf(const char *format,...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(gPrintfBuf, sizeof(gPrintfBuf), format, args);
  va_end(args); 
  app_debug_print(gPrintfBuf);
}
