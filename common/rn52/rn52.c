#include "rn52.h"
#include "fifo.h"
#include "app_debug.h"

typedef enum {
  STARTING_UP = 1,
  REBOOT,
  READ_INITIAL_CONSTAT,
  READ_EXTFEAT,
  CHECK_EXTFEAT,
  SET_EXTFEAT,
  RUNNING,
  SWITCH_TO_CMD_MODE,
  SWITCH_TO_CMD_DELAY_HI,
  SWITCH_TO_CMD_DELAY_LO,
  SWITCH_TO_CMD_AWAIT_RESP,
  ACTION_OR_SET_CMD,
  ACTION_OR_SET_CMD_AWAIT_RESP,
  QUERY_CONSTAT,
  QUERY_CONSTAT_AWAIT_RESP,
  QUERY_EXTFEAT,
  QUERY_EXTFEAT_AWAIT_RESP
} rn52StateType;

bool _rn52_avrcp_cmd_fifo_is_empty(Rn52Struct *pRn52);
bool _rn52_avrcp_cmd_fifo_is_full(Rn52Struct *pRn52);
void _rn52_avrcp_cmd_fifo_push(Rn52Struct *pRn52, uint8_t avrcpCmd);
uint8_t _rn52_avrcp_cmd_fifo_pop(Rn52Struct *pRn52);

void rn52_init(Rn52Struct *pRn52,
               int (*pFuncSerialAvailable)(void),
               int (*pFuncSerialGetChar)(void),
               void (*pFuncSerialPutChar)(uint32_t c),
               uint8_t *avrcpCmdFifoMem,
               size_t avrcpCmdFifoMemSize
               )
{
  pRn52->state = STARTING_UP;
  pRn52->modeCmdNotData = false;
  pRn52->connectionStatus = 0;
  pRn52->returnState = STARTING_UP;
  pRn52->rxStrLen = 0;
  pRn52->rxStr[0] = '\0';
  pRn52->txCmd[0] = '\0';
  pRn52->replyMilliSecTimeStamp = 0;
  pRn52->cmd2CmdMilliSecTimeStamp = 0;
  pRn52->pFuncSerialAvailable = pFuncSerialAvailable;
  pRn52->pFuncSerialGetChar = pFuncSerialGetChar;
  pRn52->pFuncSerialPutChar = pFuncSerialPutChar;
  fifo_nomalloc_init(&(pRn52->avrcpCmdFifo),
                     "rn52AvrcpCmdFifo",
                     avrcpCmdFifoMem,
                     avrcpCmdFifoMemSize, 1);
}

static inline char nibble2HexChar(uint16_t nibbleArg)
{
  if (nibbleArg < 0xA) {
    return nibbleArg + '0';
  } 
  if (nibbleArg <= 0xF) {
    return nibbleArg + 'A';
  }
  return 'X';
}

static inline uint8_t hexChar2Nibble(char charArg)
{
  if (charArg >= '0' && charArg <= '9') {
    return (uint8_t)(charArg - '0');
  }
  if (charArg >= 'a' && charArg <= 'f') {
    return (uint8_t)(charArg - 'a' + 10);
  }
  if (charArg >= 'A' && charArg <= 'F'){
    return (uint8_t)(charArg - 'A' + 10);
  }
  return 0xff;
}

