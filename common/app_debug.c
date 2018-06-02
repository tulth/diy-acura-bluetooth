#include <stdarg.h>
#include <stdio.h>
#include "app_debug.h"
#ifdef EMBEDDED
#include "usb_serial.h"
#endif


void app_debug_putchar(const char arg)
{
#ifdef EMBEDDED  
  usb_serial_putchar(arg);
#else // NOT EMBEDDED
  printf("%c", arg);
#endif
}

void app_debug_print(const char *arg)
{
  while (*arg != '\0') {
    app_debug_putchar(*arg);
    arg++;
  }
}

char gPrintfBuf[256];

void app_debug_printf(const char *format,...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(gPrintfBuf, sizeof(gPrintfBuf), format, args);
  va_end(args); 
  app_debug_print(gPrintfBuf);
}
