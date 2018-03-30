#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>
#include "app_debug.h"

#define MSG_STR_SIZE 256

#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_MBUS_SENSE (1<<2)
#define PINBIT_MBUS_DRIVE_LO (1<<1)

#define USBSERIAL Serial

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  bool pinVal, prevPinVal;
  bool driveMbusPinLo = false;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */
  GPIOC_PDDR |= PINBIT_MBUS_DRIVE_LO;  /* gpio data direction reg, for driveMbusPinLo */
  GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive low */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  USBSERIAL.begin(115200);
  blinkMilliSecElapsed = 0;
  prevPinVal = 0;
  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
    }

    if (USBSERIAL.available()) {
      driveMbusPinLo = (USBSERIAL.read() == '0');  /* caution! note the invert! */
      if (driveMbusPinLo) {
        GPIOC_PSOR = PINBIT_MBUS_DRIVE_LO;  /* drive low, so we drive 1 out; inverted by the drive transistor */
      } else {
        GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't pull low */
      }
    }

    pinVal = (GPIOC_PDIR & PINBIT_MBUS_SENSE) != 0;
    if (pinVal != prevPinVal) {
      msgStrLen = snprintf(msgStr, MSG_STR_SIZE, "%08x %d", micros(), pinVal);
      USBSERIAL.println(msgStr);
      prevPinVal = pinVal;
    }
    
    yield();
  }
}

/* int __assert_func(const char *fn, long line) */
void __assert(const char *, int, const char *)
{
  USBSERIAL.println("assert fail! halting!");
  while (1) { ; }
}
