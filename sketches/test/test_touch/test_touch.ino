/*----------------------------------------------------------------------
 * Simple test program for Sensors module.
 ----------------------------------------------------------------------*/

#include "TeensyUtils.h"
#include "Sensors.h"

int led_cycle   = 0;
TeensyUtils *tu;
Sensors *ts;

const int ledPin = 13;

void setup() {
  tu = TeensyUtils::setup();
  ts = Sensors::setup(tu);
}

void loop() {
  float proximity[NUM_CHANNELS];
  int sensorStatus[NUM_CHANNELS];
  int sensorChanged[NUM_CHANNELS];

  int numChanged = ts->getTouchStatus(proximity, sensorStatus, sensorChanged);

  if (numChanged > 0) {
    Serial.print(numChanged);
    Serial.print("  ");
    for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
      Serial.print(sensorNumber);
      Serial.print(":");
      Serial.print(sensorStatus[sensorNumber] ? "T" : "R");
      Serial.print(sensorChanged[sensorNumber] ? "-new " : "     ");
    }
    Serial.println();
  }
}
    
