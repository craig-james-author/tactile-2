/*----------------------------------------------------------------------
 * Test sketch for the libraries/Tactile/TactileFileManager module 
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TactileCPU.h"
#include "TactileFileManager.h"

TactileCPU *tc;
TactileFileManager *fm;

void setup() {
  tc = TactileCPU::setup();
  tc->setLogLevel(1);
  fm = new TactileFileManager(tc);
}

void loop() {
  delay(1000);
  tc->turnLedOn();
  delay(250);
  tc->turnLedOff();
}
