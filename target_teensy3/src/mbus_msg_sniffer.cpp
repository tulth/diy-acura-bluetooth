#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <usb_dev.h>
#include <usb_serial.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"

#define BYTE_MEM_SIZE 64
#define MSG_MEM_SIZE 2
#define MSG_STR_SIZE 256

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  uint8_t rxByteMem[BYTE_MEM_SIZE];
  uint8_t txByteMem[BYTE_MEM_SIZE];
  uint8_t rxNibble;
  MbusMsgStruct rxMsgMem[MSG_MEM_SIZE];
  MbusMsgStruct txMsgMem[MSG_MEM_SIZE];
  MbusMsgStruct rxMsg;
  MbusPhyStruct phy;
  MbusLinkStruct link;
  bool driveMbusPinLo;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;
  
  usb_init();
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  PORTC_PCR2 |= PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
  GPIOC_PDDR |= 0x20;  // gpio data direction reg, for led bit

  blinkMilliSecElapsed = 0;

  mbus_phy_init(&phy,
                rxByteMem,
                BYTE_MEM_SIZE,
                txByteMem,
                BYTE_MEM_SIZE);
  mbus_phy_rx_enable(&phy);
  mbus_link_init(&link,
                 rxMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusMsgStruct),
                 txMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusMsgStruct));

  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = 0x20;  // gpio toggle reg, for led bit
      blinkMilliSecElapsed = 0;
    }

    /* phy */
    mbus_phy_update(&phy,
                    micros(),
                    (GPIOC_PDIR & 0x04) != 0,
                    &driveMbusPinLo
                    );

    /* phy->link */
    if (!mbus_phy_rx_is_empty(&phy)) {
      rxNibble = mbus_phy_rx_pop(&phy);
      mbus_link_rx_update(&link, rxNibble);
    }
    
    /* link */
    if (!mbus_link_rx_is_empty(&link)) {
      mbus_link_rx_pop(&link, &rxMsg);
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
