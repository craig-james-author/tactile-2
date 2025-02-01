/*----------------------------------------------------------------------
 * Simple test program, plays up to four tracks according to whether
 * it senses proximity on the input channel, and uses the input value
 * to control volume.
 ----------------------------------------------------------------------*/

#include "TactileCPU.h"
#include "TactileSensors.h"
#include "TactileAudio.h"

int led_cycle = 0;
TactileCPU     *tc;
TactileSensors *ts;
TactileAudio   *ta;

void setup() {
  tc = TactileCPU::setup();
  ts = TactileSensors::setup(tc);
  ta = TactileAudio::setup(tc);
}

void log(const char *action, int track, float volume) {
  Serial.print(action);
  Serial.print(track);
  Serial.print(", volume ");
  Serial.println(volume);
}

void loop() {

  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    float p = ts->getProximityPercent(sensorNumber);
    int playing = ta->isPlaying(sensorNumber);
    if (p > 10.0) {
      if (!playing)
        ta->startTrack(sensorNumber);
      ta->setVolume(p);
      log("play ", sensorNumber, p);
    } else {
      if (playing) {
        ta->stopTrack(sensorNumber);
        log("stop ", sensorNumber, p);
        ta->setVolume(0);
      }
    }
  }
}
    
