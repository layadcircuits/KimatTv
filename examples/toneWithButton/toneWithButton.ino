#include <KimatTv.h>
KimatTv ktv(0x38); // I2C device address of Kimat TV is 0x38.
// The library will call this function when a communication error occurs.
void errorHandler(KTvErrorCodes error) {
  Serial.print(F("Error state!!! Error code: "));
  // Print out the error code (see the datasheet for more info).
  Serial.println((uint8_t)error);
  while (1); // Hangup (or do something else here).
  // Resetting both the Kimat TV module and the host is recommended.
}
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  // Attach the error handler function to the library.
  ktv.attachErrorHandler(errorHandler);
  ktv.init(); // Initialize communication.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.startTone(3000, 3000); // Play a 3-kHz tone for 3 seconds.
  ktv.delay(3000); // Wait for 3 sec before sending a new command.
  ktv.waitUntilReady(); // Wait for the device to become ready.
}

void loop() {
  static KTvBtnStatus btnAStPrev;
  static KTvBtnStatus btnAStCur = KTvBtnStatus::unknown;

  btnAStCur = ktv.getPinStatus(KTvBtn::btnA);
  if (btnAStPrev != btnAStCur) {
    Serial.print(F("user: Btn A: "));
    if (btnAStCur == KTvBtnStatus::unknown) Serial.println(F("unknown"));
    else if (btnAStCur == KTvBtnStatus::pressed) Serial.println(F("pressed"));
    else if (btnAStCur == KTvBtnStatus::released) Serial.println(F("released"));
    btnAStPrev = btnAStCur;
  }

  static uint32_t tRef; // Reference timer variable.
  static uint8_t userState = 0;
  switch (userState) {
    case 0:
      if (ktv.isReady()) { // Wait until the device is ready.
        ktv.startTone(500, 1000); // Play a 500-Hz tone for 1 sec.
        tRef = millis(); // Start the timer.
        userState = 1; // Go to the next state.
      }
      break;
    case 1:
      if (millis() - tRef > 1000) { // Wait for the current tone to finish playing.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.startTone(5000, 5000); // Play a 5-kHz tone for 5 seconds.
          tRef = millis(); // Start the timer.
          userState = 2; // Go to the next state.
        }
      }
      break;
    case 2:
      if (millis() - tRef > 5000) { // Wait for the current tone to finish playing.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.startTone(10000, 10000); // Play a 10-kHz tone for 10 seconds.
          tRef = millis(); // Start the timer.
          userState = 3; // Go to the next state.
        }
      }
      break;
    case 3:
      if (millis() - tRef > 7000) { // Wait for 3 seconds.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.stopTone(); // Stop the current tone.
          userState = 4; // Done.
        }
      }
      break;
    case 4:
      // do nothing
      break;
    default:
      break;
  }
  
  // Run other tasks here. Avoid blocking delays.
  static uint32_t tRefHb{};
  if (millis() - tRefHb > 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    tRefHb = millis();
  }
  ktv.run(); // Let the library run (very important).
}
