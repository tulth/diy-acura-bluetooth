#ifndef FILE_MBUS_PHY_SEEN
#define FILE_MBUS_PHY_SEEN

#include <stdbool.h>
#include <stdint.h>
#include "fifo.h"

#define MBUS_LOW_TOO_LONG_CODE 16 
#define MBUS_TIMEOUT_CODE 17
#define MBUS_END_MSG_CODE 18

typedef struct {
  uint8_t state;
  uint8_t bitShifter;
  unsigned long microSecTimeStamp;
  fifo byteFifo;
} MbusPhyTxRxStruct;

typedef struct {
  MbusPhyTxRxStruct tx;
  MbusPhyTxRxStruct rx;
} MbusPhyStruct;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern char mbus_phy_rxnibble2ascii(uint8_t nib);  /* convert nibble to ascii hex */
  extern void mbus_phy_init(MbusPhyStruct *pMbusPhy,
                            uint8_t *rxByteMemIn,
                            size_t rxByteMemInSize,
                            uint8_t *txByteMemIn,
                            size_t txByteMemInSize);
  extern void mbus_phy_update(MbusPhyStruct *pMbusPhy,
                              unsigned long microSecElapsed,
                              bool mbusPinHi,
                              bool *pDriveMbusPinLo);
  extern void mbus_phy_rx_enable(MbusPhyStruct *pMbusPhy);
  extern void mbus_phy_rx_disable(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_rx_is_busy(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_rx_is_enabled(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_rx_is_empty(MbusPhyStruct *pMbusPhy);
  extern uint8_t mbus_phy_rx_pop(MbusPhyStruct *pMbusPhy);
  extern void mbus_phy_tx_enable(MbusPhyStruct *pMbusPhy);
  extern void mbus_phy_tx_disable(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_tx_is_busy(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_tx_is_enabled(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_tx_is_empty(MbusPhyStruct *pMbusPhy);
  extern bool mbus_phy_tx_is_full(MbusPhyStruct *pMbusPhy);
  extern void mbus_phy_tx_push(MbusPhyStruct *pMbusPhy, uint8_t nibble);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_MBUS_PHY_SEEN

