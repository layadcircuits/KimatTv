// #include <KTvDefs.h>
#include <KimatTv.h>

KimatTv ktv(0x08);

void setup()
{
  Serial.begin(9600);
  ktv.attachDebug(&Serial);
  ktv.init();
}

void loop()
{
  // static KTvBtnStatus btnAStPrev{}, btnBStPrev{}, btnCStPrev{}, btnDStPrev{}, btnEStPrev{}, btnFStPrev{};
  static KTvBtnStatus btnAStPrev{KTvBtnStatus::released}, btnBStPrev{KTvBtnStatus::released}, btnCStPrev{KTvBtnStatus::released}, btnDStPrev{KTvBtnStatus::released}, btnEStPrev{KTvBtnStatus::released}, btnFStPrev{KTvBtnStatus::released};
  static KTvBtnStatus btnAStCur{}, btnBStCur{}, btnCStCur{}, btnDStCur{}, btnEStCur{}, btnFStCur{};

  btnAStCur = ktv.getPinStatus(KTvBtn::btnA);
  if(btnAStPrev != btnAStCur){
    Serial.print(F("user: Btn A: "));
    if(btnAStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnAStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnAStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnAStPrev = btnAStCur;
  }

  btnBStCur = ktv.getPinStatus(KTvBtn::btnB);
  if(btnBStPrev != btnBStCur){
    Serial.print(F("user: Btn B: "));
    if(btnBStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnBStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnBStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnBStPrev = btnBStCur;
  }

  btnCStCur = ktv.getPinStatus(KTvBtn::btnC);
  if(btnCStPrev != btnCStCur){
    Serial.print(F("user: Btn C: "));
    if(btnCStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnCStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnCStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnCStPrev = btnCStCur;
  }

  btnDStCur = ktv.getPinStatus(KTvBtn::btnD);
  if(btnDStPrev != btnDStCur){
    Serial.print(F("user: Btn D: "));
    if(btnDStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnDStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnDStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnDStPrev = btnDStCur;
  }

  btnEStCur = ktv.getPinStatus(KTvBtn::btnE);
  if(btnEStPrev != btnEStCur){
    Serial.print(F("user: Btn E: "));
    if(btnEStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnEStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnEStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnEStPrev = btnEStCur;
  }

  btnFStCur = ktv.getPinStatus(KTvBtn::btnF);
  if(btnFStPrev != btnFStCur){
    Serial.print(F("user: Btn F: "));
    if(btnFStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if(btnFStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if(btnFStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnFStPrev = btnFStCur;
  }

  ktv.run();
}