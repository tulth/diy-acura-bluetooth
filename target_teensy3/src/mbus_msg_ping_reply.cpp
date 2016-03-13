#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <usb_dev.h>
#include <usb_serial.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"
#include "circular_buffer.h"

#define BYTE_MEM_SIZE 64
#define MSG_MEM_SIZE 2
#define MSG_STR_SIZE 256

#define PINBIT_LED (1<<5)
#define PINBIT_MBUS_SENSE (1<<2)

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  uint8_t rxByteMem[BYTE_MEM_SIZE];
  uint8_t txByteMem[BYTE_MEM_SIZE];
  uint8_t rxNibble;
  MbusRxMsgStruct rxMsgMem[MSG_MEM_SIZE];
  MbusTxMsgStruct txMsgMem[MSG_MEM_SIZE];
  MbusRxMsgStruct rxMsg;
  MbusPhyStruct phy;
  MbusLinkStruct link;
  bool driveMbusPinLo = false;
  bool phyDirectionUpdated;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;
  
  usb_init();
  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */
  
  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */
  GPIOC_PDDR |= (1<<1);  /* gpio data direction reg, for driveMbusPinLo */
  GPIOC_PCOR = (1<<1);  // don't drive low

  blinkMilliSecElapsed = 0;

  mbus_phy_init(&phy,
                rxByteMem,
                BYTE_MEM_SIZE,
                txByteMem,
                BYTE_MEM_SIZE);
  mbus_phy_rx_enable(&phy);
  mbus_link_init(&link,
                 rxMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusRxMsgStruct),
                 txMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusTxMsgStruct),
                 &(phy.tx.byteFifo));

  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  // gpio toggle reg, for led bit
      blinkMilliSecElapsed = 0;
    }

    /* phy */
    mbus_phy_update(&phy,
                    micros(),
                    (GPIOC_PDIR & PINBIT_MBUS_SENSE) != 0,
                    &driveMbusPinLo
                    );
    if (driveMbusPinLo) {
      GPIOC_PSOR = (1<<1);  // drive low, so we drive 1 out, which is inverted by the drive transistor
    } else {
      GPIOC_PCOR = (1<<1);  // don't pull low
    }

    /* phy->link */
    if (!mbus_phy_rx_is_empty(&phy)) {
      rxNibble = mbus_phy_rx_pop(&phy);
      mbus_link_rx_push_nibble(&link, rxNibble);
    }

    
    /* link */
    mbus_link_update(&link, mbus_phy_rx_is_busy(&phy), mbus_phy_tx_is_busy(&phy), &phyDirectionUpdated);
    if (phyDirectionUpdated) {
      if (mbus_link_is_direction_rx(&link)) {
        mbus_phy_tx_disable(&phy);
        mbus_phy_rx_enable(&phy);
      } else {
        mbus_phy_rx_disable(&phy);
        mbus_phy_tx_enable(&phy);
      }
    }
    /* print link rx msg if one is available */
    if (!mbus_link_rx_is_empty(&link)) {
      mbus_link_rx_pop(&link, &rxMsg);
      if ((!rxMsg.errId) && (rxMsg.parsed.directionH2C)) {
        if (rxMsg.parsed.msgType == MSGTYPE_ping) {
          mbus_phy_tx_push(&phy, 0x9);
          mbus_phy_tx_push(&phy, 0x8);
          mbus_phy_tx_push(&phy, 0x2);
          usb_serial_write("sent pong\n", 10);
        }
      }
      msgStrLen = mbus_link_msgToStr(&rxMsg,
                                     msgStr,
                                     MSG_STR_SIZE);
      usb_serial_write(msgStr, msgStrLen - 1);  // -1 to skip null terminator
      usb_serial_putchar('\n');
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
