/*----------------------------------------------------------------------
 * Test program, for TactileAudio and TactileSensors module.  uses input
 * sensors in touch/release mode to start/stop * audio tracks.
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TactileCPU.h"
#include "TactileSensors.h"
#include "TactileAudio.h"

TactileCPU     *tc;
TactileSensors *ts;
TactileAudio   *ta;

void setup() {
  tc = TactileCPU::setup();
  ts = TactileSensors::setup(tc);
  ta = TactileAudio::setup(tc);
}

void loop() {
  int sensorStatus[NUM_SENSORS];
  int sensorChanged[NUM_SENSORS];

  int numChanged = ts->getTouchStatus(sensorStatus, sensorChanged);

  int numTouched = 0;
  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    if (sensorStatus[sensorNumber] == IS_TOUCHED)
      numTouched++;
  }

  if (numChanged > 0) {

    Serial.print(numTouched);
    Serial.print("  ");
    for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
      Serial.print(sensorNumber);
      Serial.print(":");
      Serial.print(sensorStatus[sensorNumber] ? "T " : "R ");
    }
    Serial.println();

    for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
      if (sensorChanged[sensorNumber] == NEW_TOUCH) {
        if (ta->isPaused(sensorNumber)) {
          tc->log_action("track is paused: ", sensorNumber);
          ta->resumeTrack(sensorNumber);
          tc->log_action("track resumed: ", sensorNumber);
        }
        else {
          ta->startTrack(sensorNumber);
          tc->log_action("started track: ", sensorNumber);
        }
      }
      else if (sensorChanged[sensorNumber] == NEW_RELEASE) {
        ta->pauseTrack(sensorNumber);
        tc->log_action("paused track: ", sensorNumber);
      }
    }
  }
}
    
