// #include <KTvDefs.h>
#include <KimatTv.h>

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
    //   while(!ktv.isReady()); ktv.fill(KTvColor::black);
    //   while(!ktv.isReady()); ktv.setPixel(0,0,KTvColor::white);
    //   while(!ktv.isReady()); ktv.setPixel(119,0,KTvColor::white);
    //   while(!ktv.isReady()); ktv.setPixel(0,95,KTvColor::white);
    //   while(!ktv.isReady()); ktv.setPixel(119,95,KTvColor::white);
    //   userSt = 1;
    // }
    while(!ktv.isReady()); ktv.fill(KTvColor::black);
    while(!ktv.isReady()); ktv.setPixel(0,0,KTvColor::white);
    while(!ktv.isReady()); ktv.setPixel(119,0,KTvColor::white);
    while(!ktv.isReady()); ktv.setPixel(0,95,KTvColor::white);
    while(!ktv.isReady()); ktv.setPixel(119,95,KTvColor::white);
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

    if(ktv.getStatus() == KTvStatus::ready) // much faster
    {
      ktv.drawPixels16(x,y, KTvOrientation::vertical, 0xAAAA);
      y += 16;
      if (y >= 96){
        y = 0;
        x++;
        if (x >= 120)
        {
          x = 0;
          userSt = 2;
        }
      }
    }
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