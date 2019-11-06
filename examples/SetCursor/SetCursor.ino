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
      if(ktv.isReady()){
        ktv.fill(KTvColor::black);
        userSt = 1;
      }
      break;
    case 1:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("Hello world! 0123456");
        // ktv.print(F("Hello world! 0123456"));
        userSt = 2;
      }
      break;
    case 2:
    {
      if(ktv.isReady()){
        ktv.setCursor(0,8);
        userSt = 3;
      }
    }
      break;
    case 3:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("ABCDEFGHIJKLMNOPQRST");
        userSt = 4;
      }
      break;
    case 4:
      if(ktv.isReady()){
        ktv.setCursor(0,16);
        userSt = 5;
      }
      break;
    case 5:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("abcdefghijklmnopqrst");
        userSt = 6;
      }
      break;
    case 6:
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