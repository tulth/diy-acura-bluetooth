#include <stdbool.h>
#include <stdint.h>
#include <WProgram.h>
#include <core_pins.h>
#include "mbus_phy.h"
#include "mbus_link.h"
#include "rn52.h"
#include "app_debug.h"

// #define CPU_RESTART_ADDR 			(uint32_t *)0xE000ED0C
// #define CPU_RESTART_VAL 			0x5FA0004
// #define CPU_RESTART 				(*CPU_RESTART_ADDR = CPU_RESTART_VAL)
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

#define RN52_AVRCP_CMD_MEM_SIZE 16
#define BYTE_MEM_SIZE 256
#define RX_MSG_MEM_NUM 4
#define TX_MSG_MEM_NUM 16
#define MSG_STR_SIZE 256


#define PINBIT_LED CORE_PIN13_BITMASK
#define PINBIT_MBUS_SENSE CORE_PIN23_BITMASK
#define PINBIT_MBUS_DRIVE_LO CORE_PIN22_BITMASK
#define PINBIT_RN52_CMDLO CORE_PIN4_BITMASK
#define PINCFG_RN52_CMDLO CORE_PIN4_CONFIG
#define PINDDR_RN52_CMDLO CORE_PIN4_DDRREG
#define PINDOR_RN52_CMDLO CORE_PIN4_PORTREG
#define PINBIT_RN52_CONNSTAT_EVENT CORE_PIN5_BITMASK

#define USBSERIAL Serial
#define HWSERIAL1 Serial1

typedef enum {
  START = 1,
  STOPPED,
  START_PLAYING,
  PLAYING,
  MBUS_SAYS_STOP,
  RN52_STOPPED
} appStateType;

typedef struct {
  appStateType state;
  elapsedMillis timer;
  fifo simpleMbusMsgFifo;
  MbusLinkStruct link;
  Rn52Struct rn52;
  bool noshuttle;
  bool repeatAll;
  bool repeatOne;
  uint8_t disk;
  uint8_t numTracks;
  uint8_t track;
  uint8_t index;
  unsigned int seconds;
} appStruct;

typedef enum {
  OTHER = 1,
  ping,
  headPowerOn,
  play,
  pause,
  stop,
  switchDisk1,
  switchDisk2,
  switchDisk3,
  switchDisk4,
  switchDisk5,
  switchDisk6,
  next,
  prev
  //  repeatAll,
  //  random,
} appMbusSimpleRxMsgType;

static inline const char *simpleMbusMsg2Str(appMbusSimpleRxMsgType msg)
{
  switch(msg) {
  case OTHER:
    return "OTHER";
    break;
  case ping:
    return "ping";
    break;
  case headPowerOn:
    return "headPowerOn";
    break;
  case play:
    return "play";
    break;
  case pause:
    return "pause";
    break;
  case stop:
    return "stop";
    break;
  case switchDisk1:
    return "switchDisk1";
    break;
  case switchDisk2:
    return "switchDisk2";
    break;
  case switchDisk3:
    return "switchDisk3";
    break;
  case switchDisk4:
    return "switchDisk4";
    break;
  case switchDisk5:
    return "switchDisk5";
    break;
  case switchDisk6:
    return "switchDisk6";
    break;
  }
}

void app_init(appStruct *app);
void app_update(appStruct *app);

