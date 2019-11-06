#ifndef KIMATTV_H
#define KIMATTV_H

#include "KTvDefs.h"
#include <Wire.h>

#ifdef DBG_ALL
// attach debug port to library first then enable any below
// #define DBG_D1 // init()
// #define DBG_D2 // constructPacket()
// #define DBG_D3 // transmitPacket()
// #define DBG_D4A // run(), st: checkReply
// #define DBG_D4AU // run(), st: checkReply, usr cmd
// #define DBG_D4AB // run(), st: checkReply, btn cmd
// #define DBG_D4B // run(), st: initial
// #define DBG_D4C // run(), st: pollForReply
// #define DBG_D5 // error state
// #define DBG_D6 // setPixel()
// #define DBG_D7 // drawPixels16()
// #define DBG_D8 // drawPixelLine()

#endif

#ifdef TST_ALL
// #define TST_1 // test KTvErrorCodes::btnQueryTimeout, KTvErrorCodes::userCmdTimeout
#endif

class KimatTv
{
public:
  KimatTv(uint8_t i2cAddress)
      : deviceAddress{i2cAddress}, status{KTvStatus::ready}, btnStates{}, 
      sendBfr{}, sendBfr2{}, isUsrCmdPending{}, queryBtnCtr{}, btnCmdErrCtr{},
      userCmdErrCtr{}, pktLenToTx{},
      deviceErrorCode{KTvErrorCodes::ok},
      dbgPrt{}, errorHandler{}
  {
  }
  void init();
  void attachDebug(Stream *ptr) { dbgPrt = ptr; };
  void attachErrorHandler(ErrorHandler handler) { errorHandler = handler; };
  KTvStatus getStatus() { return status; };
  // KTvStatus getStatus();
  bool isReady();
  void waitUntilReady();
  KTvBtnStatus getPinStatus(KTvBtn btn) { return btnStates[(uint8_t)btn]; }
  void setCursor(uint8_t x, uint8_t y);
  void setFont(KTvFont font);
  void print(char *str);
  void print(const char *str) {print((char*)str);}
  void fill(KTvColor color);
  void setPixel(uint8_t x, uint8_t y, KTvColor color);
  void tone(KTvToneCmd cmdType, uint16_t freq, uint32_t duration);
  void startTone(uint16_t freq, uint32_t duration);
  void stopTone();
  void delay(uint32_t period);
  void run();

  void drawPixels16(uint8_t x, uint8_t y, KTvOrientation orientation, uint16_t binary);
  void drawPixelLine(KTvOrientation orientation, uint8_t offset, uint16_t *ptr);

private:
  uint8_t deviceAddress;
  KTvStatus status;
  KTvBtnStatus btnStates[(uint8_t)KTvBtn::max];
  uint8_t sendBfr[KTV_LEN_MAX_PKT]; // used for user cmds
  uint8_t sendBfr2[KTV_LEN_MIN_PKT]; // used for btn query cmds
  bool isUsrCmdPending;
  uint8_t queryBtnCtr;
  uint8_t btnCmdErrCtr;
  uint8_t userCmdErrCtr;
  uint8_t pktLenToTx;
  KTvErrorCodes deviceErrorCode;
  Stream *dbgPrt;
  ErrorHandler errorHandler;

  void printU8Array(uint8_t * arr, uint8_t len);
  uint16_t getChecksum(uint8_t *bfr, uint8_t len);
  uint8_t constructPacket(uint8_t cmd0, uint8_t cmd1, uint8_t len, uint8_t *payload, uint8_t *bfr);
  void transmitPacket(uint8_t width, uint8_t *bfr);
  void queryButton(KTvBtn btn);
};
#endif