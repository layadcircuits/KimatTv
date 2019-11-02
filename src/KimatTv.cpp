#include "KimatTv.h"

void KimatTv::init()
{
  Wire.begin();
// init device polling here
#ifdef DBG_D1
  if (dbgPrt != nullptr)
  {
    dbgPrt->println(F("KTv: I2C init'd!"));
  }
#endif
}

void KimatTv::printU8Array(uint8_t *arr, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    dbgPrt->print(*(arr + i), HEX);
    dbgPrt->print(F(" "));
  }
}

uint16_t KimatTv::getChecksum(uint8_t *sendBfr, uint8_t len)
{
  uint16_t chksm = 0;
  for (uint8_t i = 0; i < len; i++)
  {
    chksm += sendBfr[i];
  }
  return chksm;
}

// clear bfr before calling this function
// returns the number of bytes in the constructed packet
uint8_t KimatTv::constructPacket(uint8_t cmd0, uint8_t cmd1, uint8_t len, uint8_t *payload, uint8_t *bfr)
{
  bfr[(uint8_t)KTvPktOffset::start] = (uint8_t)KTvSymbol::start;
  bfr[(uint8_t)KTvPktOffset::cmd0] = cmd0;
  bfr[(uint8_t)KTvPktOffset::cmd1] = cmd1;
  bfr[(uint8_t)KTvPktOffset::len] = len;
  memcpy(&bfr[(uint8_t)KTvPktOffset::payload], payload, len);
  uint8_t offsetChksm0 = (uint8_t)KTvPktOffset::payload + len;
  uint8_t offsetChksm1 = offsetChksm0 + 1;
  uint8_t offsetSymbolEnd = offsetChksm1 + 1;

  uint16_t chksm = getChecksum(bfr, 4 + len);
  bfr[offsetChksm0] = (chksm >> 8) & 0x00FF;
  bfr[offsetChksm1] = chksm & 0x00FF;
  bfr[offsetSymbolEnd] = (uint8_t)KTvSymbol::end;

#ifdef DBG_D2
  if (dbgPrt != nullptr)
  {
    dbgPrt->print(F("KTv: Constructed pkt: "));
    printU8Array(bfr, len + 7);
    dbgPrt->print(F("\nGenerated checksum: "));
    dbgPrt->println(chksm, HEX);
  }
#endif

  return (len + 7);
}

void KimatTv::transmitPacket(uint8_t width, uint8_t *bfr)
{
  Wire.beginTransmission(deviceAddress);
  for (uint8_t i = 0; i < width; i++)
  {
    Wire.write(bfr[i]);
  }
  Wire.endTransmission(); // stop transmitting
#ifdef DBG_D3
  if (dbgPrt != nullptr)
  {
    dbgPrt->println(F("KTv: Pkt txd!"));
  }
#endif
}

void KimatTv::queryButton(KTvBtn btn)
{
  uint8_t payload[1]{(uint8_t)KTvSymbol::dummy};
  memset(sendBfr2, 0, sizeof(sendBfr2));
  uint8_t lenPkt = constructPacket((uint8_t)KTvCmd::queryButton, (uint8_t)btn, 0x01, payload, sendBfr2);
  transmitPacket(lenPkt, sendBfr2);
}

void KimatTv::run()
{
  enum class MnSt00 : uint8_t
  {
    initial, // ready to send next cmd
    pollForReply,
    checkReply,
    retransmit,
    error
  };
  static bool isBtnCmdPending{};

  static MnSt00 mnSt = MnSt00::initial;
  static uint32_t tRefSend{};
  switch (mnSt)
  {
  case MnSt00::initial:
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->print(F("isUsrCmdPending isBtnCmdPending: "));
        dbgPrt->print(isUsrCmdPending); dbgPrt->println(isBtnCmdPending);
      }
#endif

    if(!isUsrCmdPending && !isBtnCmdPending){
      queryButton((KTvBtn)queryBtnCtr);
      status = KTvStatus::busy;
      tRefSend = millis();
      mnSt = MnSt00::pollForReply;
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Btn query cmd sent to device."));
      }
#endif
    }
    else if(!isUsrCmdPending && isBtnCmdPending){
      queryButton((KTvBtn)queryBtnCtr);
      status = KTvStatus::busy;
      tRefSend = millis();
      mnSt = MnSt00::pollForReply;
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Btn query cmd sent to device."));
      }
#endif
    }
    else if(isUsrCmdPending && !isBtnCmdPending){
      // send user cmd here
      status = KTvStatus::busy;
      tRefSend = millis();
      mnSt = MnSt00::pollForReply;
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Usr cmd sent to device."));
      }
#endif
    }
    else if(isUsrCmdPending && isBtnCmdPending){
      queryButton((KTvBtn)queryBtnCtr);
      status = KTvStatus::busy;
      tRefSend = millis();
      mnSt = MnSt00::pollForReply;
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Btn query cmd sent to device."));
      }
#endif
    }



