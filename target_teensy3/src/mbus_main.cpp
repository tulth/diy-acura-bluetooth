#include <stdint.h>
#include <WProgram.h>
#include <usb_dev.h>
#include <usb_serial.h>
#include <core_pins.h>
#include "mbus.h"

#define BYTE_BUF_SIZE 64
#define MSG_BUF_SIZE 4
#define MSG_STR_SIZE 256

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  MbusStruct mbusRx;
  uint8_t rxByteBuf[BYTE_BUF_SIZE];
  uint8_t txByteBuf[BYTE_BUF_SIZE];
  MbusMsgStruct msgBuf[MSG_BUF_SIZE];
  char msgStr[MSG_STR_SIZE];
  MbusMsgStruct rxMsg;
  int msgStrLen;
  bool driveMbusPinLo;
  
  usb_init();
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  PORTC_PCR2 |= PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
  GPIOC_PDDR |= 0x20;  // gpio data direction reg, for led bit

  usb_serial_write("sniffer:\n", 9);
  blinkMilliSecElapsed = 0;
  mbus_init(&mbusRx,
            rxByteBuf, BYTE_BUF_SIZE,
            txByteBuf, BYTE_BUF_SIZE,
            msgBuf, MSG_BUF_SIZE
            );


  while (1) {
    if (blinkMilliSecElapsed > 10000) {
      GPIOC_PTOR = 0x20;  // gpio toggle reg, for led bit
      blinkMilliSecElapsed = 0;
    }
    mbus_update(&mbusRx,
                micros(),
                (GPIOC_PDIR & 0x04) != 0,
                &driveMbusPinLo
                );
    if (!mbus_is_empty(&mbusRx)) {
      mbus_pop(&mbusRx, &rxMsg);
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
