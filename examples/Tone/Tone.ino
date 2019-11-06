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
  ktv.attachErrorHandler(errorHandler);
  ktv.init();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  static KTvBtnStatus btnAStPrev;
  static KTvBtnStatus btnAStCur = KTvBtnStatus::unknown;

  btnAStCur = ktv.getPinStatus(KTvBtn::btnA);
  if(btnAStPrev != btnAStCur){
    Serial.print(F("user: Btn A: "));
    if(btnAStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnAStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnAStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnAStPrev = btnAStCur;
  }

  static uint8_t userSt = 0;
  switch(userSt){
    case 0:
      while(!ktv.isReady()); ktv.fill(KTvColor::black);
      while(!ktv.isReady()); ktv.setCursor(0,0);
      //////////////////////////////////01234567890123456789
      while(!ktv.isReady()); ktv.print("500 Hz, 1000 ms     ");
      while(!ktv.isReady()); ktv.startTone(500, 1000);
      ktv.delay(1000);

      while(!ktv.isReady()); ktv.setCursor(0,8);
      //////////////////////////////////01234567890123456789
      while(!ktv.isReady()); ktv.print("5000 Hz, 5000 ms    ");
      while(!ktv.isReady()); ktv.startTone(5000, 5000);
      ktv.delay(5000);

      while(!ktv.isReady()); ktv.setCursor(0,16 );
      //////////////////////////////////01234567890123456789
      while(!ktv.isReady()); ktv.print("10000 Hz, 3000 ms   ");
      while(!ktv.isReady()); ktv.startTone(10000, 3000);
      ktv.delay(3000);

      userSt = 1;
      break;
    case 1:
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