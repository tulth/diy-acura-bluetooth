#include "mbus_link.h"
#include "stdio.h"
#include "mbus_phy.h"

void lengthCheck(MbusMsgStruct *pMbusMsgOut, bool lengthOk);
void doCheckSum(MbusMsgStruct *pMbusMsgOut);
void doDirection(MbusMsgStruct *pMbusMsgOut);
void parseHead2CdBody(MbusMsgStruct *);
void parseCd2HeadBody(MbusMsgStruct *);
void parseSetPlayState(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq);
void parseSetDiskTrack(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq);
void parseSetMode(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq);
void parseChanging(MbusMsgStruct *pMbusMsgOut, uint8_t* nibbleSeq);
void parsePlayState(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq);
void parseDiskInfo(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq);


void mbus_link_parseMsg(uint8_t* nibbleSequence, unsigned int numNibbles, MbusMsgStruct *pMbusMsgOut)
{
  int index;
  pMbusMsgOut->errId = 0;
  //printf("%p %d\n", nibbleSequence, numNibbles);
  /****
   * copy in raw nibbles, up to the array size, also do size and invalid signal checking
   ****/
  if (numNibbles > NIBBLE_ARRAY_SIZE) {
    pMbusMsgOut->rawNibbles.nibbleArrayLength = NIBBLE_ARRAY_SIZE;
    pMbusMsgOut->errId = ERR_ID_LENGTH;
  }
  else {
    pMbusMsgOut->rawNibbles.nibbleArrayLength = numNibbles;
  }
  //printf("pMbusMsgOut->rawNibbles.nibbleArrayLength %d\n", pMbusMsgOut->rawNibbles.nibbleArrayLength);
  for (index=0; index < pMbusMsgOut->rawNibbles.nibbleArrayLength; index++) {
    pMbusMsgOut->rawNibbles.nibbleArray[index] = nibbleSequence[index];
    if ((nibbleSequence[index] == 'T') ||
        (nibbleSequence[index] == 'L') ||
        (nibbleSequence[index] == 'X')) {
      pMbusMsgOut->errId = ERR_ID_SIGNAL;
    }
  }
  lengthCheck(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArrayLength > 2);
  if (pMbusMsgOut->errId != 0) {
    return;
  }
  /****
   * verify checksum 
   ****/
  doCheckSum(pMbusMsgOut);
  if (pMbusMsgOut->errId != 0) {
    return;
  }
  doDirection(pMbusMsgOut);
  if (pMbusMsgOut->errId != 0) {
    return;
  }
  
  if (pMbusMsgOut->parsed.directionH2C) {
    parseHead2CdBody(pMbusMsgOut);
  }
  else {
    parseCd2HeadBody(pMbusMsgOut);
  }
  
}

void lengthCheck(MbusMsgStruct *pMbusMsgOut, bool lengthOk)
{
  if (!lengthOk) {
    pMbusMsgOut->errId = ERR_ID_LENGTH;
  }
}

void doCheckSum(MbusMsgStruct *pMbusMsgOut)
{
  int index;
  uint8_t expectedCheckSum = 0;
  uint8_t actualCheckSum = pMbusMsgOut->rawNibbles.nibbleArray[pMbusMsgOut->rawNibbles.nibbleArrayLength-1];

  expectedCheckSum = 0;
  for (index=0; index < (pMbusMsgOut->rawNibbles.nibbleArrayLength - 1); index++) {
    expectedCheckSum ^= pMbusMsgOut->rawNibbles.nibbleArray[index];
  }
  expectedCheckSum = (expectedCheckSum + 1) % 16;
  if (actualCheckSum != expectedCheckSum) {
    pMbusMsgOut->errId = ERR_ID_CHECKSUM;
  }
}

