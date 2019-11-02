#ifndef KIMATTV_H
#define KIMATTV_H

#include "KTvDefs.h"
#include <Wire.h>

#ifdef DBG_ALL
// #define DBG_D1 // init()
// #define DBG_D2 // constructPacket()
// #define DBG_D3 // transmitPacket()
#define DBG_D4A // run(), st: checkReply (filtered reply)
#define DBG_D4B // run(), st: initial
#define DBG_D4C // run(), st: pollForReply
#define DBG_D5 // error state

#endif

#ifdef TST_ALL
// #define TST_1 // test KTvErrorCodes::btnQueryTimeout
#endif

class KimatTv
{
public:
  KimatTv(uint8_t i2cAddress)
      : deviceAddress{i2cAddress}, status{KTvStatus::unknown}, btnStates{}, 
      sendBfr{}, sendBfr2{}, isUsrCmdPending{}, queryBtnCtr{}, queryBtnErrCtr{},
      deviceErrorCode{KTvErrorCodes::ok},
      dbgPrt{}
  {
  }
  void init();
  void attachDebug(Stream *ptr) { dbgPrt = ptr; };
  KTvStatus getStatus() { return status; };
  KTvBtnStatus getPinStatus(KTvBtn btn) { return btnStates[(uint8_t)btn]; }
  void run();

private:
  uint8_t deviceAddress;
  KTvStatus status;
  KTvBtnStatus btnStates[(uint8_t)KTvBtn::max];
  uint8_t sendBfr[KTV_LEN_MAX_PKT]; // used for user cmds
  uint8_t sendBfr2[KTV_LEN_MIN_PKT]; // used for btn query cmds
  bool isUsrCmdPending;
  uint8_t queryBtnCtr;
  uint8_t queryBtnErrCtr;
  KTvErrorCodes deviceErrorCode;
  Stream *dbgPrt;

  void printU8Array(uint8_t * arr, uint8_t len);
  uint16_t getChecksum(uint8_t *bfr, uint8_t len);
  uint8_t constructPacket(uint8_t cmd0, uint8_t cmd1, uint8_t len, uint8_t *payload, uint8_t *bfr);
  void transmitPacket(uint8_t width, uint8_t *bfr);
  void queryButton(KTvBtn btn);
};
#endif