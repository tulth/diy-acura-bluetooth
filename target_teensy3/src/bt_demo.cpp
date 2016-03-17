#include <stdint.h>
#include <WProgram.h>
#include "rn52.h"
#include "app_debug.h"

#define RN52_AVRCP_CMD_MEM_SIZE 16

#define PINBIT_LED (1<<5)
#define PINBIT_RN52_CMDLO (1<<13)
#define PINBIT_RN52_CONNSTAT_EVENT (1<<7)

#define USBSERIAL Serial
#define HWSERIAL1 Serial1

void setCmdMode(void);
void readUntilNewline(void);
void reconnectRn52(void);

extern "C" int main(void)
{
  int incomingByte;
  Rn52Struct rn52;
  bool cmdPin = true;
  uint8_t avrcpCmdMem[RN52_AVRCP_CMD_MEM_SIZE];

  PORTA_PCR13 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* CMD LO */
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* state change bit */

  GPIOA_PDDR |= PINBIT_RN52_CMDLO;  /* gpio data direction reg, for cmd lo */
  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  HWSERIAL1.begin(115200);
  USBSERIAL.begin(115200);

  rn52_init(&rn52,
            serial_available,
            serial_getchar,
            serial_putchar,
            avrcpCmdMem,
            sizeof(avrcpCmdMem)
            );
  
  while (1) {
    rn52_update(&rn52,
                millis(),
                (GPIOC_PDIR & PINBIT_RN52_CONNSTAT_EVENT) != 0,
                &cmdPin);
    if (cmdPin) {
      GPIOC_PSOR = PINBIT_RN52_CMDLO;
    } else {
      GPIOC_PCOR = PINBIT_RN52_CMDLO;
    }

    
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
    GPIOA_PSOR = PINBIT_RN52_CMDLO;  /* set cmd lo 1 */
    delay(100);
    GPIOA_PCOR = PINBIT_RN52_CMDLO;  /* set cmd lo 0 */
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
