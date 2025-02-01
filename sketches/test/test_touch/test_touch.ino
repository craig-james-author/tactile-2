/*----------------------------------------------------------------------
 * Simple test program for TactileSensors module.
 ----------------------------------------------------------------------*/

#include "TactileCPU.h"
#include "TactileSensors.h"

int led_cycle   = 0;
TactileCPU *tc;
TactileSensors *ts;

const int ledPin = 13;

void setup() {
  tc = TactileCPU::setup();
  ts = TactileSensors::setup(tc);
}

void loop() {
  int sensorStatus[NUM_SENSORS];
  int sensorChanged[NUM_SENSORS];

  int numChanged = ts->getTouchStatus(sensorStatus, sensorChanged);

  if (numChanged > 0) {
    Serial.print(numChanged);
    Serial.print("  ");
    for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
      Serial.print(sensorNumber);
      Serial.print(":");
      Serial.print(sensorStatus[sensorNumber] ? "T" : "R");
      Serial.print(sensorChanged[sensorNumber] ? "-new " : "     ");
    }
    Serial.println();
  }
}
    