//     if (isUsrCmdPending)
//     {
//       // send user cmd
//       status = KTvStatus::busy;
//       tRefSend = millis();
//       mnSt = MnSt00::pollForReply;
// #ifdef DBG_D4B
//       if (dbgPrt != nullptr)
//       {
//         dbgPrt->println(F("KTv: Usr cmd sent to device."));
//       }
// #endif
//     }
//     else
//     {
//       queryButton((KTvBtn)queryBtnCtr);
//       status = KTvStatus::busy;
//       tRefSend = millis();
//       mnSt = MnSt00::pollForReply;
// #ifdef DBG_D4B
//       if (dbgPrt != nullptr)
//       {
//         dbgPrt->println(F("KTv: Btn query cmd sent to device."));
//       }
// #endif
//     }
    break;
  case MnSt00::pollForReply:
    if (millis() - tRefSend > KTV_PERIOD_WAITBEFOREPOLL) // wait before polling
    {
      Wire.requestFrom(deviceAddress, (uint8_t)1); // poll reply here
      tRefSend = millis();
      mnSt = MnSt00::checkReply;
#ifdef DBG_D4C
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Poll reply req sent to device."));
      }
#endif
#ifdef TST_1
      uint8_t r = Wire.read(); // WA1 (a 0x00 is available in the bus just after a request)
      if (dbgPrt != nullptr)
      {
        dbgPrt->print(F("KTv: TST_1 dev reply: ")); dbgPrt->println(r);
      }