void _doAvrCpCmdIfAvailable(Rn52Struct *pRn52)
{
  uint8_t avrcpCmd;
  if (!_rn52_avrcp_cmd_fifo_is_empty(pRn52)) {
    avrcpCmd = _rn52_avrcp_cmd_fifo_pop(pRn52);
    if (avrcpCmd == RN52_AVRCP_CMD_RECONNECT_LAST) {
      strcpy(pRn52->txCmd, "B");
      pRn52->state = ACTION_OR_SET_CMD;
      pRn52->returnState = RUNNING;
    } else if (avrcpCmd == RN52_AVRCP_CMD_REBOOT) {
      strcpy(pRn52->txCmd, "R,1");
      pRn52->state = ACTION_OR_SET_CMD;
      pRn52->returnState = RUNNING;
    } else if (avrcpCmd == RN52_AVRCP_CMD_PAIRING) {
      strcpy(pRn52->txCmd, "@,1");
      pRn52->state = ACTION_OR_SET_CMD;
      pRn52->returnState = RUNNING;
    } else if (rn52_is_connected(pRn52)) {
      switch (avrcpCmd) {
      case RN52_AVRCP_CMD_PLAY:
        if (!rn52_avrcp_is_playing(pRn52)) {
          strcpy(pRn52->txCmd, "AP");
        }
        break;
      case RN52_AVRCP_CMD_PAUSE:
        if (rn52_avrcp_is_playing(pRn52)) {
          strcpy(pRn52->txCmd, "AP");
        }
        break;
      case RN52_AVRCP_CMD_NEXT:
        strcpy(pRn52->txCmd, "AT+");
        break;
      case RN52_AVRCP_CMD_PREV:
        strcpy(pRn52->txCmd, "AT-");
        break;
      case RN52_AVRCP_CMD_PLAYPAUSE:
        strcpy(pRn52->txCmd, "AP");
        break;
      }
      pRn52->state = ACTION_OR_SET_CMD;
      pRn52->returnState = RUNNING;
    } else { // drop it
      pRn52->state = RUNNING;
    }
  }
}

  /* case RECONNECT: */
  /*   idx=0; */
  /*   pRn52->txCmd[idx++] = 'B'; */
  /*   pRn52->txCmd[idx++] = '\0'; */
  /*   pRn52->state = ACTION_OR_SET_CMD; */
  /*   pRn52->returnState = RECONNECT_CHECK; */
  /*   break; */
  /* case RECONNECT_CHECK: */
  /*   pRn52->state = QUERY_CONSTAT; */
  /*   pRn52->returnState = RUNNING; */
  /*   break; */
  

bool _rn52_is_cmd_reboot(Rn52Struct *pRn52)
{
  if (pRn52->txCmd[0] != 'R') { return false; }
  if (pRn52->txCmd[1] != ',') { return false; }
  if (pRn52->txCmd[2] != '1') { return false; }
  return (strncmp(pRn52->txCmd, "R,1", 3));
}

