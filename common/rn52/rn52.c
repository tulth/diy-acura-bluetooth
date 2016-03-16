#include "rn52.h"
#include "fifo.h"
#include "app_debug.h"

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
  pRn52->state = RN52_STATE_STARTING_UP;
  pRn52->modeCmdNotData = false;
  pRn52->connectionStatus = 0;
  pRn52->returnState = RN52_STATE_STARTING_UP;
  pRn52->rxStrLen = 0;
  pRn52->rxStr[0] = '\0';
  pRn52->txCmd[0] = '\0';
  pRn52->milliSecTimeStamp = 0;
  pRn52->pFuncSerialAvailable = pFuncSerialAvailable;
  pRn52->pFuncSerialGetChar = pFuncSerialGetChar;
  pRn52->pFuncSerialPutChar = pFuncSerialPutChar;
  fifo_nomalloc_init(&(pRn52->avrcpCmdFifo),
                     avrcpCmdFifoMem,
                     avrcpCmdFifoMemSize, 1);
}

void _rn52_cmd(Rn52Struct *pRn52,
               char *cmd,
               unsigned long milliSecElapsed,
               uint8_t returnState)
{
  
}

  
void rn52_update(Rn52Struct *pRn52,
                 unsigned long milliSecElapsed,
                 bool eventStatePin,
                 bool *pCmdPin)
{
  uint8_t avrcpCmd;
  char *pTxChar;
  int idx;
  uint8_t entryState = pRn52->state;
  
  if (pRn52->pFuncSerialAvailable()) {
    pRn52->rxStr[pRn52->rxStrLen] = pRn52->pFuncSerialGetChar();
    pRn52->rxStrLen++;
  }
  switch (pRn52->state) {
  case RN52_STATE_STARTING_UP:
    pRn52->state = RN52_STATE_SWITCH_TO_CMD_MODE;
    pRn52->returnState = RN52_STATE_STARTING_UP2;
    break;
  case RN52_STATE_STARTING_UP2:
    pRn52->state = RN52_STATE_QUERY_CONSTAT;
    break;
  case RN52_STATE_DISCONNECT:
    if (!pRn52->modeCmdNotData) {  /* FIXME: for now force cmd mode */
      pRn52->state = RN52_STATE_SWITCH_TO_CMD_MODE;
      pRn52->returnState = RN52_STATE_CONNECT;
    } else if (!eventStatePin) {  /* read pending event */
      pRn52->state = RN52_STATE_QUERY_CONSTAT;
    } else {
      strcpy(pRn52->txCmd,"B");
      pRn52->state = RN52_STATE_ACTION_CMD;
      pRn52->returnState = RN52_STATE_DISCONNECT;
    }
    break;
  case RN52_STATE_CONNECT:
    if (!pRn52->modeCmdNotData) {  /* FIXME: for now force cmd mode */
      pRn52->state = RN52_STATE_SWITCH_TO_CMD_MODE;
      pRn52->returnState = RN52_STATE_CONNECT;
    } else if (!eventStatePin) {  /* read pending event */
      pRn52->state = RN52_STATE_QUERY_CONSTAT;
    } else {
      if (_rn52_avrcp_cmd_fifo_is_empty(pRn52)) {
        avrcpCmd = _rn52_avrcp_cmd_fifo_pop(pRn52);
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
        pRn52->state = RN52_STATE_ACTION_CMD;
        pRn52->returnState = RN52_STATE_CONNECT;
      }
    }
    break;
  case RN52_STATE_SWITCH_TO_CMD_MODE:
    *pCmdPin = false;
    pRn52->milliSecTimeStamp = milliSecElapsed;
    pRn52->state = RN52_STATE_SWITCH_TO_CMD_DELAY;
    pRn52->rxStrLen = 0;
    break;
  case RN52_STATE_SWITCH_TO_CMD_DELAY:
    if ((milliSecElapsed - pRn52->milliSecTimeStamp) > 100) {
      pRn52->state = RN52_STATE_SWITCH_TO_CMD_AWAIT_RESP;
      *pCmdPin = true;
      pRn52->milliSecTimeStamp = milliSecElapsed;
    }
    break;
  case RN52_STATE_SWITCH_TO_CMD_AWAIT_RESP:
    if (pRn52->rxStrLen >= 5) {
      if (strcmp(pRn52->rxStr, "CMD\r\n")) {
        /* got good cmd response, return */
        pRn52->state = pRn52->returnState;
        pRn52->rxStrLen = 0;
        pRn52->modeCmdNotData = true;
      } else {
        /* bad response, try again */
        pRn52->state = RN52_STATE_SWITCH_TO_CMD_MODE;
        pRn52->rxStrLen = 0;
      }
    } else if ((milliSecElapsed - pRn52->milliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = RN52_STATE_SWITCH_TO_CMD_MODE;
      pRn52->rxStrLen = 0;
    }
    break;
  case RN52_STATE_ACTION_CMD:
    pTxChar = pRn52->txCmd;
    while (pTxChar != '\0') {
      pRn52->pFuncSerialPutChar(*pTxChar);
      pTxChar++;
    }
    pRn52->pFuncSerialPutChar('\r');
    pRn52->pFuncSerialPutChar('\n');
    pRn52->state = RN52_STATE_ACTION_CMD_AWAIT_RESP;
    pRn52->rxStrLen = 0;
    break;
  case RN52_STATE_ACTION_CMD_AWAIT_RESP:
    if (pRn52->rxStrLen >= 5) {
      if (strcmp(pRn52->rxStr, "AOK\r\n")) {
        /* got good response, return */
        pRn52->state = pRn52->returnState;
        pRn52->rxStrLen = 0;
      } else {
        /* bad response, try again */
        pRn52->state = RN52_STATE_ACTION_CMD;
        pRn52->rxStrLen = 0;
      }
    } else if ((milliSecElapsed - pRn52->milliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = RN52_STATE_ACTION_CMD;
      pRn52->rxStrLen = 0;
    }
    break;
  case RN52_STATE_QUERY_CONSTAT:     
    pRn52->pFuncSerialPutChar('Q');
    pRn52->pFuncSerialPutChar('\r');
    pRn52->pFuncSerialPutChar('\n');
    break;
  case RN52_STATE_QUERY_CONSTAT_AWAIT_RESP:
    if (pRn52->rxStrLen >= 6) {
      if ((pRn52->rxStr[4] == '\r') && (pRn52->rxStr[5] == '\n')) {
        /* got good response, return */
        pRn52->connectionStatus = 0;
        for (idx = 0; idx < 4; idx++) {
          pRn52->connectionStatus = pRn52->connectionStatus << 4;
          pRn52->connectionStatus |= (pRn52->rxStr[idx] - '0');
        }
        if (rn52_is_connected(pRn52)) {
          pRn52->state = RN52_STATE_CONNECT;
        } else {
          pRn52->state = RN52_STATE_DISCONNECT;
        }
        pRn52->rxStrLen = 0;
      } else {
        /* bad response, try again */
        pRn52->state = RN52_STATE_QUERY_CONSTAT;
        pRn52->rxStrLen = 0;
      }
    } else if ((milliSecElapsed - pRn52->milliSecTimeStamp) > RN52_CMD_TIMEOUT_MILLSEC) {
      /* timed out, try again */
      pRn52->state = RN52_STATE_QUERY_CONSTAT;
      pRn52->rxStrLen = 0;
    }
    break;
  }
  if (entryState != pRn52->state) {
    app_debug_printf("rn52_st: %d>%d", entryState, pRn52->state);
  }
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
  bool audioStreaming = ((pRn52->connectionStatus & 0x0f00) >> 8) == 13;
  return connectedA2dp && audioStreaming;
}
