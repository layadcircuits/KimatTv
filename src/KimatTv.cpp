#include "KimatTv.h"

void KimatTv::init()
{
  Wire.begin();
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

bool KimatTv::isReady(){
  return (status == KTvStatus::ready) ? true : false;  
}

void KimatTv::waitUntilReady(){
  run();
  while(status != KTvStatus::ready){
    run();
  }
}

void KimatTv::setCursor(uint8_t x, uint8_t y){
  if (status == KTvStatus::busy) return;
  uint8_t payload[2]{x,y};
  memset(sendBfr,0,sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::setCursor, (uint8_t)KTvSymbol::dummy, 0x02, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::setFont(KTvFont font){
  if (status == KTvStatus::busy) return;
  uint8_t payload[1]{(uint8_t)KTvSymbol::dummy};
  memset(sendBfr,0,sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::setFont, (uint8_t)font, 0x01, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::queryButton(KTvBtn btn)
{
  uint8_t payload[1]{(uint8_t)KTvSymbol::dummy};
  memset(sendBfr2, 0, sizeof(sendBfr2));
  uint8_t lenPkt = constructPacket((uint8_t)KTvCmd::queryButton, (uint8_t)btn, 0x01, payload, sendBfr2);
  transmitPacket(lenPkt, sendBfr2);
}

void KimatTv::print(char *str){
  if (status == KTvStatus::busy) return;
  uint8_t payload[KTV_LEN_MAX_PAYLOAD]{};
  uint8_t i;
  for(i=0; i<KTV_LEN_MAX_PAYLOAD; i++){
    if(str[i]) payload[i] = str[i];
    else break;
  }
  memset(sendBfr,0,sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::print, (uint8_t)KTvSymbol::dummy, i, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::fill(KTvColor color)
{
  if (status == KTvStatus::busy)
    return;
  uint8_t payload[1]{(uint8_t)KTvSymbol::dummy};
  memset(sendBfr, 0, sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::fill, (uint8_t)color, 0x01, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::setPixel(uint8_t x, uint8_t y, KTvColor color){
#ifdef DBG_D6
  if(dbgPrt != nullptr){
    dbgPrt->print(F("KTv: setPixel():tst pt 1: x,y: ")); dbgPrt->print(x);
    dbgPrt->print(F(",")); dbgPrt->println(y);
  }
#endif
  if (status == KTvStatus::busy) return;
#ifdef DBG_D6
  if(dbgPrt != nullptr){
    dbgPrt->print(F("KTv: setPixel():tst pt 2: x,y: ")); dbgPrt->print(x);
    dbgPrt->print(F(",")); dbgPrt->println(y);
  }
#endif
  // uint8_t payload[]{x,y,(uint8_t)color};
  uint8_t payload[]{x,y};
  memset(sendBfr,0,sizeof(sendBfr));
  // pktLenToTx =  constructPacket((uint8_t)KTvCmd::setPixel, (uint8_t)KTvSymbol::dummy, 0x03, payload, sendBfr);
  pktLenToTx = constructPacket((uint8_t)KTvCmd::setPixel, (uint8_t)color, 0x02, payload, sendBfr);
#ifdef DBG_D6
  if(dbgPrt != nullptr){
    dbgPrt->println(F("KTv: setPixel(): constructed pkt: "));
    printU8Array(sendBfr, pktLenToTx);
    dbgPrt->println();
  }
#endif
  isUsrCmdPending = true;
}

void KimatTv::tone(KTvToneCmd cmdType, uint16_t freq, uint32_t duration){
  if (status == KTvStatus::busy) return;
  uint8_t payload[]{
    (uint8_t)((freq>>8) & 0xFF),
    (uint8_t)(freq & 0xFF),
    (uint8_t)((duration>>24) & 0xFF),
    (uint8_t)((duration>>16) & 0xFF),
    (uint8_t)((duration>>8) & 0xFF),
    (uint8_t)((duration) & 0xFF)
  };
  memset(sendBfr,0,sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::tone, (uint8_t)cmdType, 0x06, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::startTone(uint16_t freq, uint32_t duration){
  tone(KTvToneCmd::start, freq, duration);
}

void KimatTv::stopTone(){
  tone(KTvToneCmd::stop, 0, 0);
}

void KimatTv::delay(uint32_t period){
  static uint32_t tRef{};
  tRef = millis();
  while(millis() - tRef <= period){
    run();
  }
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
      dbgPrt->println();
      dbgPrt->print(F("isUsrCmdPending isBtnCmdPending: "));
      dbgPrt->print(isUsrCmdPending);
      dbgPrt->println(isBtnCmdPending);
    }
#endif

    if (isUsrCmdPending && !isBtnCmdPending)
    {
      // send user cmd here
      transmitPacket(pktLenToTx, sendBfr);
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Usr cmd sent to device."));
      }
#endif
    }
    else
    {
      queryButton((KTvBtn)queryBtnCtr);
#ifdef DBG_D4B
      if (dbgPrt != nullptr)
      {
        dbgPrt->println(F("KTv: Btn query cmd sent to device."));
      }
#endif
    }
    status = KTvStatus::busy;
    tRefSend = millis();
    mnSt = MnSt00::pollForReply;
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
        dbgPrt->print(F("KTv: TST_1 device reply: "));
        dbgPrt->println(r);
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
      KTvSymbol rcvdReply = (KTvSymbol)b;

      if (isUsrCmdPending && !isBtnCmdPending) // user cmd sent and waiting for a reply
      {
        if (rcvdReply == KTvSymbol::ack) // the device ack'd the usr cmd
        {
#ifdef DBG_D4AU
          if (dbgPrt != nullptr)
          {
            dbgPrt->println(F("KTv: user cmd ack'd"));
          }
#endif
          if (userCmdErrCtr)
          {
            userCmdErrCtr = 0;
#ifdef DBG_D4AU
            if (dbgPrt != nullptr)
            {
              dbgPrt->print(F("userCmdErrCtr reset: "));
              dbgPrt->println(userCmdErrCtr);
            }
#endif
          }
          isUsrCmdPending = false;
          isBtnCmdPending = true;
          // status = KTvStatus::ready;
          mnSt = MnSt00::initial;
        }
        else // the device did not ack the usr cmd
        {
          userCmdErrCtr++;
#ifdef DBG_D4AU
          if (dbgPrt != nullptr)
          {
            dbgPrt->print(F("KTv: userCmdErrCtr: "));
            dbgPrt->println(userCmdErrCtr);
          }
#endif
          if (userCmdErrCtr < QTY_COUNT_MAXERROREVENTS)
          {
            if (rcvdReply == KTvSymbol::busy)
            {
              tRefSend = millis();
              mnSt = MnSt00::pollForReply;
            }
            else
              mnSt = MnSt00::initial;
          }
          else
          {
            // if (rcvdReply == KTvSymbol::ack)
            //   deviceErrorCode = KTvErrorCodes::userCmdAkd;
            if (rcvdReply == KTvSymbol::nak)
              deviceErrorCode = KTvErrorCodes::userCmdNkd;
            else if (rcvdReply == KTvSymbol::ready)
              deviceErrorCode = KTvErrorCodes::userCmdNoReplyReady;
            else if (rcvdReply == KTvSymbol::busy)
              deviceErrorCode = KTvErrorCodes::userCmdNoReplyBusy;
            else
              deviceErrorCode = KTvErrorCodes::userCmdUnkRep;
            mnSt = MnSt00::error;
          }
        }
      }

      else // btn query cmd sent and waiting for a reply
      {
        if ( // the device replied with a button byte
            rcvdReply == KTvSymbol::btnAHi || rcvdReply == KTvSymbol::btnALo ||
            rcvdReply == KTvSymbol::btnBHi || rcvdReply == KTvSymbol::btnBLo ||
            rcvdReply == KTvSymbol::btnCHi || rcvdReply == KTvSymbol::btnCLo ||
            rcvdReply == KTvSymbol::btnDHi || rcvdReply == KTvSymbol::btnDLo ||
            rcvdReply == KTvSymbol::btnEHi || rcvdReply == KTvSymbol::btnELo ||
            rcvdReply == KTvSymbol::btnFHi || rcvdReply == KTvSymbol::btnFLo)
        {
          if (rcvdReply == KTvSymbol::btnAHi || rcvdReply == KTvSymbol::btnALo)
            btnStates[(uint8_t)KTvBtn::btnA] = (rcvdReply == KTvSymbol::btnAHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
          else if (rcvdReply == KTvSymbol::btnBHi || rcvdReply == KTvSymbol::btnBLo)
            btnStates[(uint8_t)KTvBtn::btnB] = (rcvdReply == KTvSymbol::btnBHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
          else if (rcvdReply == KTvSymbol::btnCHi || rcvdReply == KTvSymbol::btnCLo)
            btnStates[(uint8_t)KTvBtn::btnC] = (rcvdReply == KTvSymbol::btnCHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
          else if (rcvdReply == KTvSymbol::btnDHi || rcvdReply == KTvSymbol::btnDLo)
            btnStates[(uint8_t)KTvBtn::btnD] = (rcvdReply == KTvSymbol::btnDHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
          else if (rcvdReply == KTvSymbol::btnEHi || rcvdReply == KTvSymbol::btnELo)
            btnStates[(uint8_t)KTvBtn::btnE] = (rcvdReply == KTvSymbol::btnEHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
          else if (rcvdReply == KTvSymbol::btnFHi || rcvdReply == KTvSymbol::btnFLo)
            btnStates[(uint8_t)KTvBtn::btnF] = (rcvdReply == KTvSymbol::btnFHi) ? KTvBtnStatus::pressed : KTvBtnStatus::released;
#ifdef DBG_D4AB
          if (dbgPrt != nullptr)
          {
            dbgPrt->println(F("KTv: btn query rcvd a btn reply"));
          }
#endif
          if (btnCmdErrCtr)
          {
            btnCmdErrCtr = 0;
#ifdef DBG_D4AB
            if (dbgPrt != nullptr)
            {
              dbgPrt->print(F("btnCmdErrCtr reset: "));
              dbgPrt->println(btnCmdErrCtr);
            }
#endif
          }

          // if (!isUsrCmdPending && !isBtnCmdPending)
          // {
          //   status = KTvStatus::ready;
          //   mnSt = MnSt00::initial;
          // }
          // else if (!isUsrCmdPending && isBtnCmdPending)
          // {
          //   isBtnCmdPending = false;
          //   status = KTvStatus::ready;
          //   mnSt = MnSt00::initial;
          // }
          // else if (isUsrCmdPending && isBtnCmdPending)
          // {
          //   isBtnCmdPending = false;
          //   status = KTvStatus::ready;
          //   mnSt = MnSt00::initial;
          // }

          if (isBtnCmdPending){
            isBtnCmdPending = false;
            status = KTvStatus::ready;
          }
          else if(!isUsrCmdPending && !isBtnCmdPending ){
            status = KTvStatus::ready;
          }

          queryBtnCtr++;
          if ((KTvBtn)queryBtnCtr >= KTvBtn::max)
            queryBtnCtr = 0;

          // status = KTvStatus::ready;
          mnSt = MnSt00::initial;
        }
        else // the device did not reply with a button byte
        {
          btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
          btnCmdErrCtr++;
#ifdef DBG_D4AB
          if (dbgPrt != nullptr)
          {
            dbgPrt->print(F("KTv: btnCmdErrCtr: "));
            dbgPrt->println(btnCmdErrCtr);
          }
#endif
          if (btnCmdErrCtr < QTY_COUNT_MAXERROREVENTS)
          {
            if (rcvdReply == KTvSymbol::busy)
            {
              tRefSend = millis();
              mnSt = MnSt00::pollForReply;
            }
            else
              mnSt = MnSt00::initial;
          }
          else
          {
            if (rcvdReply == KTvSymbol::ack)
              deviceErrorCode = KTvErrorCodes::btnQueryAkd;
            else if (rcvdReply == KTvSymbol::nak)
              deviceErrorCode = KTvErrorCodes::btnQueryNkd;
            else if (rcvdReply == KTvSymbol::ready)
              deviceErrorCode = KTvErrorCodes::btnQueryNoReplyReady;
            else if (rcvdReply == KTvSymbol::busy)
              deviceErrorCode = KTvErrorCodes::btnQueryNoReplyBusy;
            else
              deviceErrorCode = KTvErrorCodes::btnQueryUnkRep;
            mnSt = MnSt00::error;
          }
        }
      }
    }

    else if (millis() - tRefSend > KTV_PERIOD_TIMEOUT)
    {
      if (isUsrCmdPending && !isBtnCmdPending) // user cmd sent and the device didn't reply within KTV_PERIOD_TIMEOUT
      {
        userCmdErrCtr++;
#ifdef DBG_D4AU
        if (dbgPrt != nullptr)
        {
          dbgPrt->println(F("KTv: user cmd reply timed out!"));
          dbgPrt->print(F("KTv: userCmdErrCtr: "));
          dbgPrt->println(userCmdErrCtr);
        }
#endif
        if (userCmdErrCtr < QTY_COUNT_MAXERROREVENTS)
          mnSt = MnSt00::initial;
        else
        {
          deviceErrorCode = KTvErrorCodes::userCmdTimeout;
          mnSt = MnSt00::error;
        }
      }
      else // btn query cmd sent and the device didn't reply within KTV_PERIOD_TIMEOUT
      {
        btnStates[queryBtnCtr] = KTvBtnStatus::unknown;
        btnCmdErrCtr++;
#ifdef DBG_D4AB
        if (dbgPrt != nullptr)
        {
          dbgPrt->println(F("KTv: btn cmd reply timed out!"));
          dbgPrt->print(F("KTv: btnCmdErrCtr: "));
          dbgPrt->println(btnCmdErrCtr);
        }
#endif
        if (btnCmdErrCtr < QTY_COUNT_MAXERROREVENTS)
          mnSt = MnSt00::initial;
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
        if (deviceErrorCode == KTvErrorCodes::btnQueryAkd ||
            deviceErrorCode == KTvErrorCodes::btnQueryNkd ||
            deviceErrorCode == KTvErrorCodes::btnQueryNoReplyBusy ||
            deviceErrorCode == KTvErrorCodes::btnQueryNoReplyReady ||
            deviceErrorCode == KTvErrorCodes::btnQueryUnkRep ||
            deviceErrorCode == KTvErrorCodes::btnQueryTimeout)
        {
          dbgPrt->println(F("too many btn query error events!"));
          dbgPrt->print(F("  : last error: "));
          switch (deviceErrorCode)
          {
          case KTvErrorCodes::btnQueryAkd:
            dbgPrt->println(F("btn query ack'd"));
            break;
          case KTvErrorCodes::btnQueryNkd:
            dbgPrt->println(F("btn query nak'd"));
            break;
          case KTvErrorCodes::btnQueryNoReplyBusy:
            dbgPrt->println(F("btn query no reply(device is always busy)"));
            break;
          case KTvErrorCodes::btnQueryNoReplyReady:
            dbgPrt->println(F("btn query no reply(device is always ready)"));
            break;
          case KTvErrorCodes::btnQueryUnkRep:
            dbgPrt->println(F("btn query unknown reply"));
            break;
          case KTvErrorCodes::btnQueryTimeout:
            dbgPrt->println(F("btn query no reply(timeout)"));
            break;
          default:
            break;
          }
        }
        else if ( //deviceErrorCode == KTvErrorCodes::userCmdAkd ||
                 deviceErrorCode == KTvErrorCodes::userCmdNkd ||
                 deviceErrorCode == KTvErrorCodes::userCmdNoReplyBusy ||
                 deviceErrorCode == KTvErrorCodes::userCmdNoReplyReady ||
                 deviceErrorCode == KTvErrorCodes::userCmdUnkRep ||
                 deviceErrorCode == KTvErrorCodes::userCmdTimeout)
        {
          dbgPrt->println(F("too many user cmd error events!"));
          dbgPrt->print(F("  : last error: "));
          switch (deviceErrorCode)
          {
          // case KTvErrorCodes::userCmdAkd:
          //   dbgPrt->println(F("user cmd ack'd"));
          //   break;
          case KTvErrorCodes::userCmdNkd:
            dbgPrt->println(F("user cmd nak'd"));
            break;
          case KTvErrorCodes::userCmdNoReplyBusy:
            dbgPrt->println(F("user cmd no reply(device is always busy)"));
            break;
          case KTvErrorCodes::userCmdNoReplyReady:
            dbgPrt->println(F("user cmd no reply(device is always ready)"));
            break;
          case KTvErrorCodes::userCmdUnkRep:
            dbgPrt->println(F("user cmd unknown reply"));
            break;
          case KTvErrorCodes::userCmdTimeout:
            dbgPrt->println(F("user cmd no reply(timeout)"));
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
    case 1: //
      if(errorHandler !=nullptr) errorHandler(deviceErrorCode);
      st = 2;
      break;
    case 2: // hangup
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

void KimatTv::drawPixels16(uint8_t x, uint8_t y, KTvOrientation orientation, uint16_t binary){
#ifdef DBG_D7
  if(dbgPrt != nullptr){
    dbgPrt->println(F("KTv: Test point 1: drawPixels16!"));
  }
#endif
  if (status == KTvStatus::busy)
    return;
#ifdef DBG_D7
  if(dbgPrt != nullptr){
    dbgPrt->println(F("KTv: Test point 2: drawPixels16!"));
    dbgPrt->print(F("KTv: Test point 2 bitmap: 0x"));
    dbgPrt->println(binary, HEX);
  }
#endif
  uint8_t payload[]{x, y, (uint8_t)(binary >> 8), (uint8_t)binary};
  memset(sendBfr, 0, sizeof(sendBfr));
  pktLenToTx = constructPacket((uint8_t)KTvCmd::drawPixels16, (uint8_t)orientation, 4, payload, sendBfr);
  isUsrCmdPending = true;
}

void KimatTv::drawPixelLine(KTvOrientation orientation, uint8_t offset, uint16_t *ptr){
#ifdef DBG_D8
  if(dbgPrt != nullptr){
    dbgPrt->println(F("KTv: Test point 1: drawPixelLine!"));
  }
#endif
  if (status == KTvStatus::busy)
    return;
#ifdef DBG_D8
  if(dbgPrt != nullptr){
    dbgPrt->println(F("KTv: Test point 2: drawPixelLine!"));
  }
#endif
  if(orientation == KTvOrientation::horizontal){
    for(uint8_t x=0; x<8;x++){ // 8 x 16 = 128
      // while(!isReady());
      waitUntilReady();
      drawPixels16(x*16, offset, KTvOrientation::horizontal, *(ptr+x));
    }
  }
  else if(orientation == KTvOrientation::vertical){
    for(uint8_t y=0; y<6;y++){ // 6 x 16 = 96
      // while(!isReady());
      waitUntilReady();
      drawPixels16(offset, y*16, KTvOrientation::vertical, *(ptr+y));
    }
  }
}