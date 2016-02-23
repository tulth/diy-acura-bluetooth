#include "mbus.h"
#include "stdio.h"
#include "circular_buffer.h"

void mbus_init(MbusStruct *pMbus,
               uint8_t *rxByteMemIn,
               size_t rxByteMemInSize,
               uint8_t *txByteMemIn,
               size_t txByteMemInSize,
               MbusMsgStruct *msgMemIn,
               size_t msgMemInSize
               )
{
  circular_buffer_nomalloc_init(&(pMbus->msgFifo),
                                msgMemIn,
                                msgMemInSize,
                                sizeof(MbusMsgStruct));
  mbus_phy_init(&(pMbus->phy),
                rxByteMemIn,
                rxByteMemInSize,
                txByteMemIn,
                txByteMemInSize);
  mbus_phy_rx_enable(&(pMbus->phy));
  pMbus->nibbles.nibbleArrayLength = 0;
}

// internal use only!
void _mbus_msg_push(MbusStruct *pMbus)
{
  MbusMsgStruct rxMbusMsg;
  // printf("MSG! pMbus->nibbles.nibbleArrayLength: %d\n", pMbus->nibbles.nibbleArrayLength);
  mbus_link_parseMsg(pMbus->nibbles.nibbleArray,
                     pMbus->nibbles.nibbleArrayLength-1,
                     &rxMbusMsg);
  circular_buffer_push(&(pMbus->msgFifo), &rxMbusMsg);
}

void mbus_update(MbusStruct *pMbus,
                 unsigned long microSecElapsed,
                 bool mbusPinHi,
                 bool *pDriveMbusPinLo)
{
  uint8_t rxNibble;
  
  mbus_phy_update(&(pMbus->phy),
                  microSecElapsed,
                  mbusPinHi,
                  pDriveMbusPinLo);
  if (!mbus_phy_rx_is_empty(&(pMbus->phy))) {
    rxNibble = mbus_phy_rx_pop(&(pMbus->phy));
    // printf("rxNibble %c\n", mbus_phy_rxnibble2ascii(rxNibble));
    pMbus->nibbles.nibbleArray[pMbus->nibbles.nibbleArrayLength] = rxNibble;
    if (pMbus->nibbles.nibbleArrayLength < NIBBLE_ARRAY_SIZE) {
      pMbus->nibbles.nibbleArrayLength++;
    }
    if (rxNibble == MBUS_END_MSG_CODE) {
      _mbus_msg_push(pMbus);
      pMbus->nibbles.nibbleArrayLength = 0;
    }
  }
  
}

bool mbus_is_empty(MbusStruct *pMbus)
{
  return circular_buffer_is_empty(&(pMbus->msgFifo));
}

void mbus_pop(MbusStruct *pMbus, MbusMsgStruct *pMbusMsgOut)
{
  circular_buffer_pop(&(pMbus->msgFifo), pMbusMsgOut);
}

int mbus_msgToStr(MbusMsgStruct *pMbusMsgIn,
                  char *strOut,
                  unsigned int strOutMaxSize)
{
  return mbus_link_msgToStr(pMbusMsgIn,
                            strOut,
                            strOutMaxSize);
}