void rn52_update(Rn52Struct *pRn52,
                 unsigned long milliSecElapsed,
                 bool eventStatePin,
                 bool *pCmdPin)
{
  char *pTxChar;
  int idx;
  char rxC;
  uint16_t extFeat;
  uint8_t entryState = pRn52->state;
  
  if (pRn52->pFuncSerialAvailable()) {
    rxC = pRn52->pFuncSerialGetChar();
    pRn52->rxStr[pRn52->rxStrLen] = rxC;
#ifdef DEBUG_RN52
    /* app_debug_printf("r"); */
    /* app_debug_putchar(rxC); */
    /* app_debug_printf("%d\r\n", pRn52->rxStrLen); */
#endif /* DEBUG_RN52 */
    pRn52->rxStrLen++;
  }
  switch (pRn52->state) {
  case STARTING_UP:
    pRn52->state = SWITCH_TO_CMD_MODE;
    pRn52->returnState = REBOOT;
    break;
  case REBOOT:
    idx=0;
    pRn52->txCmd[idx++] = 'R';
    pRn52->txCmd[idx++] = ',';
    pRn52->txCmd[idx++] = '1';
    pRn52->txCmd[idx++] = '\0';
    pRn52->state = ACTION_OR_SET_CMD;
    pRn52->returnState = READ_INITIAL_CONSTAT;
    break;
  case READ_INITIAL_CONSTAT:
    pRn52->state = QUERY_CONSTAT;
    pRn52->returnState = READ_EXTFEAT;
    pRn52->eventLoMilliSecTimeStamp = milliSecElapsed;  /* clear this */
    break;
  case READ_EXTFEAT:
    pRn52->state = QUERY_EXTFEAT;
    pRn52->returnState = CHECK_EXTFEAT;
    break;
  case CHECK_EXTFEAT:
    if (pRn52->extFeat != DEFAULT_EXTFEAT) {
      app_debug_printf("mismatch extfeat! DEFAULT_EXTFEAT:0x%04X extFeat:0x%04X\r\n",
                       DEFAULT_EXTFEAT, pRn52->extFeat);
      pRn52->state = SET_EXTFEAT;
    } else {
      pRn52->state = RUNNING;
    }
    break;
  case SET_EXTFEAT:
    idx=0;
    pRn52->txCmd[idx++] = 's';
    pRn52->txCmd[idx++] = '%';
    pRn52->txCmd[idx++] = ',';
    pRn52->txCmd[idx++] = nibble2HexChar(DEFAULT_EXTFEAT >> 12);
    pRn52->txCmd[idx++] = nibble2HexChar((DEFAULT_EXTFEAT >> 8) & 0xF);
    pRn52->txCmd[idx++] = nibble2HexChar((DEFAULT_EXTFEAT >> 4) & 0xF);
    pRn52->txCmd[idx++] = nibble2HexChar((DEFAULT_EXTFEAT >> 0) & 0xF);
    pRn52->txCmd[idx++] = '\0';
    pRn52->state = ACTION_OR_SET_CMD;
    pRn52->returnState = STARTING_UP;
    break;
  case RUNNING:
    if (!pRn52->modeCmdNotData) {  /* FIXME: for now force cmd mode */
      pRn52->state = SWITCH_TO_CMD_MODE;
      pRn52->returnState = RUNNING;
      /* read pending event if its new (still low after 100 ms) */
    } else if ((!eventStatePin) && (milliSecElapsed - pRn52->eventLoMilliSecTimeStamp > 105)) {
      pRn52->eventLoMilliSecTimeStamp = milliSecElapsed;
      pRn52->state = QUERY_CONSTAT;
      pRn52->returnState = RUNNING;
    } else {  /* far end is not connected */
      _doAvrCpCmdIfAvailable(pRn52);
    }
    break;
  case SWITCH_TO_CMD_MODE:
    *pCmdPin = true;
    pRn52->replyMilliSecTimeStamp = milliSecElapsed;
    pRn52->state = SWITCH_TO_CMD_DELAY_HI;
    pRn52->rxStrLen = 0;
    break;
  case SWITCH_TO_CMD_DELAY_HI:
    if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > 500) {
      pRn52->state = SWITCH_TO_CMD_DELAY_LO;
      *pCmdPin = false;
      pRn52->replyMilliSecTimeStamp = milliSecElapsed;
      pRn52->rxStrLen = 0;
    }
    break;
  case SWITCH_TO_CMD_DELAY_LO:
    if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > 100) {
      pRn52->state = SWITCH_TO_CMD_AWAIT_RESP;
      *pCmdPin = false;
      pRn52->replyMilliSecTimeStamp = milliSecElapsed;
    }
    break;
  case SWITCH_TO_CMD_AWAIT_RESP:
    if (pRn52->rxStrLen >= 5) {
      if (strcmp(pRn52->rxStr, "CMD\r\n")) {
        /* got good cmd response, return */
        pRn52->state = pRn52->returnState;
        pRn52->rxStrLen = 0;
        pRn52->modeCmdNotData = true;
      } else {
        /* bad response, try again */
        pRn52->state = SWITCH_TO_CMD_MODE;
        pRn52->rxStrLen = 0;
      }
    } else if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
#ifdef DEBUG_RN52
      app_debug_printf("timeout-s2car\r\n");
#endif /* DEBUG_RN52 */
      pRn52->state = SWITCH_TO_CMD_MODE;
      pRn52->rxStrLen = 0;
    }
    break;
  case ACTION_OR_SET_CMD:
    pRn52->replyMilliSecTimeStamp = milliSecElapsed;
    pTxChar = pRn52->txCmd;
#ifdef DEBUG_RN52
    app_debug_printf("rn52tx:%s\r\n", pRn52->txCmd);
