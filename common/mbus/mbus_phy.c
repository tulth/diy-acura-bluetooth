#include <stdio.h>
#include "mbus_phy.h"
#include "app_debug.h"

#define MBUS_STATE_RX_DISABLED 0
#define MBUS_STATE_RX_IDLE 1
#define MBUS_STATE_RX_AWAITING_RISE 2
#define MBUS_STATE_RX_AWAITING_FALL 3
#define MBUS_STATE_RX_NIBBLE_ENDED 4
#define MBUS_STATE_RX_FAILED_LOW 5

#define MBUS_STATE_TX_DISABLED         0
#define MBUS_STATE_TX_ENABLE           1
#define MBUS_STATE_TX_TURNAROUND_DELAY 2
#define MBUS_STATE_TX_IDLE             3
#define MBUS_STATE_TX_SENDING          4
#define MBUS_STATE_TX_SENDING_ONE      5
#define MBUS_STATE_TX_SENDING_ZERO     6
#define MBUS_STATE_TX_SEND_BIT_GAP     7
#define MBUS_STATE_TX_MSG2MSG_GAP      8
#define MBUS_STATE_TX_MSG2MSG_WAIT     9

// times in microseconds
#define BIT_TIME                 ( (unsigned long)   3100)
#define BIT_ZERO_LOW_TIME        ( (unsigned long)   675)
#define BIT_ONE_LOW_TIME         ( (unsigned long)   1890)
#define BIT_ONEVAL_THRESH_TIME   ( (unsigned long)   (BIT_ZERO_LOW_TIME + BIT_ONE_LOW_TIME) >> 1)
#define BIT_LOW_TOO_LONG_TIME    ( (unsigned long)   (BIT_TIME + BIT_ONE_LOW_TIME) >> 1)
#define NIBBLE_END_GAP_TIME      ( (unsigned long)   (BIT_TIME))
#define INTERBIT_TIMEOUT_TIME    ( (unsigned long)   (BIT_TIME))
#define TX_TURNAROUND_DELAY_TIME ( (unsigned long)   (BIT_TIME))
#define TX_MSG2MSG_WAIT          ( (unsigned long)   2 * (BIT_TIME))

// private function prototypes! 
void _mbus_phy_rx_init(MbusPhyTxRxStruct *pMbusPhyRx,
                       uint8_t *byteMemIn,
                       size_t byteMemInSize);
void _mbus_phy_rx_update(MbusPhyTxRxStruct *pMbusPhyRx,
                         const unsigned long microSecElapsed,
                         const bool mbusPinHi);
void _mbus_phy_rx_push(MbusPhyTxRxStruct *pMbusPhyRx, uint8_t byte);
void _mbus_phy_tx_init(MbusPhyTxRxStruct *pMbusPhyTx,
                       uint8_t *byteMemIn,
                       size_t byteMemInSize);
void _mbus_phy_tx_update(MbusPhyTxRxStruct *pMbusPhyTx,
                         const unsigned long microSecElapsed,
                         bool *pDriveMbusPinLo);
bool _mbus_phy_tx_is_empty(MbusPhyTxRxStruct *pMbusPhyTx);
uint8_t _mbus_phy_tx_pop(MbusPhyTxRxStruct *pMbusPhyTx);

void mbus_phy_rx_enable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->rx.state = MBUS_STATE_RX_IDLE; }
void mbus_phy_rx_disable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->rx.state = MBUS_STATE_RX_DISABLED; }
bool mbus_phy_rx_is_busy(MbusPhyStruct *pMbusPhy)
{ return (pMbusPhy->rx.state != MBUS_STATE_RX_DISABLED) && (pMbusPhy->rx.state != MBUS_STATE_RX_IDLE); }  
bool mbus_phy_rx_is_enabled(MbusPhyStruct *pMbusPhy)
{ return (pMbusPhy->rx.state != MBUS_STATE_RX_DISABLED); }  

