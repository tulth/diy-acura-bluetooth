#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <usb_dev.h>
#include <usb_serial.h>
#include <core_pins.h>

#define USB_STR_MAX_LEN 256

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  char usbStrBuf[USB_STR_MAX_LEN];
  int usbStrLen;
  bool pinVal, prevPinVal;
  
  usb_init();
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  PORTC_PCR2 |= PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
  GPIOC_PDDR |= 0x20;  // gpio data direction reg, for led bit

  usb_serial_write("vcd:\n", 6);
  blinkMilliSecElapsed = 0;

  prevPinVal = 0;
  while (1) {
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = 0x20;  // gpio toggle reg, for led bit
      blinkMilliSecElapsed = 0;
    }
    
    pinVal = (GPIOC_PDIR & 0x04) != 0;
    if (pinVal != prevPinVal) {
      usbStrLen = snprintf(usbStrBuf, USB_STR_MAX_LEN, "%08x %d\n", micros(), pinVal);
      usb_serial_write(usbStrBuf, usbStrLen);
      prevPinVal = pinVal;
    } 
    yield();
  }
}

// int __assert_func(const char *fn, long line)
void __assert(const char *, int, const char *)
{
  usb_serial_write("assert fail! halting!\n", 21);
  while (1) { ; }
}