#endif /* DEBUG_RN52 */
    while (*pTxChar != '\0') {
      pRn52->pFuncSerialPutChar(*pTxChar);
      pTxChar++;
    }
    pRn52->pFuncSerialPutChar('\r');
    pRn52->pFuncSerialPutChar('\n');
    pRn52->state = ACTION_OR_SET_CMD_AWAIT_RESP;
    pRn52->rxStrLen = 0;
    break;
  case ACTION_OR_SET_CMD_AWAIT_RESP:
    if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = ACTION_OR_SET_CMD;
      pRn52->rxStrLen = 0;
    } else if (_rn52_is_cmd_reboot(pRn52)) {
      if (pRn52->rxStrLen >= 9) {
        if (strcmp(pRn52->rxStr, "Reboot!\r\n")) {
          /* got good response, return */
          pRn52->state = pRn52->returnState;
          pRn52->rxStrLen = 0;
        } else {
          /* bad response, try again */
          pRn52->state = ACTION_OR_SET_CMD;
          pRn52->rxStrLen = 0;
        }
      }
    } else { // not a timeout or special reply command
      if (pRn52->rxStrLen >= 5) {
        if (strcmp(pRn52->rxStr, "AOK\r\n")) {
          /* got good response, return */
          pRn52->state = pRn52->returnState;
          pRn52->rxStrLen = 0;
        } else {
          /* bad response, try again */
          pRn52->state = ACTION_OR_SET_CMD;
          pRn52->rxStrLen = 0;
        }
      }
    }
    break;
  case QUERY_CONSTAT:
    pRn52->replyMilliSecTimeStamp = milliSecElapsed;
#ifdef DEBUG_RN52
    app_debug_printf("rn52tx:Q\r\n", pRn52->txCmd);
#endif /* DEBUG_RN52 */
    pRn52->pFuncSerialPutChar('Q');
    pRn52->pFuncSerialPutChar('\r');
    pRn52->pFuncSerialPutChar('\n');
    pRn52->state = QUERY_CONSTAT_AWAIT_RESP;
    break;
  case QUERY_CONSTAT_AWAIT_RESP:
    if ((pRn52->rxStrLen > 0) && pRn52->rxStr[pRn52->rxStrLen-1] == '\n') {
#ifdef DEBUG_RN52
      app_debug_print("rn52rx:");
      for (idx = 0; idx < pRn52->rxStrLen-1; idx++) {
        app_debug_putchar(pRn52->rxStr[idx]);
      }
      app_debug_putchar('\r');
      app_debug_putchar('\n');
#endif /* DEBUG_RN52 */
      if (pRn52->rxStrLen == 6) {
        /* got good response, return */
        pRn52->connectionStatus = 0;
        for (idx = 0; idx < 4; idx++) {
          pRn52->connectionStatus = pRn52->connectionStatus << 4;
          pRn52->connectionStatus |= hexChar2Nibble(pRn52->rxStr[idx]);
        }
#ifdef DEBUG_RN52
        app_debug_printf("rn52conStat:0x%04X\r\n", pRn52->connectionStatus);
#endif /* DEBUG_RN52 */
        pRn52->rxStrLen = 0;
        pRn52->state = pRn52->returnState;
      } else {
        /* bad response, try again */
        pRn52->state = QUERY_CONSTAT;
        pRn52->rxStrLen = 0;
#ifdef DEBUG_RN52
        app_debug_printf("rn52conStat:bad response\r\n", pRn52->connectionStatus);
#endif /* DEBUG_RN52 */
      }
    } else if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = QUERY_CONSTAT;
      pRn52->rxStrLen = 0;
#ifdef DEBUG_RN52
      app_debug_printf("rn52conStat:timeout\r\n", pRn52->connectionStatus);
#endif /* DEBUG_RN52 */
    }
    break;
  case QUERY_EXTFEAT:
    pRn52->replyMilliSecTimeStamp = milliSecElapsed;
#ifdef DEBUG_RN52
    app_debug_printf("rn52tx:G%\r\n", pRn52->txCmd);
#endif /* DEBUG_RN52 */
    pRn52->pFuncSerialPutChar('G');
    pRn52->pFuncSerialPutChar('%');
    pRn52->pFuncSerialPutChar('\r');
    pRn52->pFuncSerialPutChar('\n');
    pRn52->state = QUERY_EXTFEAT_AWAIT_RESP;
    break;
  case QUERY_EXTFEAT_AWAIT_RESP:
    if ((pRn52->rxStrLen > 0) && pRn52->rxStr[pRn52->rxStrLen-1] == '\n') {
#ifdef DEBUG_RN52
      app_debug_print("rn52rx:");
      for (idx = 0; idx < pRn52->rxStrLen-1; idx++) {
        app_debug_putchar(pRn52->rxStr[idx]);
      }
      app_debug_putchar('\r');
      app_debug_putchar('\n');
#endif /* DEBUG_RN52 */
      if (pRn52->rxStrLen == 6) {
        /* got good response, return */
        pRn52->extFeat = 0;
        for (idx = 0; idx < 4; idx++) {
          pRn52->extFeat = pRn52->extFeat << 4;
          pRn52->extFeat |= hexChar2Nibble(pRn52->rxStr[idx]);
        }
#ifdef DEBUG_RN52
        app_debug_printf("rn52extFeat:0x%04X\r\n", pRn52->extFeat);
#endif /* DEBUG_RN52 */
        pRn52->rxStrLen = 0;
        pRn52->state = pRn52->returnState;
      } else {
        /* bad response, try again */
        pRn52->state = QUERY_EXTFEAT;
        pRn52->rxStrLen = 0;
      }
    } else if ((milliSecElapsed - pRn52->replyMilliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = QUERY_EXTFEAT;
      pRn52->rxStrLen = 0;
    }
    break;
  }
