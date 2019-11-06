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
  Serial.begin(9600);
  // Attach the error handler function to the library.
  ktv.attachErrorHandler(errorHandler);
  ktv.init(); // Initialize communication.
  ktv.waitUntilReady();// Wait for the device to become ready.
  ktv.fill(KTvColor::inverse); // Fill the screen with black color.
  ktv.delay(3000); // Wait for 3 sec to see the effect.
  ktv.waitUntilReady(); // Wait for the device to become ready.
}
void loop() {
  static uint32_t tRef; // Reference timer variable.
  static uint8_t userState = 0;
  switch (userState) {
    case 0:
      if(millis() - tRef > 3000){ // Wait for 3 sec to see the effect.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.fill(KTvColor::white); // Fill the screen with white color.
          tRef = millis(); // Start the timer.
          userState = 1; // Go to the next state.     
        }
      }
      break;
    case 1:
      if(millis() - tRef > 3000){ // Wait for 3 sec to see the effect.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.fill(KTvColor::black); // Fill the screen with black color.
          tRef = millis(); // Start the timer.
          userState = 0; // Go back to the previous state.          
        }
      }
      break; 
    default:
      break;
  }
  
  // Run other tasks here. Avoid blocking delays.
  
  ktv.run(); // Let the library run (very important).  
}
