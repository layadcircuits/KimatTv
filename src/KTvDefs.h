#ifndef KTVDEFS_H
#define KTVDEFS_H
#include "Arduino.h"

#define DBG_ALL
#define TST_ALL

const uint32_t KTV_PERIOD_TIMEOUT = 3000;

#ifdef DBG_ALL
const uint32_t KTV_PERIOD_WAITBEFOREPOLL = 20;
#else
const uint32_t KTV_PERIOD_WAITBEFOREPOLL = 25;
#endif

const uint8_t QTY_COUNT_MAXQUERYBTNERRORS = 4;  // The maximum query button command error events allowed.
                                                // The library will go to the error state if this parameter is exceeded.
                                                // Possible causes: consecutive NAK and timeout  events

enum class KTvCmd : uint8_t
{
  ///////////////////cfontcmd2:         len:    payload:
  setFont = 0x01, // 0x01   0x01(4x6)     0x01    0x01(dummy)
  //                        0x02(6x8)
  //                        0x03(8x8)
  setCursor, // 0x02   0x01(dummy)   0x02    0xXX(x), 0xYY(y)
  print,     // 0x03   0x01(dummy)   0xLL    0xSS, 0xTT, 0xRR...(up to 20 bytes only)
  setPixel,  // 0x04   0x01(black)   0x02    0xXX(x), 0xYY(y)
  //                        0x02(white)
  //                        0x03(invert)
  fill, // 0x04   0x01(black)   0x01    0x01(dummy)
  //                        0x02(white)
  //                        0x03(invert)
  tone, // 0x05 //0x01(start)   0x06    frequency(2 bytes), durationMs(4 byes)
  //                        0x02(stop)
  queryButton // 0x06 //0x01(dummy)   0x01    0x01(dummy)
};

enum class KTvFont : uint8_t
{
  font4x6 = 0x01,
  font6x8,
  font8x8
};
enum class KTvColor : uint8_t
{
  black,
  white,
  inverse
};
enum class KTvToneCmd : uint8_t
{
  start,
  stop
};
enum class KTvBtn : uint8_t
{
  btnA,
  btnB,
  btnC,
  btnD,
  btnE,
  btnF,
  max
};

enum class KTvSymbol : uint8_t
{
  start = '@',
  end = '#',
  ready = 0x05,
  ack = 0x06,
  busy = 0x07,
  nak = 0x15,
  dummy = 0xAA,  
  btnAHi = 'A', btnALo = 'a', // Hi:btn is pressed, Lo: btn is released
  btnBHi = 'B', btnBLo = 'b', // Hi:btn is pressed, Lo: btn is released
  btnCHi = 'C', btnCLo = 'c', // Hi:btn is pressed, Lo: btn is released
  btnDHi = 'D', btnDLo = 'd', // Hi:btn is pressed, Lo: btn is released
  btnEHi = 'E', btnELo = 'e', // Hi:btn is pressed, Lo: btn is released
  btnFHi = 'F', btnFLo = 'f' // Hi:btn is pressed, Lo: btn is released
};

const uint16_t KTV_LEN_HEADER = 7;
const uint16_t KTV_LEN_MIN_PAYLOAD = 1;
const uint16_t KTV_LEN_MAX_PAYLOAD = 20;
const uint16_t KTV_LEN_MIN_PKT = KTV_LEN_HEADER + KTV_LEN_MIN_PAYLOAD;
const uint16_t KTV_LEN_MAX_PKT = KTV_LEN_HEADER + KTV_LEN_MAX_PAYLOAD;

enum class KTvPktOffset : uint8_t
{
  start,
  cmd0,
  cmd1,
  len,
  payload
};

enum class KTvStatus : uint8_t
{
  unknown,
  ready,
  busy,
  error
};

enum class KTvBtnStatus : uint8_t
{
  unknown,
  released,
  pressed
};

enum class KTvErrorCodes : uint8_t{
  ok, // device status as perceived by the library is OK
  btnQueryAkd, // last error event was an ack'd btn query cmd
  btnQueryNkd, // last error event was a nak'd btn query cmd
  btnQueryNoReply, // last error event was a no reply to a btn query cmd (device is always ready)
  btnQueryUnkRep, // last error event was an unknown reply to a btn query event
  btnQueryTimeout, // last error event was a no reply to a btn query event (timeout)
  userCmdEvents // the KTv module failed to respond to user commands as per the error event threshold
};
#endif