void mbus_phy_tx_enable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->tx.state = MBUS_STATE_TX_ENABLE; }
void mbus_phy_tx_disable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->tx.state = MBUS_STATE_TX_DISABLED; }
bool mbus_phy_tx_is_busy(MbusPhyStruct *pMbusPhy)
{
  if (pMbusPhy->tx.state == MBUS_STATE_TX_DISABLED) {
    return false;
  } else if (pMbusPhy->tx.state != MBUS_STATE_TX_IDLE) {
    return true;
  } else {
    return !(_mbus_phy_tx_is_empty(&(pMbusPhy->tx)));
  }
}  
bool mbus_phy_tx_is_enabled(MbusPhyStruct *pMbusPhy)
{ return (pMbusPhy->tx.state != MBUS_STATE_TX_DISABLED); }  



char mbus_phy_rxnibble2ascii(uint8_t nib)  /* convert nibble to ascii hex */
{
  switch(nib) {
  case 0: return('0'); break;
  case 1: return('1'); break;
  case 2: return('2'); break;
  case 3: return('3'); break;
  case 4: return('4'); break;
  case 5: return('5'); break;
  case 6: return('6'); break;
  case 7: return('7'); break;
  case 8: return('8'); break;
  case 9: return('9'); break;
  case 10: return('A'); break;
  case 11: return('B'); break;
  case 12: return('C'); break;
  case 13: return('D'); break;
  case 14: return('E'); break;
  case 15: return('F'); break;
  case MBUS_LOW_TOO_LONG_CODE: return('L'); break;  // special intra-nibble timeout code
  case MBUS_TIMEOUT_CODE: return('T'); break;  // special intra-nibble timeout code
  case MBUS_END_MSG_CODE: return('\n'); break; // appears after intra-nibble timeout, or inter-nibble timeout (normal cmd end) 
  default: return('X'); break;
  }
  return(0);
}

uint8_t mbus_phy_ascii2txnibble(char ascii_char)  /* convert ascii hex to nibble */
{
  switch(ascii_char) {
  case '0': return(0); break;
  case '1': return(1); break;
  case '2': return(2); break;
  case '3': return(3); break;
  case '4': return(4); break;
  case '5': return(5); break;
  case '6': return(6); break;
  case '7': return(7); break;
  case '8': return(8); break;
  case '9': return(9); break;
  case 'A': return(10); break;
  case 'B': return(11); break;
  case 'C': return(12); break;
  case 'D': return(13); break;
  case 'E': return(14); break;
  case 'F': return(15); break;
  case '\n': return(MBUS_END_MSG_CODE); break; // appears after intra-nibble timeout, or inter-nibble timeout (normal cmd end) 
  default: return(0x80); break;
  }
  return(0);
}


// 0x08 is shifted left to eventually flag a full nibble received as 0x80 bit set
static const uint8_t RX_BIT_SHIFTER_INIT          = 0x08;
static const uint8_t RX_BIT_SHIFTER_DONE_MASK     = 0x80;
static const uint8_t TX_BIT_SHIFTER_INIT          = 0x01;
static const uint8_t TX_BIT_SHIFTER_NOT_DONE_MASK = 0x0F;

// assumes byteMemIn should be a an allocated buffer of byteMemInSize bytes
// assumes memory has already been allocated for pMbusPhy
void mbus_phy_init(MbusPhyStruct *pMbusPhy,
                   uint8_t *rxByteMemIn,
                   size_t rxByteMemInSize,
                   uint8_t *txByteMemIn,
                   size_t txByteMemInSize)
{
  _mbus_phy_rx_init(&(pMbusPhy->rx), rxByteMemIn, rxByteMemInSize);
  _mbus_phy_tx_init(&(pMbusPhy->tx), txByteMemIn, txByteMemInSize);
}

void _mbus_phy_rx_init(MbusPhyTxRxStruct *pMbusPhyRx,
                       uint8_t *rxByteMemIn,
                       size_t rxByteMemInSize
                       )
{
  fifo_nomalloc_init(&(pMbusPhyRx->byteFifo),
                     "mbusPhyRx",
                     rxByteMemIn,
                     rxByteMemInSize, 1);
  pMbusPhyRx->state = MBUS_STATE_RX_DISABLED;
  pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
  pMbusPhyRx->microSecTimeStamp = 0;
}

