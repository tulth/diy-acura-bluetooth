#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"
#include "rn52.h"
#include "app_debug.h"

#define RN52_AVRCP_CMD_MEM_SIZE 16
#define BYTE_MEM_SIZE 64
#define MSG_MEM_SIZE 2
#define MSG_STR_SIZE 256

#define PINBIT_LED (1<<5)
#define PINBIT_MBUS_SENSE (1<<2)
#define PINBIT_MBUS_DRIVE_LO (1<<1)
#define PINBIT_RN52_CMDLO (1<<13)
#define PINBIT_RN52_CONNSTAT_EVENT (1<<7)

#define USBSERIAL Serial
#define HWSERIAL1 Serial1

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  uint8_t rxByteMem[BYTE_MEM_SIZE];
  uint8_t txByteMem[BYTE_MEM_SIZE];
  uint8_t rxNibble;
  MbusRxMsgStruct rxMsgMem[MSG_MEM_SIZE];
  MbusTxMsgStruct txMsgMem[MSG_MEM_SIZE];
  MbusRxMsgStruct rxMsg;
  MbusTxMsgStruct txMsg;
  MbusPhyStruct mbusPhy;
  MbusLinkStruct mbusLink;
  bool driveMbusPinLo = false;
  bool mbusPhyDirectionUpdated;
  char msgStr[MSG_STR_SIZE];
  int msgStrLen;
  /* for rn52 bluetooth */
  Rn52Struct rn52;
  bool rn52CmdModePin = true;
  uint8_t rn52AvrcpCmdMem[RN52_AVRCP_CMD_MEM_SIZE];

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */
  PORTA_PCR13 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* RN52 CMD LO */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* RN52 connection status change bit */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */
  GPIOC_PDDR |= PINBIT_MBUS_DRIVE_LO;  /* gpio data direction reg, for driveMbusPinLo */
  GPIOA_PDDR |= PINBIT_RN52_CMDLO;  /* gpio data direction reg, for cmd lo */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */
  GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive mbus low */
  GPIOA_PSOR = PINBIT_RN52_CMDLO;  /* don't drive rn52 cmd request low (yet) */

  HWSERIAL1.begin(115200);
  USBSERIAL.begin(115200);
  blinkMilliSecElapsed = 0;
  mbus_phy_init(&mbusPhy,
                rxByteMem,
                BYTE_MEM_SIZE,
                txByteMem,
                BYTE_MEM_SIZE);
  mbus_phy_rx_enable(&mbusPhy);
  mbus_link_init(&mbusLink,
                 rxMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusRxMsgStruct),
                 txMsgMem,
                 MSG_MEM_SIZE * sizeof(MbusTxMsgStruct),
                 &(mbusPhy.tx.byteFifo));
  rn52_init(&rn52,
            serial_available,
            serial_getchar,
            serial_putchar,
            rn52AvrcpCmdMem,
            sizeof(rn52AvrcpCmdMem)
            );

  delay(2000);
  app_debug_print("start\n");
  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
    }

    /* mbusPhy */
    mbus_phy_update(&mbusPhy,
                    micros(),
                    (GPIOC_PDIR & PINBIT_MBUS_SENSE) != 0,
                    &driveMbusPinLo
                    );
    if (driveMbusPinLo) {
      GPIOC_PSOR = PINBIT_MBUS_DRIVE_LO;  /* drive low, so we drive 1 out; inverted by the drive transistor */
    } else {
      GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't pull low */
    }
    
    /* mbusPhy->mbusLink */
    if (!mbus_phy_rx_is_empty(&mbusPhy)) {
      rxNibble = mbus_phy_rx_pop(&mbusPhy);
      mbus_link_rx_push_nibble(&mbusLink, rxNibble);
    }

    /* mbusLink */
    mbus_link_update(&mbusLink, mbus_phy_rx_is_busy(&mbusPhy), mbus_phy_tx_is_busy(&mbusPhy), &mbusPhyDirectionUpdated);
    if (mbusPhyDirectionUpdated) {
      if (mbus_link_is_direction_rx(&mbusLink)) {
        mbus_phy_tx_disable(&mbusPhy);
        mbus_phy_rx_enable(&mbusPhy);
      } else {
        //        mbus_phy_rx_disable(&mbusPhy);
        mbus_phy_tx_enable(&mbusPhy);
      }
    }
    /* print mbusLink rx msg if one is available */
    if (!mbus_link_rx_is_empty(&mbusLink)) {
      mbus_link_rx_pop(&mbusLink, &rxMsg);
      msgStrLen = mbus_link_msgToStr(&rxMsg,
                                     msgStr,
                                     MSG_STR_SIZE);
      if ((!rxMsg.errId) && (rxMsg.parsed.directionH2C)) {
        if (rxMsg.parsed.msgType == MSGTYPE_ping) {
          txMsg.nibbles.packNibbles = 0x982;
          txMsg.nibbles.numNibbles = 3;
          mbus_link_tx_push(&mbusLink, &txMsg);
          USBSERIAL.println("sent pong");
        } else if (rxMsg.parsed.msgType == MSGTYPE_setPlayState && rxMsg.parsed.body.setPlayState.play) {
          // txMsg.nibbles.packNibbles = 0x9BA1002000A2;
          // txMsg.nibbles.numNibbles = 12;
          txMsg.nibbles.packNibbles = 0x9940201014300011;
          txMsg.nibbles.numNibbles = 16;
          mbus_link_tx_push(&mbusLink, &txMsg);
          USBSERIAL.println("sent no shuttle reply");
        } else if (rxMsg.parsed.msgType == MSGTYPE_headPowerOn) {
          // txMsg.nibbles.packNibbles = 0x9BA1002000A2;
          // txMsg.nibbles.numNibbles = 12;
          txMsg.nibbles.packNibbles = 0x992020101430009F;
          //txMsg.nibbles.packNibbles = 0x9C60115754573;
          txMsg.nibbles.numNibbles = 16;
          mbus_link_tx_push(&mbusLink, &txMsg);
          txMsg.nibbles.packNibbles = 0x9F000061;
          txMsg.nibbles.numNibbles = 8;
          mbus_link_tx_push(&mbusLink, &txMsg);
          USBSERIAL.println("sent ??");
        }
      }
      USBSERIAL.println(msgStr);
    }
    //    app_debug_print("aaa\n");
    //  }{
    /* rn 52 bluetooth */
    rn52_update(&rn52,
                millis(),
                (GPIOD_PDIR & PINBIT_RN52_CONNSTAT_EVENT) != 0,
                &rn52CmdModePin);
    if (rn52CmdModePin) {
      GPIOA_PSOR = PINBIT_RN52_CMDLO;
    } else {
      GPIOA_PCOR = PINBIT_RN52_CMDLO;
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

void app_debug_print(const char *arg)
{
  USBSERIAL.println(arg);
}
