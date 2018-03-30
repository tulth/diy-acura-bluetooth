#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>

#define MSG_STR_SIZE 256

#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_MBUS_SENSE_LO (1<<2)
#define PINBIT_MBUS_DRIVE_LO (1<<1)
#define PINBIT_ACC_SENSE_LO (1<<5)
#define PINBIT_HOLD_POWER_ON (1<<6)
#define USBSERIAL Serial

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  bool mbusPinVal, prevMbusPinVal;
  bool accPinVal, prevAccPinVal;
  char inChar;

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE LO */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */
  PORTD_PCR5 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* ACC SENSE LO */
  PORTD_PCR6 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* HOLD POWER ON */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, output */
  GPIOC_PDDR |= PINBIT_MBUS_DRIVE_LO;  /* gpio data direction reg, output */
  GPIOD_PDDR |= PINBIT_HOLD_POWER_ON;  /* gpio data direction reg, output */
  
  GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive mbus low */
  GPIOD_PCOR = PINBIT_HOLD_POWER_ON;  /* don't hold power on */
  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  USBSERIAL.begin(115200);
  blinkMilliSecElapsed = 0;
  prevMbusPinVal = 0;
  prevAccPinVal = 0;
  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
      // GPIOC_PTOR = PINBIT_MBUS_DRIVE_LO;
    }

    if (USBSERIAL.available()) {
      inChar = USBSERIAL.read();
      if (inChar == '0') {
        GPIOC_PSOR = PINBIT_MBUS_DRIVE_LO;  /* drive mbus low, so we drive 1 out; inverted by the drive transistor */
      }
      if (inChar == '1') {
        GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive mbus low, so we drive 0 out; disabling drive transistor */
      }
      if (inChar == 'h') {
        GPIOD_PSOR = PINBIT_HOLD_POWER_ON;
      }
      if (inChar == 'r') {
        GPIOD_PCOR = PINBIT_HOLD_POWER_ON;
      }
    }

    mbusPinVal = (GPIOC_PDIR & PINBIT_MBUS_SENSE_LO) != 0;
    if (mbusPinVal != prevMbusPinVal) {
      if (mbusPinVal) {
        USBSERIAL.println("m1");
      }  else {
        USBSERIAL.println("m0");
      }
      prevMbusPinVal = mbusPinVal;
    }
    
    accPinVal = (GPIOD_PDIR & PINBIT_ACC_SENSE_LO) != 0;
    if (accPinVal != prevAccPinVal) {
      if (accPinVal) {
        USBSERIAL.println("a1");
      }  else {
        USBSERIAL.println("a0");
      }
      prevAccPinVal = accPinVal;
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