void _mbus_phy_tx_init(MbusPhyTxRxStruct *pMbusPhyTx,
                       uint8_t *txByteMemIn,
                       size_t txByteMemInSize
                       )
{
  fifo_nomalloc_init(&(pMbusPhyTx->byteFifo),
                     "mbusPhyTx",
                     txByteMemIn,
                     txByteMemInSize, 1);
  pMbusPhyTx->state = MBUS_STATE_TX_DISABLED;
  pMbusPhyTx->bitShifter = TX_BIT_SHIFTER_INIT;
  pMbusPhyTx->microSecTimeStamp = 0;
}

void mbus_phy_update(MbusPhyStruct *pMbusPhy, // inout
                     const unsigned long microSecElapsed,
                     const bool mbusPinHi,
                     bool *pDriveMbusPinLo  // output
                     )
{
  _mbus_phy_rx_update(&(pMbusPhy->rx), microSecElapsed, mbusPinHi);
  _mbus_phy_tx_update(&(pMbusPhy->tx), microSecElapsed, pDriveMbusPinLo);
}

/* this does a shift left first, then or's in the next RX bit */
void _mbus_phy_rx_update(MbusPhyTxRxStruct *pMbusPhyRx,
                         unsigned long microSecElapsed,
                         bool mbusPinHi)
{
  uint8_t entryState = pMbusPhyRx->state;
  unsigned long prevTimeStamp = pMbusPhyRx->microSecTimeStamp;
  //printf("state enter: %u, mbusPinHi %d\n", pMbusPhyRx->state, mbusPinHi);
  /* handle bit edges */
  switch (pMbusPhyRx->state) {
  case MBUS_STATE_RX_DISABLED:
    break;
  case MBUS_STATE_RX_IDLE:
  case MBUS_STATE_RX_NIBBLE_ENDED:
  case MBUS_STATE_RX_AWAITING_FALL:
    if (!mbusPinHi) {
      pMbusPhyRx->state = MBUS_STATE_RX_AWAITING_RISE;
      pMbusPhyRx->microSecTimeStamp = microSecElapsed;
    }
    break;
  case MBUS_STATE_RX_AWAITING_RISE:
    if (mbusPinHi) {
      pMbusPhyRx->bitShifter = pMbusPhyRx->bitShifter << 1;
      if ((microSecElapsed - pMbusPhyRx->microSecTimeStamp) > BIT_ONEVAL_THRESH_TIME) {
        // long interval, thus we rxed a "one bit"
        pMbusPhyRx->bitShifter |= 0x01; // insert the one bit, otherwise insert nothing
      }
      pMbusPhyRx->microSecTimeStamp = microSecElapsed;
      if (pMbusPhyRx->bitShifter & RX_BIT_SHIFTER_DONE_MASK) { // we got a full nibble
        _mbus_phy_rx_push(pMbusPhyRx, (pMbusPhyRx->bitShifter & 0x0F));
        pMbusPhyRx->state = MBUS_STATE_RX_NIBBLE_ENDED;
        pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
      } else {
        pMbusPhyRx->state = MBUS_STATE_RX_AWAITING_FALL;
      }
    }
    break;
  case MBUS_STATE_RX_FAILED_LOW:
    if (mbusPinHi) {
      pMbusPhyRx->state = MBUS_STATE_RX_IDLE;
    }
    break;
  }

  /* handle timeouts */
  switch (pMbusPhyRx->state) {
  case MBUS_STATE_RX_IDLE:
  case MBUS_STATE_RX_FAILED_LOW:
    return;
  case MBUS_STATE_RX_AWAITING_FALL:
    if ((microSecElapsed - pMbusPhyRx->microSecTimeStamp) > INTERBIT_TIMEOUT_TIME) {
      _mbus_phy_rx_push(pMbusPhyRx, (pMbusPhyRx->bitShifter & 0x0F));
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_TIMEOUT_CODE);
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_END_MSG_CODE);
      pMbusPhyRx->state = MBUS_STATE_RX_IDLE;
      pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
    }
    break;
  case MBUS_STATE_RX_AWAITING_RISE:
    if ((microSecElapsed - pMbusPhyRx->microSecTimeStamp) > BIT_LOW_TOO_LONG_TIME) {
      _mbus_phy_rx_push(pMbusPhyRx, (pMbusPhyRx->bitShifter & 0x0F));
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_LOW_TOO_LONG_CODE);
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_END_MSG_CODE);
      pMbusPhyRx->state = MBUS_STATE_RX_FAILED_LOW;
      pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
    }
    break;
  case MBUS_STATE_RX_NIBBLE_ENDED:
    // app_debug_printf("nibend %d\n", microSecElapsed - pMbusPhyRx->microSecTimeStamp);
    if ((microSecElapsed - pMbusPhyRx->microSecTimeStamp) > NIBBLE_END_GAP_TIME) {
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_END_MSG_CODE);
      pMbusPhyRx->state = MBUS_STATE_RX_IDLE;
      pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
      /* if (microSecElapsed & 0x3f < 5) { */
      /* } */
    }
    break;
  }
  //printf("state leave: %u\n", pMbusPhyRx->state);
