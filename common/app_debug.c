#include <stdarg.h>
#include <stdio.h>
#include "app_debug.h"
#ifdef EMBEDDED
#include "usb_serial.h"
#endif


void app_debug_print(const char *arg)
{
#ifdef EMBEDDED  
  while (*arg != '\0') {
    usb_serial_putchar(*arg);
    arg++;
  }
  usb_serial_putchar('\n');
#else // NOT EMBEDDED
  printf("%s\n", arg);
#endif
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
