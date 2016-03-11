#include "mbus_phy.h"
#include "stdio.h"

#define MBUS_STATE_RX_DISABLED 0
#define MBUS_STATE_RX_IDLE 1
#define MBUS_STATE_RX_AWAITING_RISE 2
#define MBUS_STATE_RX_AWAITING_FALL 3
#define MBUS_STATE_RX_NIBBLE_ENDED 4
#define MBUS_STATE_RX_FAILED_LOW 5

#define MBUS_STATE_TX_DISABLED 0
#define MBUS_STATE_TX_IDLE 1
#define MBUS_STATE_TX_SENDING_ONE 2
#define MBUS_STATE_TX_SENDING_ZERO 3
#define MBUS_STATE_TX_SEND_BIT_GAP 4

// times in microseconds
#define BIT_TIME               ( (unsigned long)   3100)
#define BIT_ZERO_LOW_TIME      ( (unsigned long)   675)
#define BIT_ONE_LOW_TIME       ( (unsigned long)   1890)
#define BIT_ONEVAL_THRESH_TIME ( (unsigned long)   (BIT_ZERO_LOW_TIME + BIT_ONE_LOW_TIME) >> 1)
#define BIT_LOW_TOO_LONG_TIME  ( (unsigned long)   (BIT_TIME + BIT_ONE_LOW_TIME) >> 1)
#define NIBBLE_END_GAP_TIME    ( (unsigned long)   (BIT_TIME))
#define INTERBIT_TIMEOUT_TIME  ( (unsigned long)   (BIT_TIME))

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
{ return (pMbusPhy->rx.state != MBUS_STATE_RX_DISABLED) || (pMbusPhy->rx.state != MBUS_STATE_RX_IDLE); }  
bool mbus_phy_rx_is_enabled(MbusPhyStruct *pMbusPhy)
{ return (pMbusPhy->rx.state != MBUS_STATE_RX_DISABLED); }  

void mbus_phy_tx_enable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->tx.state = MBUS_STATE_TX_IDLE; }
void mbus_phy_tx_disable(MbusPhyStruct *pMbusPhy)
{ pMbusPhy->tx.state = MBUS_STATE_TX_DISABLED; }
bool mbus_phy_tx_is_busy(MbusPhyStruct *pMbusPhy)
{
  if (pMbusPhy->tx.state == MBUS_STATE_TX_DISABLED) {
    return true;
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
  circular_buffer_nomalloc_init(&(pMbusPhyRx->byteFifo),
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
  circular_buffer_nomalloc_init(&(pMbusPhyTx->byteFifo),
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
    if ((microSecElapsed - pMbusPhyRx->microSecTimeStamp) > NIBBLE_END_GAP_TIME) {
      _mbus_phy_rx_push(pMbusPhyRx, MBUS_END_MSG_CODE);
      pMbusPhyRx->state = MBUS_STATE_RX_IDLE;
      pMbusPhyRx->bitShifter = RX_BIT_SHIFTER_INIT;
    }
    break;
  }
  //printf("state leave: %u\n", pMbusPhyRx->state);
}


bool mbus_phy_rx_is_empty(MbusPhyStruct *pMbusPhy)
{
  return circular_buffer_is_empty(&(pMbusPhy->rx.byteFifo));
}

// internal use only!
void _mbus_phy_rx_push(MbusPhyTxRxStruct *pMbusPhyRx, uint8_t byte)
{
  circular_buffer_push(&(pMbusPhyRx->byteFifo), &byte);
}

// check that it is NOT empty first before calling!
uint8_t mbus_phy_rx_pop(MbusPhyStruct *pMbusPhy)
{
  uint8_t returnVal;
  circular_buffer_pop(&(pMbusPhy->rx.byteFifo), &returnVal);
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
  return circular_buffer_is_empty(&(pMbusPhyTx->byteFifo));
}

bool mbus_phy_tx_is_empty(MbusPhyStruct *pMbusPhy)
{
  return circular_buffer_is_empty(&(pMbusPhy->tx.byteFifo));
}

// check that it is NOT empty first before calling!
uint8_t _mbus_phy_tx_pop(MbusPhyTxRxStruct *pMbusPhyTx)
{
  uint8_t returnVal;
  circular_buffer_pop(&(pMbusPhyTx->byteFifo), &returnVal);
  return returnVal;
}



/* this sends the msbit of the shifter then does a shift left */
void _mbus_phy_tx_update(MbusPhyTxRxStruct *pMbusPhyTx,
                         unsigned long microSecElapsed,
                         bool *pDriveMbusPinLo)
{
  bool outBitOne;
  //printf("state enter: %u, microSecElapsed %lu pDriveMbusPinLo %u\n", pMbusPhyTx->state, microSecElapsed, *pDriveMbusPinLo);
  
  switch (pMbusPhyTx->state) {
  case MBUS_STATE_TX_DISABLED:
    break;
  case MBUS_STATE_TX_IDLE:
    if (!_mbus_phy_tx_is_empty(pMbusPhyTx)) {
      pMbusPhyTx->microSecTimeStamp = microSecElapsed;
      pMbusPhyTx->bitShifter = TX_BIT_SHIFTER_INIT;
      pMbusPhyTx->bitShifter |= ((_mbus_phy_tx_pop(pMbusPhyTx) & 0x0F)<<4);
      outBitOne = _txShiftOutBit(&(pMbusPhyTx->bitShifter));
      if (outBitOne) {
        pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ONE;
      }
      else {
        pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ZERO;
      }
      *pDriveMbusPinLo = true;
    }
    else {
      *pDriveMbusPinLo = false;
    }
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
        pMbusPhyTx->microSecTimeStamp = microSecElapsed;
        outBitOne = _txShiftOutBit(&(pMbusPhyTx->bitShifter));
        if (outBitOne) {
          pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ONE;
        } else {
          pMbusPhyTx->state = MBUS_STATE_TX_SENDING_ZERO;
        }
        *pDriveMbusPinLo = true;
      } else {  // done with a nibble
        pMbusPhyTx->state = MBUS_STATE_TX_IDLE;
      }
    }
    break;
  }
  //printf("state leave: %u, microSecElapsed %lu pDriveMbusPinLo %u\n", pMbusPhyTx->state, microSecElapsed, *pDriveMbusPinLo);
}

bool mbus_phy_tx_is_full(MbusPhyStruct *pMbusPhy)
{
  return circular_buffer_is_full(&(pMbusPhy->tx.byteFifo));
}

void mbus_phy_tx_push(MbusPhyStruct *pMbusPhy, uint8_t nibble)
{
  circular_buffer_push(&(pMbusPhy->tx.byteFifo), &nibble);
}