#ifdef DEBUG_MBUS_PHY
  if (entryState != pMbusPhyRx->state) {
    app_debug_printf("phyrx: %u>%u, us %lu usTS %lu>%lu mbusHi %u\n", entryState, pMbusPhyRx->state, microSecElapsed, prevTimeStamp, pMbusPhyRx->microSecTimeStamp, mbusPinHi);
  }
#endif /* DEBUG_MBUS_PHY */
}


bool mbus_phy_rx_is_empty(MbusPhyStruct *pMbusPhy)
{
  return fifo_is_empty(&(pMbusPhy->rx.byteFifo));
}

// internal use only!
void _mbus_phy_rx_push(MbusPhyTxRxStruct *pMbusPhyRx, uint8_t byte)
{
  fifo_push(&(pMbusPhyRx->byteFifo), &byte);
}

// check that it is NOT empty first before calling!
uint8_t mbus_phy_rx_pop(MbusPhyStruct *pMbusPhy)
{
  uint8_t returnVal;
  fifo_pop(&(pMbusPhy->rx.byteFifo), &returnVal);
  return returnVal;
}

static inline bool _txShiftOutBit(uint8_t *shifter)
{
  bool retVal = ((*shifter & 0x80) != 0);
  *shifter = *shifter << 1;
  return retVal;
}

bool _mbus_phy_tx_is_empty(MbusPhyTxRxStruct *pMbusPhyTx)
{
  return fifo_is_empty(&(pMbusPhyTx->byteFifo));
}

bool mbus_phy_tx_is_empty(MbusPhyStruct *pMbusPhy)
{
  return fifo_is_empty(&(pMbusPhy->tx.byteFifo));
}

// check that it is NOT empty first before calling!
uint8_t _mbus_phy_tx_pop(MbusPhyTxRxStruct *pMbusPhyTx)
{
  uint8_t returnVal;
  fifo_pop(&(pMbusPhyTx->byteFifo), &returnVal);
  return returnVal;
}



