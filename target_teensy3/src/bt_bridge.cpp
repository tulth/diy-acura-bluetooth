#include <stdint.h>
#include <WProgram.h>

#define PINBIT_LED (1<<5)
#define PINBIT_CMDLO (1<<13)
#define PINBIT_STATECHANGE (1<<7)

#define USBSERIAL Serial
#define HWSERIAL1 Serial1

void setCmdMode(void);
void readUntilNewline(void);
void reconnectBluetooth(void);

extern "C" int main(void)
{
  int incomingByte;

  PORTA_PCR13 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* CMD LO */
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* state change bit */

  GPIOA_PDDR |= PINBIT_CMDLO;  /* gpio data direction reg, for cmd lo */
  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  HWSERIAL1.begin(115200);
  USBSERIAL.begin(115200);
  setCmdMode();
  reconnectBluetooth();

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
    GPIOA_PSOR = PINBIT_CMDLO;  /* set cmd lo 1 */
    delay(100);
    GPIOA_PCOR = PINBIT_CMDLO;  /* set cmd lo 0 */
    gReadLineDone = 0;
    while (!gReadLineDone) {
      readUntilNewline();
    }
    if (gReadLine.startsWith("CMD")) {
      done = 1;
    }
  }
}

void reconnectBluetooth(void)
{
  HWSERIAL1.println("B");
}