#ifdef DEBUG_RN52
  if (entryState != pRn52->state) {
    app_debug_printf("rn52_st: %d>%d\r\n", entryState, pRn52->state);
  }
#endif /* DEBUG_RN52 */
}

void rn52_rx_char(Rn52Struct *pRn52, char newChar)
{
}

bool rn52_is_connected(Rn52Struct *pRn52)
{
  return pRn52->connectionStatus & RN52_CONSTAT_A2DP;
}

void rn52_avrcp_play(Rn52Struct *pRn52)
{
  if (rn52_is_connected(pRn52)) {
    _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_PLAY);
  }
}

void rn52_avrcp_pause(Rn52Struct *pRn52)
{
  if (rn52_is_connected(pRn52)) {
    _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_PAUSE);
  }
}

void rn52_avrcp_next(Rn52Struct *pRn52)
{
  if (rn52_is_connected(pRn52)) {
    _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_NEXT);
  }
}

void rn52_avrcp_prev(Rn52Struct *pRn52)
{
  if (rn52_is_connected(pRn52)) {
    _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_PREV);
  }
}

void rn52_avrcp_playpause(Rn52Struct *pRn52)
{
  if (rn52_is_connected(pRn52)) {
    _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_PLAYPAUSE);
  }
}

void rn52_reconnect_last(Rn52Struct *pRn52)
{
  _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_RECONNECT_LAST);
}

void rn52_pairing(Rn52Struct *pRn52)
{
  _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_PAIRING);
}

void rn52_reboot(Rn52Struct *pRn52)
{
  _rn52_avrcp_cmd_fifo_push(pRn52, RN52_AVRCP_CMD_REBOOT);
}

bool _rn52_avrcp_cmd_fifo_is_full(Rn52Struct *pRn52)
{
  return fifo_is_full(&(pRn52->avrcpCmdFifo));
}

bool _rn52_avrcp_cmd_fifo_is_empty(Rn52Struct *pRn52)
{
  return fifo_is_empty(&(pRn52->avrcpCmdFifo));
}

void _rn52_avrcp_cmd_fifo_push(Rn52Struct *pRn52, uint8_t avrcpCmd)
{
  if (!_rn52_avrcp_cmd_fifo_is_full(pRn52)) {
    fifo_push(&(pRn52->avrcpCmdFifo), &avrcpCmd);
  }
}

uint8_t _rn52_avrcp_cmd_fifo_pop(Rn52Struct *pRn52)
{
  uint8_t returnVal;
  fifo_pop(&(pRn52->avrcpCmdFifo), &returnVal);
  return returnVal;
}

bool rn52_avrcp_is_playing(Rn52Struct *pRn52)
{
  bool connectedA2dp = pRn52->connectionStatus & RN52_CONSTAT_A2DP;
  bool audioStreaming = (pRn52->connectionStatus & 0x000F) == RN52_CONSTAT_CONSTATE_AUDIO_STREAMING;
/* #ifdef DEBUG_RN52 */
/*   app_debug_printf("rn52conStat:0x%04X connected %d audiostrm %d\r\n", pRn52->connectionStatus, connectedA2dp, audioStreaming); */
/* #endif /\* DEBUG_RN52 *\/ */

  // phone seems to toggle ad2p very frequently, for example for pauses
  // the audio system pops when this happens - it is unclear why this happens
  // so just always say its streaming while on to avoid pops
  // return connectedA2dp && audioStreaming;
  return true;
}
