#include <stdint.h>
#include <WProgram.h>

#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_RN52_CMDLO CORE_PIN4_BITMASK
#define PINBIT_RN52_CONNSTAT_EVENT CORE_PIN5_BITMASK
#define PINBIT_RN52_RX CORE_PIN0_BITMASK
#define PINBIT_CTS CORE_PIN2_BITMASK

#define USBSERIAL Serial

#define MSG_STR_SIZE 256

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  int incomingByte;
  bool pinVal;
  bool prevPinVal = false;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;

  PORTA_PCR13 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* CMD LO */
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* state change bit */

  GPIOA_PDDR |= PINBIT_RN52_CMDLO;  /* gpio data direction reg, for cmd lo */
  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  PORTD_PCR0 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* CTS */
  GPIOD_PDDR |= PINBIT_CTS;  /* gpio data direction reg, CTS output */
  GPIOD_PSOR = PINBIT_CTS;  /* set CTS bit hi */
  
  USBSERIAL.begin(115200);
  delay(2000);
  blinkMilliSecElapsed = 0;

  int cmdlo_out = 1;

  GPIOA_PDOR = PINBIT_RN52_CMDLO * cmdlo_out; 

  while (1) {
    if (blinkMilliSecElapsed > 5000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
      cmdlo_out = !cmdlo_out;
      GPIOA_PDOR = PINBIT_RN52_CMDLO * cmdlo_out; 
      if (cmdlo_out) {
        USBSERIAL.println("c1");
      } else {
        USBSERIAL.println("c0");
      }
    }
    
    // pinVal = (GPIOC_PDIR & PINBIT_MBUS_SENSE) != 0;
    // if (pinVal != prevPinVal) {
    //   msgStrLen = snprintf(msgStr, MSG_STR_SIZE, "%08x %d", micros(), pinVal);
    //   USBSERIAL.println(msgStr);
    //   prevPinVal = pinVal;
    // }
    yield();
  }
}
