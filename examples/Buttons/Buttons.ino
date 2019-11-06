#include <KimatTv.h>

KimatTv ktv(0x38); // I2C device address of Kimat TV is 0x38.

// The library will call this function when a communication error occurs.
void errorHandler(KTvErrorCodes error)
{
  Serial.print(F("Error state!!! Error code: "));
  Serial.println((uint8_t)error); // Print out the error code (see the datasheet for more info).
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  while(1); // Hangup (or do something else here). Resetting both the Kimat TV module and the host is recommended.
}

void setup()
{
  Serial.begin(9600);
  ktv.attachErrorHandler(errorHandler); // Attach the error handler function to the library.
  ktv.init(); // Initialize communication.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  static KTvBtnStatus btnAStPrev{KTvBtnStatus::released}, btnBStPrev{KTvBtnStatus::released}, btnCStPrev{KTvBtnStatus::released}, btnDStPrev{KTvBtnStatus::released}, btnEStPrev{KTvBtnStatus::released}, btnFStPrev{KTvBtnStatus::released};
  static KTvBtnStatus btnAStCur{}, btnBStCur{}, btnCStCur{}, btnDStCur{}, btnEStCur{}, btnFStCur{};

  btnAStCur = ktv.getPinStatus(KTvBtn::btnA);
  if(btnAStPrev != btnAStCur){
    Serial.print(F("user: Btn A: "));
    if(btnAStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnAStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnAStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnAStPrev = btnAStCur;
  }

  btnBStCur = ktv.getPinStatus(KTvBtn::btnB);
  if(btnBStPrev != btnBStCur){
    Serial.print(F("user: Btn B: "));
    if(btnBStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnBStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnBStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnBStPrev = btnBStCur;
  }

  btnCStCur = ktv.getPinStatus(KTvBtn::btnC);
  if(btnCStPrev != btnCStCur){
    Serial.print(F("user: Btn C: "));
    if(btnCStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnCStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnCStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnCStPrev = btnCStCur;
  }

  btnDStCur = ktv.getPinStatus(KTvBtn::btnD);
  if(btnDStPrev != btnDStCur){
    Serial.print(F("user: Btn D: "));
    if(btnDStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnDStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnDStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnDStPrev = btnDStCur;
  }

  btnEStCur = ktv.getPinStatus(KTvBtn::btnE);
  if(btnEStPrev != btnEStCur){
    Serial.print(F("user: Btn E: "));
    if(btnEStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnEStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnEStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnEStPrev = btnEStCur;
  }

  btnFStCur = ktv.getPinStatus(KTvBtn::btnF);
  if(btnFStPrev != btnFStCur){
    Serial.print(F("user: Btn F: "));
    if(btnFStCur == KTvBtnStatus::unknown) Serial.println(F("unknown")); // Device was not able to update button status in the last cycle
    else if(btnFStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnFStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnFStPrev = btnFStCur;
  }
  
  static uint32_t tRefHb{};
  if(millis() - tRefHb > 500){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    tRefHb = millis();
  }

  ktv.run();
}