void doDirection(MbusMsgStruct *pMbusMsgOut)
{
  uint8_t dirNibble = pMbusMsgOut->rawNibbles.nibbleArray[0];
  if (dirNibble == 1) {
    pMbusMsgOut->parsed.directionH2C = true;
  }
  else if (dirNibble == 9) {
    pMbusMsgOut->parsed.directionH2C = false;
  }
  else {
    pMbusMsgOut->errId = ERR_ID_DIRECTION;
  }
}

void parseHead2CdBody(MbusMsgStruct *pMbusMsgOut)
{
  uint8_t cmd0 = pMbusMsgOut->rawNibbles.nibbleArray[1];
  uint8_t cmd1 = pMbusMsgOut->rawNibbles.nibbleArray[2];
  // printf("cmd0 %d\n", cmd0);
  if (cmd0 == 0x8) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 3));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_ping;
    }
  }
  else if ((cmd0 == 0x1) && (cmd1 == 0x1)) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 6));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_setPlayState;
      parseSetPlayState(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 3);
    }
  }
  else if ((cmd0 == 0x1) && (cmd1 == 0x3)) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 9));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_setDiskTrack;
      parseSetDiskTrack(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 3);
    }
  }
  else if ((cmd0 == 0x1) && (cmd1 == 0x4)) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 9));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_setMode;
      parseSetMode(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 3);
    }
  }
  else if (cmd0 == 0x9) {
    // printf("pMbusMsgOut->rawNibbles.nibbleArrayLength %d\n", pMbusMsgOut->rawNibbles.nibbleArrayLength);
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 3));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_headPowerOn;
    }
  }
  else {
    pMbusMsgOut->errId = ERR_ID_UNKNOWN_TYPE;
  }
}

void parseCd2HeadBody(MbusMsgStruct *pMbusMsgOut)
{
  uint8_t cmd0 = pMbusMsgOut->rawNibbles.nibbleArray[1];
  //  uint8_t cmd1 = pMbusMsgOut->rawNibbles.nibbleArray[2];
  if (cmd0 == 0x8) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 3));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_pong;
    }
  }
  else if (cmd0 == 0x9) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 16));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_playState;
      parsePlayState(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 2);
    }
  }
  else if (cmd0 == 0xA) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 13));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_cdPowerOn;
    }
  }
  else if (cmd0 == 0xB) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 12));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_changing;
      parseChanging(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 2);
    }
  }
  else if (cmd0 == 0xC) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 13));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_diskInfo;
      parseDiskInfo(pMbusMsgOut, pMbusMsgOut->rawNibbles.nibbleArray + 2);
    }
  }
  else if (cmd0 == 0xD) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 11));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_unknownStatus;
    }
  }
  else if (cmd0 == 0xF) {
    lengthCheck(pMbusMsgOut, (pMbusMsgOut->rawNibbles.nibbleArrayLength == 8));
    if (pMbusMsgOut->errId == 0) {
      pMbusMsgOut->parsed.msgType = MSGTYPE_ackWait;
    }
  }
  else {
    pMbusMsgOut->errId = ERR_ID_UNKNOWN_TYPE;
  }
}

static inline uint8_t nibblePairToBcd(uint8_t nibHi, uint8_t nibLo)
{
  return nibHi*10 + nibLo;
}

