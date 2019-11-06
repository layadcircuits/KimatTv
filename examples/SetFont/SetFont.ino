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

  // static uint8_t userSt = 0;
  // switch(userSt){
  //   case 0:
  //     while(!ktv.isReady()); ktv.fill(KTvColor::black);
  //     while(!ktv.isReady()); ktv.setFont(KTvFont::font4x6);
  //     //////////////////////////////////01234567890123456789
  //     while(!ktv.isReady()); ktv.print("This is a 4x6 font!!");

  //     while(!ktv.isReady()); ktv.setFont(KTvFont::font6x8);
  //     while(!ktv.isReady()); ktv.setCursor(0,6);
  //     //////////////////////////////////01234567890123456789
  //     while(!ktv.isReady()); ktv.print("This is a 6x8 font!!");

  //     while(!ktv.isReady()); ktv.setFont(KTvFont::font8x8);
  //     while(!ktv.isReady()); ktv.setCursor(0,14);
  //     //////////////////////////////////01234567890123456789
  //     while(!ktv.isReady()); ktv.print("This is an 8x8 font!");

  //     userSt = 1;
  //     break;
  //   case 1:
  //     break;
  //   default:
  //     break;
  // }

//   static uint8_t userSt = 0;
//   switch(userSt){
//     case 0:
//       ktv.fill(KTvColor::black);
// Serial.println(F("Test point 1"));
//       ktv.delay(1000);
// Serial.println(F("Test point 2"));
//       ktv.setFont(KTvFont::font4x6);
// Serial.println(F("Test point 3"));
//       ktv.delay(1000);
// Serial.println(F("Test point 4"));
//       ///////////01234567890123456789
//       ktv.print("This is a 4x6 font!!");

// Serial.println(F("Test point 5"));
//       ktv.delay(1000);
// Serial.println(F("Test point 6"));
//       ktv.setFont(KTvFont::font6x8);
// Serial.println(F("Test point 7"));
//       ktv.delay(1000);

//       ktv.setCursor(0,6);

//       ktv.delay(1000);

//       ///////////01234567890123456789
//       ktv.print("This is a 6x8 font!!");
// Serial.println(F("Test point 8"));
//       ktv.delay(1000);
// Serial.println(F("Test point 9"));
//       ktv.setFont(KTvFont::font8x8);
// Serial.println(F("Test point 10"));
//       ktv.delay(1000);
// Serial.println(F("Test point 11"));
//       ktv.setCursor(0,14);
// Serial.println(F("Test point 12"));
//       ktv.delay(1000);
// Serial.println(F("Test point 13"));
//       ///////////01234567890123456789
//       ktv.print("This is an 8x8 font!");
//       userSt = 1;
//       break;
//     case 1:
//       break;
//     default:
//       break;
//   }



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
        ktv.setFont(KTvFont::font4x6);
        userSt = 2;
      }
      break;
    case 2:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("This is a 4x6 font!!");
        userSt = 3;
      }
      break;
    case 3:
      if(ktv.isReady()){
        ktv.setFont(KTvFont::font6x8);
        userSt = 4;
      }
      break;
    case 4:
      if(ktv.isReady()){
        ktv.setCursor(0,6);
        userSt = 5;
      }
      break;
    case 5:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("This is a 6x8 font!!");
        userSt = 6;
      }
      break;
    case 6:
      if(ktv.isReady()){
        ktv.setFont(KTvFont::font8x8);
        userSt = 7;
      }
      break;
    case 7:
      if(ktv.isReady()){
        ktv.setCursor(0,14);
        userSt = 8;
      }
      break;
    case 8:
      if(ktv.isReady()){
        ///////////01234567890123456789
        ktv.print("This is an 8x8 font!");
        userSt = 9;
      }
      break;
    case 9:
      // do nothing
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