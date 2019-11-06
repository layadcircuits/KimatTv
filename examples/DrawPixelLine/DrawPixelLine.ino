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

// bitmap source
uint16_t bitmap[8] = {
  0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0xAABB, 0xCCDD
};

void setup() {
  Serial.begin(9600);
  // Attach the error handler function to the library.
  ktv.attachErrorHandler(errorHandler);
  ktv.init(); // Initialize communication.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.fill(KTvColor::black); // Fill the screen with black color. 

  // Draw bitmap lines using a blocking technique.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  // Render the bitmap pattern as a horizontal bitmap line.
  for (uint8_t y = 0; y < 96; y++) {
    ktv.drawPixelLine(KTvOrientation::horizontal, y, bitmap);
  }
  
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.fill(KTvColor::black); // Fill the screen with black color.
}

void loop() {
  // Draw bitmap lines using a non-blocking technique.
  static uint8_t userState = 0;
  switch (userState) {
    case 0: {
        static uint8_t x = 0;
        // Render the bitmap pattern as a vertical bitmap line.
        if (ktv.isReady()) { // Wait until the device is ready.
          ktv.drawPixelLine(KTvOrientation::vertical, x, bitmap);
          x++;
          if (x >= 120) {
            userState = 1; // Done.
          }
        }
      }
      break;
    case 1:
      // do nothing
      break;
    default:
      break;
  }
  // Run other tasks here. Avoid blocking delays.
  ktv.run(); // Let the library run (very important).
}
