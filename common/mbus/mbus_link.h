#ifndef FILE_MBUS_LINK_SEEN
#define FILE_MBUS_LINK_SEEN

#include <stdbool.h>
#include <stdint.h>
#include "fifo.h"

// ERROR IDS
static const uint8_t ERR_ID_SIGNAL = 0x1;
static const uint8_t ERR_ID_LENGTH = 0x2;
static const uint8_t ERR_ID_CHECKSUM = 0x3;
static const uint8_t ERR_ID_DIRECTION = 0x4;
static const uint8_t ERR_ID_UNKNOWN_TYPE = 0x5;

// MSG IDS
static const uint8_t MSGTYPE_ping = 0x10;
static const uint8_t MSGTYPE_setPlayState = 0x11;
static const uint8_t MSGTYPE_headPowerOn = 0x12;
static const uint8_t MSGTYPE_setDiskTrack = 0x13;
static const uint8_t MSGTYPE_setMode = 0x14;
static const uint8_t MSGTYPE_pong = 0x20;
static const uint8_t MSGTYPE_ackWait = 0x21;
static const uint8_t MSGTYPE_cdPowerOn = 0x22;
static const uint8_t MSGTYPE_changing = 0x23;
static const uint8_t MSGTYPE_playState = 0x24;
static const uint8_t MSGTYPE_diskInfo = 0x25;
static const uint8_t MSGTYPE_unknownStatus = 0x26;

#define NIBBLE_ARRAY_SIZE ((unsigned int)32)

typedef struct {
  uint8_t nibbles[NIBBLE_ARRAY_SIZE];
  uint8_t numNibbles;
} MbusRawNibbleListStruct;

typedef struct {
  uint64_t packNibbles;
  uint8_t numNibbles;
} MbusPackedNibblesStruct;

// HEAD2CD MSG BODY
typedef struct {
  bool resume;
  bool stop;
  bool scanStop;
  bool fastReverse;
  bool fastForward;
  bool pause;
  bool play;
} MbusMsg_setPlayState_BodyStruct;

typedef struct {
  uint8_t disk;
  uint8_t track;
  bool pause;
  bool play;
  bool random;
} MbusMsg_setDiskTrack_BodyStruct;

typedef struct {
  bool repeatAll;
  bool repeatOne;
  bool introScan;
  bool random;
} MbusMsg_setMode_BodyStruct;

typedef struct {
  uint8_t disk;
  uint8_t track;
  bool eject;
  bool noshuttle;
  bool busy;
  bool repeatAll;
  bool repeatOne;
  bool random;
  bool done;
} MbusMsg_changing_BodyStruct;

typedef struct {
  uint8_t track;
  uint8_t index;
  uint8_t minute;
  uint8_t second;
  bool repeatAll;
  bool repeatOne;
  bool introScan;
  bool random;
  bool stopped;
  bool paused;
  bool play;
} MbusMsg_playState_BodyStruct;

typedef struct {
  uint8_t disk;
  uint8_t tracks;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t flags;
} MbusMsg_diskInfo_BodyStruct;

typedef union {
  MbusMsg_setPlayState_BodyStruct setPlayState;
  MbusMsg_setDiskTrack_BodyStruct setDiskTrack;
  MbusMsg_setMode_BodyStruct setMode;
  MbusMsg_changing_BodyStruct changing;
  MbusMsg_playState_BodyStruct playState;
  MbusMsg_diskInfo_BodyStruct diskInfo;
} MbusMsgBodyUnion;

typedef struct {
  bool directionH2C;
  uint8_t msgType;
  MbusMsgBodyUnion body;
} MbusMsgParsedStruct;

typedef struct {
  MbusPackedNibblesStruct nibbles;
} MbusTxMsgStruct;

typedef struct {
  uint8_t errId;
  MbusMsgParsedStruct parsed;
  MbusRawNibbleListStruct rawNibbles;
} MbusRxMsgStruct;

typedef struct {
  fifo rxMsgFifo;
  fifo txMsgFifo;
  MbusRawNibbleListStruct nibbles;
  bool rxNotTxMode;
  fifo *phyTxNibbleFifo;
} MbusLinkStruct;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern void mbus_link_init(MbusLinkStruct *pMbusLink,
                             MbusRxMsgStruct *rxMsgMemIn,
                             size_t rxMsgMemInSize,
                             MbusTxMsgStruct *txMsgMemIn,
                             size_t txMsgMemInSize,
                             fifo *phyTxNibbleFifo);
  extern void mbus_link_update(MbusLinkStruct *pMbusLink,
                               const bool mbusPhyRxBusy,
                               const bool mbusPhyTxBusy,
                               bool *phyDirectionUpdated);
  /* FIXME add tx capability */
  extern bool mbus_link_rx_is_empty(MbusLinkStruct *pMbusLink);
  extern void mbus_link_rx_pop(MbusLinkStruct *pMbusLink, MbusRxMsgStruct *pMbusMsgOut);
  extern void mbus_link_rx_push_nibble(MbusLinkStruct *pMbusLink,
                                       const uint8_t rxNibble);
  extern bool mbus_link_tx_is_empty(MbusLinkStruct *pMbusLink);
  extern bool mbus_link_tx_is_full(MbusLinkStruct *pMbusLink);
  extern void mbus_link_tx_push(MbusLinkStruct *pMbusLink, MbusTxMsgStruct *pMbusMsgIn);
  extern void mbus_link_parseMsg(uint8_t* nibbleSequence, unsigned int numNibbles, MbusRxMsgStruct *pMbusMsgOut);
  extern int mbus_link_msgToStr(MbusRxMsgStruct *pMbusMsgIn,
                                char *strOut,
                                unsigned int strOutMaxSize);
  static inline bool mbus_link_is_direction_rx(MbusLinkStruct *pMbusLink) { return pMbusLink->rxNotTxMode; }
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_MBUS_LINK_SEEN
