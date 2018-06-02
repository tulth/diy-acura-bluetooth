#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"
#include "app_debug.h"
#define BYTE_MEM_SIZE 64

#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_MBUS_SENSE CORE_PIN23_BITMASK
#define PINBIT_MBUS_DRIVE_LO CORE_PIN22_BITMASK

#define USBSERIAL Serial

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  uint8_t rxByteMem[BYTE_MEM_SIZE];
  uint8_t txByteMem[BYTE_MEM_SIZE];
  char txCharMem[32];
  uint8_t rxNibble;
  char txChar;
  uint8_t txNib;
  uint8_t txNibQueue[32];
  int txNibQueueIdx;
  bool transmitting = false;
  int txNibQueueLastIdx = 0;
  MbusPhyStruct phy;
  bool driveMbusPinLo = false;

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */
  GPIOC_PDDR |= PINBIT_MBUS_DRIVE_LO;  /* gpio data direction reg, for driveMbusPinLo */
  GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive low */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */

  USBSERIAL.begin(115200);
  blinkMilliSecElapsed = 0;
  mbus_phy_init(&phy,
                rxByteMem,
                BYTE_MEM_SIZE,
                txByteMem,
                BYTE_MEM_SIZE);
  mbus_phy_rx_enable(&phy);

  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
    }

    /* phy */
    mbus_phy_update(&phy,
                    micros(),
                    (GPIOC_PDIR & PINBIT_MBUS_SENSE) != 0,
                    &driveMbusPinLo
                    );
    if (driveMbusPinLo) {
      GPIOC_PSOR = PINBIT_MBUS_DRIVE_LO;  /* drive low, so we drive 1 out - inverted by the drive transistor */
    } else {
      GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't pull low */
    }

    if (!mbus_phy_rx_is_empty(&phy)) {
      rxNibble = mbus_phy_rx_pop(&phy);
      if (rxNibble == MBUS_END_MSG_CODE) {
        app_debug_putchar('\r');
        app_debug_putchar('\n');
      } else {
        app_debug_putchar(mbus_phy_rxnibble2ascii(rxNibble));
      }        
    }
    if (USBSERIAL.available()) {
      txChar = USBSERIAL.read();
      txNib = mbus_phy_ascii2txnibble(txChar);
      if (txChar == 0x0d) {  // newlines come in as 0x0D for some reason
        txNib = MBUS_END_MSG_CODE;
      }
      // app_debug_printf("beep %d %d\r\n", txChar, txNib);
      if (txNib & 0x80) {  // misbit set if bad nibble
        ; // do nothing
      } else {
        txNibQueue[txNibQueueLastIdx] = txNib;
        txNibQueueLastIdx++;
        if (txNib == MBUS_END_MSG_CODE) {
          if (!mbus_phy_rx_is_busy(&phy) and !transmitting) {
            transmitting = true;            
            mbus_phy_tx_enable(&phy);
            for (txNibQueueIdx=0; txNibQueueIdx < txNibQueueLastIdx; txNibQueueIdx++) {
              mbus_phy_tx_push(&phy, txNibQueue[txNibQueueIdx]);
            }
            for (txNibQueueIdx=0; txNibQueueIdx < txNibQueueLastIdx; txNibQueueIdx++) {
              app_debug_printf("%x  ", txNibQueue[txNibQueueIdx]);
            }
            app_debug_print("\r\n");
            txNibQueueLastIdx = 0;
          }
        }
      }
    }
    if (transmitting) {
      if (!mbus_phy_tx_is_busy(&phy)) {
        transmitting = false;
        mbus_phy_tx_disable(&phy);
      }
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