/* this sends the msbit of the shifter then does a shift left */
void _mbus_phy_tx_update(MbusPhyTxRxStruct *pMbusPhyTx,
                         unsigned long microSecElapsed,
                         bool *pDriveMbusPinLo)
{
  bool outBitOne;
  uint8_t txNibble;
  uint8_t entryState = pMbusPhyTx->state;
  // app_debug_printf("state enter: %u, microSecElapsed %lu pDriveMbusPinLo %u\n", pMbusPhyTx->state, microSecElapsed, *pDriveMbusPinLo);
  
  switch (pMbusPhyTx->state) {
  case MBUS_STATE_TX_DISABLED:
    break;
  case MBUS_STATE_TX_ENABLE:
    pMbusPhyTx->microSecTimeStamp = microSecElapsed;
    pMbusPhyTx->state = MBUS_STATE_TX_TURNAROUND_DELAY;
    break;
  case MBUS_STATE_TX_TURNAROUND_DELAY:
    if ((microSecElapsed - pMbusPhyTx->microSecTimeStamp) > TX_TURNAROUND_DELAY_TIME) {
      pMbusPhyTx->state = MBUS_STATE_TX_IDLE;
    }
    break;
  case MBUS_STATE_TX_IDLE:
    *pDriveMbusPinLo = false;
    if (!_mbus_phy_tx_is_empty(pMbusPhyTx)) {
      txNibble = _mbus_phy_tx_pop(pMbusPhyTx);
#ifdef DEBUG_MBUS_PHY
      app_debug_print("phytx:");
      app_debug_putchar(mbus_phy_rxnibble2ascii(txNibble));
      app_debug_putchar('\n');
#endif /* DEBUG_MBUS_PHY */
      if (txNibble == MBUS_END_MSG_CODE) {
        pMbusPhyTx->state = MBUS_STATE_TX_MSG2MSG_GAP;
      } else { // not end of message, its a nibble to send
        pMbusPhyTx->bitShifter = TX_BIT_SHIFTER_INIT;
        pMbusPhyTx->bitShifter |= ((txNibble & 0x0F)<<4);
        pMbusPhyTx->state = MBUS_STATE_TX_SENDING;
      }
    }
    break;
  case MBUS_STATE_TX_SENDING:
    pMbusPhyTx->microSecTimeStamp = microSecElapsed;
    outBitOne = _txShiftOutBit(&(pMbusPhyTx->bitShifter));
    if (outBitOne) {
      pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ONE;
    } else {
      pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ZERO;
    }
    *pDriveMbusPinLo = true;
    break;
  case MBUS_STATE_TX_SENDING_ONE:
    if ((microSecElapsed - pMbusPhyTx->microSecTimeStamp) > BIT_ONE_LOW_TIME) {
      pMbusPhyTx->state = MBUS_STATE_TX_SEND_BIT_GAP;
      *pDriveMbusPinLo = false;
    } else {
      //printf("drive lo!\n");
      *pDriveMbusPinLo = true;
      //printf("state interim: %u, microSecElapsed %lu pDriveMbusPinLo %u\n", pMbusPhyTx->state, microSecElapsed, *pDriveMbusPinLo);
    }
    break;
  case MBUS_STATE_TX_SENDING_ZERO:
    if ((microSecElapsed - pMbusPhyTx->microSecTimeStamp) > BIT_ZERO_LOW_TIME) {
      pMbusPhyTx->state = MBUS_STATE_TX_SEND_BIT_GAP;
      *pDriveMbusPinLo = false;
    } else {
      *pDriveMbusPinLo = true;
    }
    break;
  case MBUS_STATE_TX_SEND_BIT_GAP:
    *pDriveMbusPinLo = false;
    if ((microSecElapsed - pMbusPhyTx->microSecTimeStamp) > BIT_TIME) {
      if (pMbusPhyTx->bitShifter & TX_BIT_SHIFTER_NOT_DONE_MASK) {
        pMbusPhyTx->state = MBUS_STATE_TX_SENDING;
      } else {  // done with a nibble
        pMbusPhyTx->state = MBUS_STATE_TX_IDLE;
      }
    }
    break;
  case MBUS_STATE_TX_MSG2MSG_GAP:
    pMbusPhyTx->state = MBUS_STATE_TX_MSG2MSG_WAIT;
    pMbusPhyTx->microSecTimeStamp = microSecElapsed;
    break;
  case MBUS_STATE_TX_MSG2MSG_WAIT:
    if ((microSecElapsed - pMbusPhyTx->microSecTimeStamp) > TX_MSG2MSG_WAIT) {
      pMbusPhyTx->state = MBUS_STATE_TX_IDLE;
    }
    break;
  }
  /* if (entryState != pMbusPhyTx->state) { */
  /*   app_debug_printf("phytx: %u, us %lu drvLo %u\n", pMbusPhyTx->state, microSecElapsed, *pDriveMbusPinLo); */
  /* } */
}

bool mbus_phy_tx_is_full(MbusPhyStruct *pMbusPhy)
{
  return fifo_is_full(&(pMbusPhy->tx.byteFifo));
}

void mbus_phy_tx_push(MbusPhyStruct *pMbusPhy, uint8_t nibble)
{
  fifo_push(&(pMbusPhy->tx.byteFifo), &nibble);
}
