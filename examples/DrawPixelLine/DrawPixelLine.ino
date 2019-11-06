// #include <KTvDefs.h>
#include <KimatTv.h>
#include "bitmap.h"

KimatTv ktv(0x38); // I2C device address of Kimat TV is 0x38

// the library calls this function when a comms error occurs
void errorHandler(KTvErrorCodes error)
{
  Serial.print(F("Error state!!! error code: "));
  Serial.println((uint8_t)error);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  while(1); // hangup
}

void setup()
{
  Serial.begin(9600);
  // ktv.attachDebug(&Serial);
  ktv.attachErrorHandler(errorHandler);
  ktv.init();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  static KTvBtnStatus btnAStPrev;
  static KTvBtnStatus btnAStCur = KTvBtnStatus::unknown;

  btnAStCur = ktv.getPinStatus(KTvBtn::btnA);
  if (btnAStPrev != btnAStCur)
  {
    Serial.print(F("user: Btn A: "));
    if (btnAStCur == KTvBtnStatus::unknown)
      Serial.println(F("unknown"));
    else if (btnAStCur == KTvBtnStatus::pressed)
      Serial.println(F("pressed"));
    else if (btnAStCur == KTvBtnStatus::released)
      Serial.println(F("released"));
    btnAStPrev = btnAStCur;
  }

  static uint8_t userSt = 0;
  switch (userSt)
  {
  case 0:
    // if (ktv.isReady())
    // {
    //   ktv.waitUntilReady(); ktv.fill(KTvColor::black);
    //   ktv.waitUntilReady(); ktv.setPixel(0,0,KTvColor::white);
    //   ktv.waitUntilReady(); ktv.setPixel(119,0,KTvColor::white);
    //   ktv.waitUntilReady(); ktv.setPixel(0,95,KTvColor::white);
    //   ktv.waitUntilReady(); ktv.setPixel(119,95,KTvColor::white);
    //   userSt = 1;
    // }
    ktv.waitUntilReady(); ktv.fill(KTvColor::black);
    ktv.waitUntilReady(); ktv.setPixel(0,0,KTvColor::white);
    ktv.waitUntilReady(); ktv.setPixel(119,0,KTvColor::white);
    ktv.waitUntilReady(); ktv.setPixel(0,95,KTvColor::white);
    ktv.waitUntilReady(); ktv.setPixel(119,95,KTvColor::white);

    ktv.waitUntilReady(); ktv.setPixel(59,47,KTvColor::white);

    userSt = 1;
    break;
  case 1:
  {
    static uint8_t x{}, y{};
    // if(ktv.getStatus() == KTvStatus::ready) // much faster
    // {
    //   ktv.drawPixels16(x,y, KTvOrientation::horizontal, 0xAAAA);
    //   x += 16;
    //   if (x >= 120){
    //     x = 0;
    //     y++;
    //     if (y >= 96)
    //     {
    //       y = 0;
    //       userSt = 2;
    //     }
    //   }
    // }

    // if(ktv.getStatus() == KTvStatus::ready) // much faster
    // {
    //   ktv.drawPixelLine(KTvOrientation::horizontal, 0, bitmap);
    //   // ktv.drawPixelLine(KTvOrientation::vertical, 0, bitmap);
    //   userSt = 2;
    // }

    // ktv.waitUntilReady();
    // for(uint8_t y = 0; y < 96; y++){
    //   ktv.drawPixelLine(KTvOrientation::horizontal, y, bitmap);
    // }

    ktv.waitUntilReady();
    for(uint8_t x = 0; x < 120; x++){
      ktv.drawPixelLine(KTvOrientation::vertical, x, bitmap);
    }

    userSt = 2;
  }
    break;
  case 2:
    break;
  default:
    break;
  }

  static uint32_t tRefHb{};
  if(millis() - tRefHb > 500){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    tRefHb = millis();
  }

  ktv.run();
}