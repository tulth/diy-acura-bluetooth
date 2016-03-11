#include <stdint.h>
#include <WProgram.h>
#include <usb_dev.h>
#include <usb_serial.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"

#define BYTE_BUF_SIZE 64

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  MbusPhyStruct mbusPhyRx;
  uint8_t rxByteBuf[BYTE_BUF_SIZE];
  uint8_t txByteBuf[BYTE_BUF_SIZE];
  bool driveMbusPinLo;
  uint8_t rxNibble;
  
  usb_init();
  PORTC_PCR5 |= PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
  PORTC_PCR2 |= PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
  GPIOC_PDDR |= 0x20;  // gpio data direction reg, for led bit

  usb_serial_write("sniffer:\n", 9);
  blinkMilliSecElapsed = 0;
  mbus_phy_init(&mbusPhyRx,
                rxByteBuf, BYTE_BUF_SIZE,
                txByteBuf, BYTE_BUF_SIZE);

  mbus_phy_rx_enable(&mbusPhyRx);

  while (1) {
    if (blinkMilliSecElapsed > 10000) {
      GPIOC_PTOR = 0x20;  // gpio toggle reg, for led bit
      blinkMilliSecElapsed = 0;
    }
    mbus_phy_update(&mbusPhyRx,
                    micros(),
                    (GPIOC_PDIR & 0x04) != 0,
                    &driveMbusPinLo
                    );
    if (!mbus_phy_rx_is_empty(&mbusPhyRx)) {
      rxNibble = mbus_phy_rx_pop(&mbusPhyRx);
      usb_serial_putchar(mbus_phy_rxnibble2ascii(rxNibble));
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
