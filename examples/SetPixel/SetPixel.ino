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
}
void loop() {
  static uint8_t userState = 0;
  switch (userState) {
    case 0:
      if (ktv.isReady()) { // Wait until the device is ready.
        ktv.fill(KTvColor::black); // Fill the screen with black color.
        userState = 1; // Go to the next state.
      }
      break;
    case 1:{ // This routines fills the screen with white pixels 
      // from left to right, and top to bottom.
      static uint8_t x{}, y{};
      if(ktv.getStatus() == KTvStatus::ready){ // Wait until the device is ready.
        ktv.setPixel(x, y, KTvColor::white);
        x++;
        if (x >= 120){
          x = 0;
          y++;
          if (y >= 96){
            userState = 2; // We are done.
          }
        }
      }
    }
      break;
    case 2:
      // do nothing
      break;
    default:
      break;
  }  
  // Run other tasks here. Avoid blocking delays.  
  ktv.run(); // Let the library run (very important).  
}
