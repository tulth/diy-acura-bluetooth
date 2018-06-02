#include <stdint.h>
#include <WProgram.h>
#include "rn52.h"

#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_RN52_CMDLO CORE_PIN4_BITMASK
#define PINCFG_RN52_CMDLO CORE_PIN4_CONFIG
#define PINDDR_RN52_CMDLO CORE_PIN4_DDRREG
#define PINDOR_RN52_CMDLO CORE_PIN4_PORTREG
#define PINBIT_RN52_CONNSTAT_EVENT CORE_PIN5_BITMASK
#define PINBIT_CTS CORE_PIN2_BITMASK

#define USBSERIAL Serial
#define HWSERIAL1 Serial1

void setCmdMode(void);
void readUntilNewline(void);
void reconnectRn52(void);

extern "C" int main(void)
{
  int incomingByte;

  PINCFG_RN52_CMDLO = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* RN52 CMD LO */
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* state change bit */

  PINDDR_RN52_CMDLO |= PINBIT_RN52_CMDLO;  /* gpio data direction reg, for cmd lo */
  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  PORTD_PCR0 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* CTS */
  GPIOD_PDDR |= PINBIT_CTS;  /* gpio data direction reg, CTS output */
  GPIOD_PSOR = PINBIT_CTS;  /* set CTS bit hi */
  
  HWSERIAL1.begin(115200);
  USBSERIAL.begin(115200);
  setCmdMode();
  reconnectRn52();

  while (1) {
    if (USBSERIAL.available() > 0) {
      GPIOC_PSOR = PINBIT_LED;  /* set led high */
      incomingByte = USBSERIAL.read();
      HWSERIAL1.write(incomingByte);
      GPIOC_PCOR = PINBIT_LED;  /* set led lo */
    }
    if (HWSERIAL1.available() > 0) {
      GPIOC_PSOR = PINBIT_LED;  /* set led high */
      incomingByte = HWSERIAL1.read();
      USBSERIAL.write(incomingByte);
      GPIOC_PCOR = PINBIT_LED;  /* set led lo */
    }
    yield();
  }
}

String gReadLine;
uint8_t gReadLineDone;

void readUntilNewline(void)
{
  char incomingByte;
  if (HWSERIAL1.available() > 0) {
    incomingByte = HWSERIAL1.read();
    gReadLine.concat(incomingByte);

    if (gReadLine.endsWith('\n')) {
      gReadLineDone =  1;
    }
  }
}


void setCmdMode(void)
{
  uint8_t done = 0;

  while(!done) {
    PINDOR_RN52_CMDLO |= PINBIT_RN52_CMDLO;
    delay(100);
    PINDOR_RN52_CMDLO &= ~PINBIT_RN52_CMDLO;
    gReadLineDone = 0;
    while (!gReadLineDone) {
      readUntilNewline();
    }
    if (gReadLine.startsWith("CMD")) {
      done = 1;
    }
  }
}

void reconnectRn52(void)
{
  HWSERIAL1.println("B");
}

