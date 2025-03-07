/*----------------------------------------------------------------------
 * Test program, for AudioPlayer and Sensors module.  uses input
 * sensors in touch/release mode to start/stop * audio tracks.
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TeensyUtils.h"
#include "Sensors.h"
#include "AudioPlayer.h"

TeensyUtils *tu;
Sensors     *ts;
AudioPlayer *ta;

void setup() {
  tu = TeensyUtils::setup();
  ts = Sensors::setup(tu);
  ta = AudioPlayer::setup(tu);
}

void loop() {
  float proximityValues[NUM_CHANNELS];
  int sensorStatus[NUM_CHANNELS];
  int sensorChanged[NUM_CHANNELS];

  int numChanged = ts->getTouchStatus(proximityValues, sensorStatus, sensorChanged);

  int numTouched = 0;
   for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
    if (sensorStatus[sensorNumber] == IS_TOUCHED)
      numTouched++;
  }

  if (numChanged > 0) {

    Serial.print(numTouched);
    Serial.print("  ");
    for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
      Serial.print(sensorNumber);
      Serial.print(":");
      Serial.print(sensorStatus[sensorNumber] ? "T " : "R ");
    }
    Serial.println();

    for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
      if (sensorChanged[sensorNumber] == NEW_TOUCH) {
        if (ta->isPaused(sensorNumber)) {
          tu->logAction("track is paused: ", sensorNumber);
          ta->resumeTrack(sensorNumber);
          tu->logAction("track resumed: ", sensorNumber);
        }
        else {
          ta->startTrack(sensorNumber);
          tu->logAction("started track: ", sensorNumber);
        }
      }
      else if (sensorChanged[sensorNumber] == NEW_RELEASE) {
        ta->pauseTrack(sensorNumber);
        tu->logAction("paused track: ", sensorNumber);
      }
    }
  }
}
    
