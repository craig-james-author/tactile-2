/*----------------------------------------------------------------------
 * Test sketch for the libraries/Tactile/AudioFileManager module 
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TeensyUtils.h"
#include "AudioFileManager.h"

TeensyUtils *tc;
AudioFileManager *fm;

void setup() {
  tc = TeensyUtils::setup();
  setLogLevel(2);
  fm = new AudioFileManager(tc);
}

void loop() {
  delay(1000);
  tc->turnLedOn();
  delay(250);
  tc->turnLedOff();
}
