#include <stdarg.h>
#include <stdio.h>
#include "app_debug.h"

char gPrintfBuf[256];

void app_debug_printf(const char *format,...)
{
  va_list args;
  va_start(args, format);
  snprintf(gPrintfBuf, sizeof(gPrintfBuf), format, args);
  app_debug_print(gPrintfBuf);
}