void parseSetPlayState(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq)
{
  pMbusMsgOut->parsed.body.setPlayState.resume = (nibbleSeq[0] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.stop = (nibbleSeq[0] & (1 << 2)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.scanStop = (nibbleSeq[0] & (1 << 0)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.fastReverse = (nibbleSeq[1] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.fastForward = (nibbleSeq[1] & (1 << 2)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.pause = (nibbleSeq[1] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.setPlayState.play = (nibbleSeq[1] & (1 << 0)) != 0;
}

void parseSetDiskTrack(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq)
{
  pMbusMsgOut->parsed.body.setDiskTrack.disk = nibbleSeq[0];
  pMbusMsgOut->parsed.body.setDiskTrack.track = nibblePairToBcd(nibbleSeq[1], nibbleSeq[2]);
  pMbusMsgOut->parsed.body.setDiskTrack.pause = (nibbleSeq[3] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.setDiskTrack.play = (nibbleSeq[3] & (1 << 0)) != 0;
  pMbusMsgOut->parsed.body.setDiskTrack.random = (nibbleSeq[4] & (1 << 2)) != 0;
}

void parseSetMode(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq)
{
  pMbusMsgOut->parsed.body.setMode.repeatOne = (nibbleSeq[0] & (1 << 2)) != 0;
  pMbusMsgOut->parsed.body.setMode.repeatAll = (nibbleSeq[0] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.setMode.random = (nibbleSeq[1] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.setMode.introScan = (nibbleSeq[1] & (1 << 4)) != 0;
}

void parseChanging(MbusMsgStruct *pMbusMsgOut, uint8_t* nibbleSeq)
{
  pMbusMsgOut->parsed.body.changing.disk = nibbleSeq[1];
  pMbusMsgOut->parsed.body.changing.track = nibblePairToBcd(nibbleSeq[2], nibbleSeq[3]);
  pMbusMsgOut->parsed.body.changing.eject = (nibbleSeq[4] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.changing.busy = (nibbleSeq[4] & (1 << 0)) != 0;
  pMbusMsgOut->parsed.body.changing.repeatAll = (nibbleSeq[5] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.changing.repeatOne = (nibbleSeq[5] & (1 << 2)) != 0;
  pMbusMsgOut->parsed.body.changing.random = (nibbleSeq[6] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.changing.done = (nibbleSeq[8] & (1 << 0)) != 0;
}


void parsePlayState(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq)
{
  pMbusMsgOut->parsed.body.playState.track = nibblePairToBcd(nibbleSeq[1], nibbleSeq[2]);
  pMbusMsgOut->parsed.body.playState.index = nibblePairToBcd(nibbleSeq[3], nibbleSeq[4]);
  pMbusMsgOut->parsed.body.playState.minute = nibblePairToBcd(nibbleSeq[5], nibbleSeq[6]);
  pMbusMsgOut->parsed.body.playState.second = nibblePairToBcd(nibbleSeq[7], nibbleSeq[8]);
  pMbusMsgOut->parsed.body.playState.repeatAll = (nibbleSeq[9] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.playState.repeatOne = (nibbleSeq[9] & (1 << 2)) != 0;
  pMbusMsgOut->parsed.body.playState.introScan = (nibbleSeq[10] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.playState.random = (nibbleSeq[10] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.playState.stopped = (nibbleSeq[12] & (1 << 3)) != 0;
  pMbusMsgOut->parsed.body.playState.paused = (nibbleSeq[12] & (1 << 1)) != 0;
  pMbusMsgOut->parsed.body.playState.play = (nibbleSeq[12] & (1 << 0)) != 0;
}

void parseDiskInfo(MbusMsgStruct *pMbusMsgOut, uint8_t *nibbleSeq)
{
  pMbusMsgOut->parsed.body.diskInfo.disk = nibbleSeq[0];
  pMbusMsgOut->parsed.body.diskInfo.tracks = nibblePairToBcd(nibbleSeq[3], nibbleSeq[4]);
  pMbusMsgOut->parsed.body.diskInfo.minutes = nibblePairToBcd(nibbleSeq[5], nibbleSeq[6]);
  pMbusMsgOut->parsed.body.diskInfo.seconds = nibblePairToBcd(nibbleSeq[7], nibbleSeq[8]);
  pMbusMsgOut->parsed.body.diskInfo.flags = nibbleSeq[9];
}

static inline void _addChar(char charToAdd, char **pStrOut, unsigned int *pStrOutMaxSize)
{
  if ((*pStrOutMaxSize) > 0) {
    **pStrOut = charToAdd;
    (*pStrOut)++;
    (*pStrOutMaxSize)--;
  }
}
            
static inline void _addStr(char *strToAdd, char **pStrOut, unsigned int *pStrOutMaxSize)
{
  while (*strToAdd != '\0') {
    _addChar(*strToAdd, pStrOut, pStrOutMaxSize);
    strToAdd++;
  }
  /* (*pStrOutMaxSize)++; */
  /* _addChar('\0', pStrOut, pStrOutMaxSize); */
  /* pStrOut--; */
}
            
int msgParsedToStr(MbusMsgParsedStruct *parsedStructIn,
                   char *strOut,
                   unsigned int strOutMaxSize
                   )
{
  unsigned int workingMaxSize = strOutMaxSize;
  // direction
  if (parsedStructIn->directionH2C) {
    _addStr("H2C ", &strOut, &workingMaxSize);
  }
  else {
    _addStr("C2H ", &strOut, &workingMaxSize);
  }
  // message type
  if (parsedStructIn->msgType == MSGTYPE_ping) {
    _addStr("ping            ", &strOut, &workingMaxSize);
  }
  else if (parsedStructIn->msgType == MSGTYPE_setPlayState) {
    // bodyStr = str(parsedStructIn->body.setPlayState);
    _addStr("setPlayState    ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "resume%d stop%d scanStop%d fastReverse%d fastForward%d pause%d play%d",
                               parsedStructIn->body.setPlayState.resume,
                               parsedStructIn->body.setPlayState.stop,
                               parsedStructIn->body.setPlayState.scanStop,
                               parsedStructIn->body.setPlayState.fastReverse,
                               parsedStructIn->body.setPlayState.fastForward,
                               parsedStructIn->body.setPlayState.pause,
                               parsedStructIn->body.setPlayState.play
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_setDiskTrack) {
    _addStr("setDiskTrack    ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "disk#%d track:%02d pause%d play%d random%d",
                               parsedStructIn->body.setDiskTrack.disk,
                               parsedStructIn->body.setDiskTrack.track,
                               parsedStructIn->body.setDiskTrack.pause,
                               parsedStructIn->body.setDiskTrack.play,
                               parsedStructIn->body.setDiskTrack.random
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_setMode) {
    _addStr("setMode         ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "repeatAll%d repeatOne%d introScan%d random%d",
                               parsedStructIn->body.setMode.repeatAll,
                               parsedStructIn->body.setMode.repeatOne,
                               parsedStructIn->body.setMode.introScan,
                               parsedStructIn->body.setMode.random
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_headPowerOn) {
    _addStr("headPowerOn     ", &strOut, &workingMaxSize);
    // FUTURE: bodyStr = str(parsedStructIn->body.headPowerOn);
  }
  else if (parsedStructIn->msgType == MSGTYPE_pong) {
    _addStr("pong            ", &strOut, &workingMaxSize);
  }
  else if (parsedStructIn->msgType == MSGTYPE_ackWait) {
    _addStr("ackWait         ", &strOut, &workingMaxSize);
    // FUTURE: bodyStr = str(parsedStructIn->body.ackWait);
  }
  else if (parsedStructIn->msgType == MSGTYPE_cdPowerOn) {
    _addStr("cdPowerOn       ", &strOut, &workingMaxSize);
    // FUTURE: bodyStr = str(parsedStructIn->body.cdPowerOn);
  }
  else if (parsedStructIn->msgType == MSGTYPE_changing) {
    _addStr("changing        ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "disk#%d Trk%02d eject%d busy%d repeatAll%d repeatOne%d random%d done%d",
                               parsedStructIn->body.changing.disk,
                               parsedStructIn->body.changing.track,
                               parsedStructIn->body.changing.eject,
                               parsedStructIn->body.changing.busy,
                               parsedStructIn->body.changing.repeatAll,
                               parsedStructIn->body.changing.repeatOne,
                               parsedStructIn->body.changing.random,
                               parsedStructIn->body.changing.done
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_playState) {
    _addStr("playState       ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "Trk:Idx %02u:%02u min:sec %02d:%02d repeatAll%d repeatOne%d introScan%d random%d stop%d pause%d play%d",
                               parsedStructIn->body.playState.track,
                               parsedStructIn->body.playState.index,
                               parsedStructIn->body.playState.minute,
                               parsedStructIn->body.playState.second,
                               parsedStructIn->body.playState.repeatAll,
                               parsedStructIn->body.playState.repeatOne,
                               parsedStructIn->body.playState.introScan,
                               parsedStructIn->body.playState.random,
                               parsedStructIn->body.playState.stopped,
                               parsedStructIn->body.playState.paused,
                               parsedStructIn->body.playState.play
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_diskInfo) {
    _addStr("diskInfo        ", &strOut, &workingMaxSize);
    workingMaxSize -= snprintf(strOut,
                               workingMaxSize,
                               "disk#%d tracks %02d min:sec %02d:%02d flags %x",
                               parsedStructIn->body.diskInfo.disk,
                               parsedStructIn->body.diskInfo.tracks,
                               parsedStructIn->body.diskInfo.minutes,
                               parsedStructIn->body.diskInfo.seconds,
                               parsedStructIn->body.diskInfo.flags
                               );
  }
  else if (parsedStructIn->msgType == MSGTYPE_unknownStatus) {
    _addStr("unknownStatus   ", &strOut, &workingMaxSize);
    // FUTURE: bodyStr = str(parsedStructIn->body.unknownStatus);
  }
  else {
    _addStr("STRIFY ERR      ", &strOut, &workingMaxSize);
  }
  return strOutMaxSize - workingMaxSize;
}

int mbus_link_msgToStr(MbusMsgStruct *pMbusMsgIn,
                       char *strOut,
                       unsigned int strOutMaxSize
                       )
{
  unsigned int idx;
  char *workingStr = strOut;
  unsigned int workingMaxSize = strOutMaxSize;
  workingStr[workingMaxSize-1] = '\0';
  workingMaxSize--;
  
  for(idx = 0; (idx < 18); idx++) {
    if (idx < pMbusMsgIn->rawNibbles.nibbleArrayLength) {
      _addChar(mbus_phy_rxnibble2ascii(pMbusMsgIn->rawNibbles.nibbleArray[idx]), &workingStr, &workingMaxSize);
    }
    else {
      _addChar(' ', &workingStr, &workingMaxSize);
    }
  }
  _addChar(' ', &workingStr, &workingMaxSize);
  
  if (pMbusMsgIn->errId == ERR_ID_SIGNAL) {
    workingMaxSize -= snprintf(workingStr, workingMaxSize, "ERROR: Bad bus signaling");
  }
  else if (pMbusMsgIn->errId == ERR_ID_LENGTH) {
    workingMaxSize -= snprintf(workingStr, workingMaxSize, "ERROR: Bad message length");
  }
  else if (pMbusMsgIn->errId == ERR_ID_CHECKSUM) {
    workingMaxSize -= snprintf(workingStr, workingMaxSize, "ERROR: CheckSum");
  }
  else if (pMbusMsgIn->errId == ERR_ID_DIRECTION) {
    workingMaxSize -= snprintf(workingStr, workingMaxSize, "ERROR: Unexpected Direction");
  }
  else if (pMbusMsgIn->errId == ERR_ID_UNKNOWN_TYPE) {
    workingMaxSize -= snprintf(workingStr, workingMaxSize, "ERROR: Unknown message type");
  }
  else {
    workingMaxSize -= msgParsedToStr(&(pMbusMsgIn->parsed), workingStr, workingMaxSize);
  }
  strOut[strOutMaxSize - workingMaxSize - 1] = '\0';
  return strOutMaxSize - workingMaxSize;
}
