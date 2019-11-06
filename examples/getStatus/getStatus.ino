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
  // Get the current status of the Kimat TV module.
  KTvStatus deviceStatus = ktv.getStatus();
  if(deviceStatus == KTvStatus::ready)
    Serial.println(F("Kimat TV status: ready!"));
  else if(deviceStatus == KTvStatus::busy)
    Serial.println(F("Kimat TV status: busy!"));
  else if(deviceStatus == KTvStatus::error)
    Serial.println(F("Kimat TV status: error!"));
  else
    Serial.println(F("Kimat TV status: unknown!"));
    
  // Run other tasks here. Avoid blocking delays.  
  ktv.run(); // Let the library run (very important).
}
