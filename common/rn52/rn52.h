#ifndef FILE_RN52_SEEN
#define FILE_RN52_SEEN

#include <stdbool.h>
#include <stdint.h>
#include "fifo.h"

#define RN52_CMD_TIMEOUT_MILLSEC  500

#define RN52_CONSTAT_A2DP                    ((1<<2)<<8)
#define RN52_CONSTAT_TRACKCHANGE             ((1<<5)<<8)

#define RN52_CONSTAT_CONSTATE_AUDIO_STREAMING     13

#define RN52_EXTFEAT_ENA_AVRCP_BUTTONS             (1<<00) /* Enable AVRCP buttons for EK */
#define RN52_EXTFEAT_ENA_POWERON_RECONNECT         (1<<01) /* Enable reconnect on power-on */
#define RN52_EXTFEAT_ENA_STARTUP_DISCOVERABLE      (1<<02) /* Discoverable on start up */
#define RN52_EXTFEAT_ENA_CODEC_INDICATORS          (1<<03) /* Codec indicators PIO7 (AAC) and PIO6 (aptX) */
#define RN52_EXTFEAT_REBOOT_AFTER_DISCONNECT       (1<<04) /* Reboot after disconnect */
#define RN52_EXTFEAT_MUTE_VOLUME_TONES             (1<<05) /* Mute volume up/down tones */
#define RN52_EXTFEAT_ENA_VOICE_BUTTON_GPIO4        (1<<06) /* Enable voice command button on PIO4 */
#define RN52_EXTFEAT_DISABLE_SYSTEM_TONES          (1<<07) /* Disable system tones */
#define RN52_EXTFEAT_POWEROFF_AFTER_PAIRING_TEMOUT (1<<08) /* Power off after pairing timeout */
#define RN52_EXTFEAT_RESET_AFTER_POWER_OFF         (1<<09) /* Reset after power off */
#define RN52_EXTFEAT_ENA_LIST_RECONNECT_POSTPANIC  (1<<10) /* Enable list reconnect after panic */
#define RN52_EXTFEAT_ENA_LATCH_EVENT_GPIO2         (1<<11) /* Enable latch event indicator PIO2 */
#define RN52_EXTFEAT_ENA_TRACK_CHANGE_EVENT        (1<<12) /* Enable track change event */
#define RN52_EXTFEAT_ENA_FIXEDVOL_TONES            (1<<13) /* Enable tones playback at fixed volume */
#define RN52_EXTFEAT_ENA_AUTOACCEPT_PASSKEY        (1<<14) /* Enable auto-accept passkey in Keyboard I/O Authentication mode */

typedef struct {
  int state;
  uint8_t modeCmdNotData;
  uint16_t connectionStatus;
  uint16_t extFeat;
  uint8_t returnState;
  uint8_t rxStrLen;
  char rxStr[64];
  char txCmd[16];
  unsigned long eventLoMilliSecTimeStamp;
  unsigned long cmd2CmdMilliSecTimeStamp;
  unsigned long replyMilliSecTimeStamp;
  int (*pFuncSerialAvailable)(void);
  int (*pFuncSerialGetChar)(void);
  void (*pFuncSerialPutChar)(uint32_t c);
  fifo avrcpCmdFifo;
} Rn52Struct;

const uint16_t DEFAULT_EXTFEAT = (RN52_EXTFEAT_DISABLE_SYSTEM_TONES  |
                                  RN52_EXTFEAT_ENA_POWERON_RECONNECT);
/* const uint16_t DEFAULT_EXTFEAT = (RN52_EXTFEAT_DISABLE_SYSTEM_TONES); */


#define RN52_AVRCP_CMD_PLAY      1
#define RN52_AVRCP_CMD_PAUSE     2
#define RN52_AVRCP_CMD_NEXT      3
#define RN52_AVRCP_CMD_PREV      4
#define RN52_AVRCP_CMD_PLAYPAUSE 5

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  extern void rn52_init(Rn52Struct *pRn52,
                             int (*pFuncSerialAvailable)(void),
                             int (*pFuncSerialGetChar)(void),
                             void (*pFuncSerialPutChar)(uint32_t c),
                             uint8_t *avrcpCmdFifoMem,
                             size_t avrcpCmdFifoMemSize);
  extern void rn52_update(Rn52Struct *pRn52,
                               unsigned long milliSecElapsed,
                               bool eventStatePin,
                               bool *pDriveCmdPinLo);
  extern bool rn52_is_connected(Rn52Struct *pRn52);
  extern void rn52_avrcp_play(Rn52Struct *pRn52);
  extern void rn52_avrcp_pause(Rn52Struct *pRn52);
  extern void rn52_avrcp_next(Rn52Struct *pRn52);
  extern void rn52_avrcp_prev(Rn52Struct *pRn52);
  extern void rn52_avrcp_playpause(Rn52Struct *pRn52);
  extern void rn52_avrcp_status(Rn52Struct *pRn52,
                                     bool *connected,
                                     bool *playing);
  extern void rn52_spp_tx_send(Rn52Struct *pRn52,
                                    const char *data);
  extern bool rn52_avrcp_is_playing(Rn52Struct *pRn52);
  extern bool rn52_spp_rx_is_empty(Rn52Struct *pRn52);
  extern int rn52_spp_rx_read(Rn52Struct *pRn52, char *charBuf, int charBufSize);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FILE_RN52_SEEN

