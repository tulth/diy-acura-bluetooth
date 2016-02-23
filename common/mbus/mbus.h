#ifndef FILE_MBUS_SEEN
#define FILE_MBUS_SEEN

#include <stdbool.h>
#include <stdint.h>
#include "mbus_phy.h"
#include "mbus_link.h"

typedef struct {
  MbusPhyStruct phy;
  circular_buffer msgFifo;
  MbusRawNibbleListStruct nibbles;
} MbusStruct;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern void mbus_init(MbusStruct *pMbus,
                        uint8_t *rxByteMemIn,
                        size_t rxByteMemInSize,
                        uint8_t *txByteMemIn,
                        size_t txByteMemInSize,
                        MbusMsgStruct *msgMemIn,
                        size_t msgMemInSize
                        );
  extern void mbus_update(MbusStruct *pMbus,
                          unsigned long microSecElapsed,
                          bool mbusPinHi,
                          bool *pDriveMbusPinLo);
  extern bool mbus_is_empty(MbusStruct *pMbus);
  extern void mbus_pop(MbusStruct *pMbus, MbusMsgStruct *pMbusMsgOut);
  extern int mbus_msgToStr(MbusMsgStruct *pMbusMsgIn,
                           char *strOut,
                           unsigned int strOutMaxSize);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_MBUS_SEEN
