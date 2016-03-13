#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>

#define MSG_STR_SIZE 256

#define PINBIT_LED (1<<5)
#define PINBIT_MBUS_SENSE (1<<2)

#define USBSERIAL Serial

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  bool pinVal, prevPinVal;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */

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