#endif
    }
    break;
  case MnSt00::checkReply:
  {
    if (Wire.available())
    {
      uint8_t b = Wire.read();
#ifdef DBG_D4A
      if (dbgPrt != nullptr)
      {
        dbgPrt->print(F("KTv: Reply fr KTv module: "));
        dbgPrt->println(b, HEX);
      }
#endif
      if (b == (uint8_t)KTvSymbol::ack)
      {
        if (isUsrCmdPending)
        {
          isUsrCmdPending = false;
          status = KTvStatus::ready;
          mnSt = MnSt00::initial;
        }
        else
        { // device ack'd a query btn cmd (unlikely to happen)
          btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
          queryBtnErrCtr++; // increment btn error ctr
          if (queryBtnErrCtr < QTY_COUNT_MAXQUERYBTNERRORS)
            mnSt = MnSt00::initial; // query the same btn in the next cycle
          else
          {
            deviceErrorCode = KTvErrorCodes::btnQueryAkd;
            mnSt = MnSt00::error;
          }
        }
      }
      else if (b == (uint8_t)KTvSymbol::nak)
      {
        if (isUsrCmdPending)
        {
          // retransmit
          // increment nak ctr
          mnSt = MnSt00::retransmit;
        }
        else
        { // device nak'd a query btn cmd
          btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
          queryBtnErrCtr++; // increment btn error ctr
          if (queryBtnErrCtr < QTY_COUNT_MAXQUERYBTNERRORS)
            mnSt = MnSt00::initial; // query the same btn in the next cycle
          else
          {
            deviceErrorCode = KTvErrorCodes::btnQueryNkd;
            mnSt = MnSt00::error;
          }
        }
      }
      else if (b == (uint8_t)KTvSymbol::busy)
      {
        // increment busy counter (no need because device will always become 'not busy'?)
        // issue another request for reply in the next cycle
        tRefSend = millis();
        mnSt = MnSt00::pollForReply;
      }
      else if (b == (uint8_t)KTvSymbol::ready)
      {
        if (isUsrCmdPending) // device did not ack/nak user cmd (unlikely to happen)
        {
          // retransmit
          // reset nak ctr
          mnSt = MnSt00::retransmit;
        }
        else
        { // device did not ack, nak nor reply to a btn query cmd (unlikely to happen)
          btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
          queryBtnErrCtr++; // increment btn error ctr
          if (queryBtnErrCtr < QTY_COUNT_MAXQUERYBTNERRORS)
            mnSt = MnSt00::initial; // query the same btn in the next cycle
          else
          {
            deviceErrorCode = KTvErrorCodes::btnQueryNoReply;
            mnSt = MnSt00::error;
          }
        }
      }
      // check button bytes here
      else if (b == (uint8_t)KTvSymbol::btnAHi || b == (uint8_t)KTvSymbol::btnALo)
      {
        btnStates[(uint8_t)KTvBtn::btnA] = (b == (uint8_t)KTvSymbol::btnAHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else if (b == (uint8_t)KTvSymbol::btnBHi || b == (uint8_t)KTvSymbol::btnBLo)
      {
        btnStates[(uint8_t)KTvBtn::btnB] = (b == (uint8_t)KTvSymbol::btnBHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else if (b == (uint8_t)KTvSymbol::btnCHi || b == (uint8_t)KTvSymbol::btnCLo)
      {
        btnStates[(uint8_t)KTvBtn::btnC] = (b == (uint8_t)KTvSymbol::btnCHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else if (b == (uint8_t)KTvSymbol::btnDHi || b == (uint8_t)KTvSymbol::btnDLo)
      {
        btnStates[(uint8_t)KTvBtn::btnD] = (b == (uint8_t)KTvSymbol::btnDHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else if (b == (uint8_t)KTvSymbol::btnEHi || b == (uint8_t)KTvSymbol::btnELo)
      {
        btnStates[(uint8_t)KTvBtn::btnE] = (b == (uint8_t)KTvSymbol::btnEHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else if (b == (uint8_t)KTvSymbol::btnFHi || b == (uint8_t)KTvSymbol::btnFLo)
      {
        btnStates[(uint8_t)KTvBtn::btnF] = (b == (uint8_t)KTvSymbol::btnFHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
        queryBtnErrCtr = 0;
        queryBtnCtr++;
        if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
          queryBtnCtr = 0;
        status = KTvStatus::ready;
        mnSt = MnSt00::initial;
      }
      else // unknown reply byte
      {
        if (isUsrCmdPending)
        {
          // retransmit
        }
        else
        { // unknown reply for a btn query
          btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
          queryBtnErrCtr++; // increment btn error ctr
          if (queryBtnErrCtr < QTY_COUNT_MAXQUERYBTNERRORS)
            mnSt = MnSt00::initial; // query the same btn in the next cycle
          else
          {
            deviceErrorCode = KTvErrorCodes::btnQueryUnkRep;
            mnSt = MnSt00::error;
          }
        }
      }
    }
    else if (millis() - tRefSend > KTV_PERIOD_TIMEOUT)
    {
      // retransmit
      if (isUsrCmdPending)
      {
      }
      else
      {
        btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
        queryBtnErrCtr++; // increment btn error ctr
        if (queryBtnErrCtr < QTY_COUNT_MAXQUERYBTNERRORS)
          mnSt = MnSt00::initial; // query the same btn in the next cycle
        else
        {
          deviceErrorCode = KTvErrorCodes::btnQueryTimeout;
          mnSt = MnSt00::error;
        }
      }
    }
  }
  break;
  case MnSt00::retransmit: // retransmit
    break;
  case MnSt00::error: // error
  {
    static uint8_t st = 0;
    switch (st)
    {
    case 0:
#ifdef DBG_D5
      if (dbgPrt != nullptr)
      {
        dbgPrt->print(F("KTv: error state: "));
        if (deviceErrorCode == KTvErrorCodes::btnQueryAkd || deviceErrorCode == KTvErrorCodes::btnQueryNkd || deviceErrorCode == KTvErrorCodes::btnQueryNoReply || deviceErrorCode == KTvErrorCodes::btnQueryUnkRep || deviceErrorCode == KTvErrorCodes::btnQueryTimeout)
        {
          dbgPrt->println(F("too many btn query error events!"));
          dbgPrt->print(F("  : last error: "));
          switch (deviceErrorCode)
          {
          case KTvErrorCodes::btnQueryAkd: // tested 2019/11/02
            dbgPrt->println(F("btn query ack'd"));
            break;
          case KTvErrorCodes::btnQueryNkd: // tested 2019/11/02
            dbgPrt->println(F("btn query nak'd"));
            break;
          case KTvErrorCodes::btnQueryNoReply: // tested 2019/11/02
            dbgPrt->println(F("btn query no reply(device is ready)"));
            break;
          case KTvErrorCodes::btnQueryUnkRep: // tested 2019/11/02
            dbgPrt->println(F("btn query unknown reply"));
            break;
          case KTvErrorCodes::btnQueryTimeout: // tested 2019/11/02
            dbgPrt->println(F("btn query no reply(timeout)"));
            break;
          default:
            break;
          }
        }
        else if (deviceErrorCode == KTvErrorCodes::userCmdAkd || deviceErrorCode == KTvErrorCodes::userCmdNkd || deviceErrorCode == KTvErrorCodes::userCmdNoReply || deviceErrorCode == KTvErrorCodes::userCmdUnkRep || deviceErrorCode == KTvErrorCodes::userCmdTimeout)
        {
          Serial.println(F("too many user cmd error events!"));
          Serial.print(F("  : last error: "));
          switch (deviceErrorCode)
          {
          case KTvErrorCodes::userCmdAkd: // tested 2019/11/02
            Serial.println(F("user cmd ack'd"));
            break;
          case KTvErrorCodes::userCmdNkd: // tested 2019/11/02
            Serial.println(F("user cmd nak'd"));
            break;
          case KTvErrorCodes::userCmdNoReply: // tested 2019/11/02
            Serial.println(F("user cmd no reply(device is ready)"));
            break;
          case KTvErrorCodes::userCmdUnkRep: // tested 2019/11/02
            Serial.println(F("user cmd unknown reply"));
            break;
          case KTvErrorCodes::userCmdTimeout: // tested 2019/11/02
            Serial.println(F("user cmd no reply(timeout)"));
            break;
          default:
            break;
          }
        }
        else if (deviceErrorCode == KTvErrorCodes::ok)
        {
          dbgPrt->println(F("device is OK!"));
        }
        else
        {
          dbgPrt->println(F("unknown device status!"));
        }
      }
#endif
      st = 1;
      break;
    case 1: // hangup
      break;
    default:
      break;
    }
  }
  break;
  default:
    break;
  }
}