extern "C" int main(void)
{
  elapsedMillis blinkMilliSecElapsed;
  bool playing = false;
  uint8_t rxByteMem[BYTE_MEM_SIZE];
  uint8_t txByteMem[BYTE_MEM_SIZE];
  uint8_t rxNibble;
  MbusRxMsgStruct rxMsgMem[RX_MSG_MEM_NUM];
  MbusTxMsgStruct txMsgMem[TX_MSG_MEM_NUM];
  MbusPhyStruct mbusPhy;
  appStruct app;
  bool driveMbusPinLo = false;
  bool mbusPhyDirectionUpdated;
  /* for rn52 bluetooth */
  bool rn52CmdModePin = true;
  bool rn52EventPin;
  bool prevRn52EventPin = true;
  uint8_t rn52AvrcpCmdMem[RN52_AVRCP_CMD_MEM_SIZE];

  PORTC_PCR5 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* LED */
  PORTC_PCR2 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* MBUS SENSE */
  PORTC_PCR1 = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* MBUS DRIVE LO */
  PINCFG_RN52_CMDLO = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); /* RN52 CMD LO */
  PORTD_PCR7 = PORT_PCR_PE  | PORT_PCR_PS  | PORT_PCR_MUX(1); /* RN52 connection status change bit */

  GPIOC_PDDR |= PINBIT_LED;  /* gpio data direction reg, for led bit */
  GPIOC_PDDR |= PINBIT_MBUS_DRIVE_LO;  /* gpio data direction reg, for driveMbusPinLo */
  PINDDR_RN52_CMDLO |= PINBIT_RN52_CMDLO;  /* gpio data direction reg, for cmd lo */

  GPIOC_PCOR = PINBIT_LED;  /* set led bit low */
  GPIOC_PCOR = PINBIT_MBUS_DRIVE_LO;  /* don't drive mbus low */
  PINDOR_RN52_CMDLO |= PINBIT_RN52_CMDLO;  /* don't drive rn52 cmd request low (yet) */

  HWSERIAL1.begin(115200);
  USBSERIAL.begin(115200);
  delay(2000);
  blinkMilliSecElapsed = 0;
  mbus_phy_init(&mbusPhy,
                rxByteMem,
                BYTE_MEM_SIZE,
                txByteMem,
                BYTE_MEM_SIZE);
  mbus_phy_rx_enable(&mbusPhy);
  mbus_link_init(&app.link,
                 rxMsgMem,
                 RX_MSG_MEM_NUM * sizeof(MbusRxMsgStruct),
                 txMsgMem,
                 TX_MSG_MEM_NUM * sizeof(MbusTxMsgStruct),
                 &(mbusPhy.tx.byteFifo));
  rn52_init(&app.rn52,
            serial_available,
            serial_getchar,
            serial_putchar,
            rn52AvrcpCmdMem,
            sizeof(rn52AvrcpCmdMem)
            );

  app_init(&app);

  app_debug_print("start\r\n");
  while (1) {
    /* blink */
    if (blinkMilliSecElapsed > 1000) {
      GPIOC_PTOR = PINBIT_LED;  /* gpio toggle reg, for led bit */
      blinkMilliSecElapsed = 0;
      //      app_debug_print("beep\r\n");
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

    /* mbusPhy->app.link */
    if (!mbus_phy_rx_is_empty(&mbusPhy)) {
      rxNibble = mbus_phy_rx_pop(&mbusPhy);
      mbus_link_rx_push_nibble(&app.link, rxNibble);
    }

    /* app.link */
    mbus_link_update(&app.link, mbus_phy_rx_is_busy(&mbusPhy), mbus_phy_tx_is_busy(&mbusPhy), &mbusPhyDirectionUpdated);
    if (mbusPhyDirectionUpdated) {
      if (mbus_link_is_direction_rx(&app.link)) {
        mbus_phy_tx_disable(&mbusPhy);
        // mbus_phy_rx_enable(&mbusPhy);
      } else {
        //        mbus_phy_rx_disable(&mbusPhy);
        mbus_phy_tx_enable(&mbusPhy);
      }
    }
    /* print app.link rx msg if one is available */
    app_update(&app);

    // if (!mbus_link_rx_is_empty(&app.link)) {
    //   mbus_link_rx_pop(&app.link, &rxMsg);
    //   msgStrLen = mbus_link_msgToStr(&rxMsg,
    //                                  msgStr,
    //                                  MSG_STR_SIZE);
    //   app_debug_printf("rxmsg: %s\n", msgStr);
    //   if ((!rxMsg.errId) && (rxMsg.parsed.directionH2C)) {
    //     if (rxMsg.parsed.msgType == MSGTYPE_ping) {
    //       txMsg.nibbles.packNibbles = 0x982;
    //       txMsg.nibbles.numNibbles = 3;
    //       mbus_link_tx_push(&app.link, &txMsg);
    //       USBSERIAL.println("sent pong");
    //     } else if (rxMsg.parsed.msgType == MSGTYPE_setPlayState && rxMsg.parsed.body.setPlayState.play) {
    //       app_debug_printf("rn52_avrcp_is_playing: %d\n", rn52_avrcp_is_playing(&app.rn52));
    //       if (rn52_avrcp_is_playing(&app.rn52)) {
    //         txMsg.nibbles.packNibbles = 0x9B950300001D;
    //         txMsg.nibbles.numNibbles = 12;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9F000007;
    //         txMsg.nibbles.numNibbles = 8;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9C501137502FD;
    //         txMsg.nibbles.numNibbles = 13;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9F000007;
    //         txMsg.nibbles.numNibbles = 8;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9C501137502FD;
    //         txMsg.nibbles.numNibbles = 13;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9F000007;
    //         txMsg.nibbles.numNibbles = 8;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9A00000000004;
    //         txMsg.nibbles.numNibbles = 13;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9D000000005;
    //         txMsg.nibbles.numNibbles = 11;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9B950300001D;
    //         txMsg.nibbles.numNibbles = 12;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         txMsg.nibbles.packNibbles = 0x9940301042200014;
    //         txMsg.nibbles.numNibbles = 16;
    //         mbus_link_tx_push(&app.link, &txMsg);

    //         mbusTxMsgTimer = 0;
    //         playing = true;
    //       }
    //       // OLD
    //       // txMsg.nibbles.packNibbles = 0x9BA1002000A2;
    //       // txMsg.nibbles.numNibbles = 12;
    //       // txMsg.nibbles.packNibbles = 0x9BA1002000A2;
    //       // txMsg.nibbles.numNibbles = 12;
    //       //          txMsg.nibbles.packNibbles = 0x992020101430009F;
    //       //txMsg.nibbles.packNibbles = 0x9C60115754573;
    //       // //          txMsg.nibbles.numNibbles = 16;
    //       // mbus_link_tx_push(&app.link, &txMsg);
    //       // txMsg.nibbles.packNibbles = 0x9F000061;
    //       // txMsg.nibbles.numNibbles = 8;
    //       // mbus_link_tx_push(&app.link, &txMsg);
    //       // txMsg.nibbles.packNibbles = 0x9F00F26E;
    //       // txMsg.nibbles.numNibbles = 8;
    //       // mbus_link_tx_push(&app.link, &txMsg);

    //       // txMsg.nibbles.packNibbles = 0x9C60115754573;
    //       // txMsg.nibbles.numNibbles = 13;
    //       // mbus_link_tx_push(&app.link, &txMsg);

    //       // txMsg.nibbles.packNibbles = 0x9A00000000004;
    //       // txMsg.nibbles.numNibbles = 13;
    //       // mbus_link_tx_push(&app.link, &txMsg);
    //     } else if (rxMsg.parsed.msgType == MSGTYPE_headPowerOn) {
    //       txMsg.nibbles.packNibbles = 0x9B950300001D;
    //       txMsg.nibbles.numNibbles = 12;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9F000007;
    //       txMsg.nibbles.numNibbles = 8;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9C501137502FD;
    //       txMsg.nibbles.numNibbles = 13;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9F000007;
    //       txMsg.nibbles.numNibbles = 8;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9C501137502FD;
    //       txMsg.nibbles.numNibbles = 13;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9F000007;
    //       txMsg.nibbles.numNibbles = 8;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9A00000000004;
    //       txMsg.nibbles.numNibbles = 13;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9D000000005;
    //       txMsg.nibbles.numNibbles = 11;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9B950300001D;
    //       txMsg.nibbles.numNibbles = 12;
    //       mbus_link_tx_push(&app.link, &txMsg);

    //       txMsg.nibbles.packNibbles = 0x9940301042200014;
    //       txMsg.nibbles.numNibbles = 16;
    //       mbus_link_tx_push(&app.link, &txMsg);


    //       // txMsg.nibbles.packNibbles = 0x992020101430009F;
    //       // txMsg.nibbles.numNibbles = 16;
    //       // mbus_link_tx_push(&app.link, &txMsg);

    //       // txMsg.nibbles.packNibbles = 0x9F000061;
    //       // txMsg.nibbles.numNibbles = 8;
    //       // mbus_link_tx_push(&app.link, &txMsg);
    //     }
    //   }
    // }

    // if (playing && (mbusTxMsgTimer >= 1000)) {
    //   txMsg.nibbles.packNibbles = 0x9940301042200014;
    //   txMsg.nibbles.numNibbles = 16;
    //   mbus_link_tx_push(&app.link, &txMsg);

    //   mbusTxMsgTimer = 0;
    // }
    /* rn 52 bluetooth */
    rn52EventPin = (GPIOD_PDIR & PINBIT_RN52_CONNSTAT_EVENT) != 0;
    // if (rn52EventPin != prevRn52EventPin) {
    //   app_debug_printf("rn52ev: %08x %d\n", millis(), rn52EventPin);
    //   prevRn52EventPin = rn52EventPin;
    // }
    rn52_update(&app.rn52,
                millis(),
                rn52EventPin,
                &rn52CmdModePin);
    if (rn52CmdModePin) {
      PINDOR_RN52_CMDLO |= PINBIT_RN52_CMDLO;
    } else {
      PINDOR_RN52_CMDLO &= ~PINBIT_RN52_CMDLO;
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

void _app_push_simple_mbus_msg(appStruct *app, appMbusSimpleRxMsgType simpleMbusMsg)
{
  app_debug_printf("queuing %d\r\n", simpleMbusMsg);
  if (!fifo_is_full(&(app->simpleMbusMsgFifo))) {
    fifo_push(&(app->simpleMbusMsgFifo), &simpleMbusMsg);
  }
}

void app_decode_push_mbus_msg(appStruct *pApp, MbusRxMsgStruct *pRxMsg)
{
  appMbusSimpleRxMsgType simpleRxMsg;
  if (pRxMsg->errId != 0) {
    return;
  }
  if (!pRxMsg->parsed.directionH2C) {
    return;
  }
  app_debug_printf("pRxMsg->parsed.msgType: %02x\r\n", pRxMsg->parsed.msgType);
  switch (pRxMsg->parsed.msgType) {
  case MSGTYPE_ping:
    _app_push_simple_mbus_msg(pApp, ping);
    break;
  case MSGTYPE_setPlayState:
    if(pRxMsg->parsed.body.setPlayState.stop) {
      app_debug_print("queueing a stop\r\n");
      _app_push_simple_mbus_msg(pApp, stop);
    }
    if(pRxMsg->parsed.body.setPlayState.pause) {
      _app_push_simple_mbus_msg(pApp, pause);
    }
    if(pRxMsg->parsed.body.setPlayState.play) {
      _app_push_simple_mbus_msg(pApp, play);
    }
    break;
  case MSGTYPE_setDiskTrack:
    if ((pRxMsg->parsed.body.setDiskTrack.disk != 0) && (pRxMsg->parsed.body.setDiskTrack.disk != pApp->disk)) {
      _app_push_simple_mbus_msg(pApp,
                                (appMbusSimpleRxMsgType) ((switchDisk1 - 1) + pRxMsg->parsed.body.setDiskTrack.disk));
    } else {
      int trackOffset = (pApp->track - (pRxMsg->parsed.body.setDiskTrack.track - 1)) % pApp->numTracks;
      app_debug_printf("trackOffset: %d\r\n", trackOffset);
      if (trackOffset < 0) {
        trackOffset += pApp->numTracks;
      }
      app_debug_printf("trackOffset: %d\r\n", trackOffset);
      if (trackOffset > (pApp->numTracks>>1)) {
        _app_push_simple_mbus_msg(pApp, next);
        pApp->track++;
        if (pApp->track >= pApp->numTracks) {
          pApp->track = 0;
        }
        app_debug_print("next\r\n");
      } else {
        _app_push_simple_mbus_msg(pApp, prev);
        if (trackOffset != 0) {
          if (pApp->track == 0) {
            pApp->track = pApp->numTracks - 1;
          } else {
            pApp->track--;
          }
        }
        app_debug_print("prev\r\n");
      }
    }
    break;
  // case MSGTYPE_ping:
  //   _app_push_simple_mbus_msg(pApp, ping);
  //   break;
  }
}

#define SIMPLE_MBUS_MSG_NUM 32
appMbusSimpleRxMsgType simpleMbusRxFifoMem[SIMPLE_MBUS_MSG_NUM];

#define NUM_TRACKS  16
void app_init(appStruct *pApp)
{
  fifo_nomalloc_init(&(pApp->simpleMbusMsgFifo),
                     "simpleMbusMsg",
                     simpleMbusRxFifoMem,
                     sizeof(simpleMbusRxFifoMem),
                     sizeof(appMbusSimpleRxMsgType));
  pApp->state = START;
  pApp->disk = 3;
  pApp->track = 0;
  pApp->index = 0;
  pApp->seconds = 0;
  pApp->numTracks = NUM_TRACKS;
  delay(1000);
  rn52_reconnect_last(&pApp->rn52);
}

char gMsgStr[MSG_STR_SIZE];

#define changeState(arg) do { app_debug_printf("appState: %s\r\n", #arg ); pApp->state = arg; } while (0)

void app_update(appStruct *pApp)
{
  bool gotRxMsg;
  static MbusRxMsgStruct rxMsg;
  appMbusSimpleRxMsgType simpleMbusMsg;

  if (!mbus_link_rx_is_empty(&pApp->link)) {
    mbus_link_rx_pop(&pApp->link, &rxMsg);
    mbus_link_msgToStr(&rxMsg,
                       gMsgStr,
                       MSG_STR_SIZE);
    app_debug_printf("rxmsg: %s\r\n", gMsgStr);
    app_decode_push_mbus_msg(pApp, &rxMsg);
  }

  gotRxMsg = false;
  if (!fifo_is_empty(&(pApp->simpleMbusMsgFifo))) {
    fifo_pop(&(pApp->simpleMbusMsgFifo), &simpleMbusMsg);
    app_debug_printf("simpleMbusMsg: %s\r\n", simpleMbusMsg2Str(simpleMbusMsg));
    switch (simpleMbusMsg) {
    case ping:
      mbus_link_tx_ping(&pApp->link);
      break;
    case headPowerOn:
      mbus_link_tx_cdPowerOn(&pApp->link);
      gotRxMsg = true;
      break;
    case next:
      rn52_avrcp_next(&pApp->rn52);
      break;
    case prev:
      rn52_avrcp_prev(&pApp->rn52);
      break;
    case switchDisk1:
      rn52_avrcp_play(&pApp->rn52);
      break;
    case switchDisk4:
      //WRITE_RESTART(0x5FA0004);
      rn52_reboot(&pApp->rn52);
      break;
    case switchDisk5:
      rn52_reconnect_last(&pApp->rn52);
      break;
    case switchDisk6:
      rn52_pairing(&pApp->rn52);
      break;
    // case switchDisk3:
    //   reboot();
    //   break;
    default:
      gotRxMsg = true;
      break;
    }
    // if (simpleMbusMsg == ping) {
    //   mbus_link_tx_ping(&pApp->link);
    // } else if (simpleMbusMsg == next) {
    //   rn52_avrcp_next(&pApp->rn52);
    // } else if (simpleMbusMsg == prev) {
    //   rn52_avrcp_prev(&pApp->rn52);
    // } else {
    //   gotRxMsg = true;
    // }
  }

  int trackOffset;
  switch (pApp->state) {
  case START:
    changeState(STOPPED);
    break;
  case STOPPED:
    if (gotRxMsg && simpleMbusMsg == play && (rn52_avrcp_is_playing(&pApp->rn52))) {
      changeState(START_PLAYING);
    }
    if (gotRxMsg && simpleMbusMsg == stop) {
      mbus_link_tx_playState(&pApp->link,
                             pApp->track,
                             pApp->index,
                             pApp->seconds, false);
    }
    break;
  case START_PLAYING:
    mbus_link_tx_changing(&pApp->link, pApp->disk, pApp->track);
    mbus_link_tx_ackWait(&pApp->link);

    mbus_link_tx_diskInfo(&pApp->link, pApp->disk, pApp->numTracks, pApp->seconds);
    mbus_link_tx_ackWait(&pApp->link);

    // mbus_link_tx_diskInfo(&pApp->link, pApp->disk, pApp->numTracks, pApp->seconds);
    // mbus_link_tx_ackWait(&pApp->link);

    // mbus_link_tx_cdPowerOn(&pApp->link);
    
    // mbus_link_tx_unknownStatus(&pApp->link);

    // mbus_link_tx_changing(&pApp->link, pApp->disk, pApp->track);

    mbus_link_tx_playState(&pApp->link,
                           pApp->track,
                           pApp->index,
                           pApp->seconds, true);
    
    pApp->timer = 0;
    changeState(PLAYING);
    break;
  case PLAYING:
    if (gotRxMsg && simpleMbusMsg == stop && (rn52_avrcp_is_playing(&pApp->rn52))) {
      changeState(MBUS_SAYS_STOP);
    } else if (!rn52_avrcp_is_playing(&pApp->rn52)) {
      changeState(RN52_STOPPED);
    } else if (gotRxMsg && simpleMbusMsg == play) {
      changeState(START_PLAYING);
    } else if (pApp->timer >= 1000) {

      mbus_link_tx_playState(&pApp->link,
                             pApp->track,
                             pApp->index,
                             pApp->seconds, true);
      pApp->timer = 0;
      pApp->seconds++;
    }
    break;
  case MBUS_SAYS_STOP:
    changeState(STOPPED);
    mbus_link_tx_playState(&pApp->link,
                           pApp->track,
                           pApp->index,
                           pApp->seconds, false);
    rn52_avrcp_pause(&pApp->rn52);
    break;
  case RN52_STOPPED:
    changeState(STOPPED);
    break;
  }

  // switch (pApp->state) {
  // case STOPPED:
  //   break;
  // case START_PLAYING:
  //   break;
  // case PLAYING:
  //   break;
  // }
}

