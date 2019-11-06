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
  Serial.println(F("Kimat TV Module"));
  // Attach the error handler function to the library.
  ktv.attachErrorHandler(errorHandler);
  ktv.init(); // Initialize communication.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.fill(KTvColor::inverse); // Invert the current pixel colors.

  ktv.delay(3000); // Wait for 3 sec to see the effect.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.fill(KTvColor::black); // Fill the screen with black color.

  ktv.delay(3000); // Wait for 3 sec to see the effect.
  ktv.waitUntilReady(); // Wait for the device to become ready.    
  ktv.fill(KTvColor::white); // Fill the screen with white color.
  ktv.waitUntilReady(); // Wait for the device to become ready.

  ktv.delay(3000); // Wait for 3 sec to see the effect.
  ktv.waitUntilReady(); // Wait for the device to become ready.
  ktv.fill(KTvColor::black); // Fill the screen with black color.

  ///////////01234567890123456789
  ktv.print("ABCDEFGHIJKLMNOPQRST"); // Print a string.
  ktv.waitUntilReady(); // Wait for the device to become ready.
}
void loop() {
  // Run other tasks here. Avoid blocking delays.
  ktv.run(); // Let the library run (very important).
}
