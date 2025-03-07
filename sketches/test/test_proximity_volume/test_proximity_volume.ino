/*----------------------------------------------------------------------
 * Simple test program, plays up to four tracks according to whether
 * it senses proximity on the input channel, and uses the input value
 * to control volume.
 ----------------------------------------------------------------------*/

#include "TeensyUtils.h"
#include "Sensors.h"
#include "AudioPlayer.h"

int led_cycle = 0;
TeensyUtils  *tu;
Sensors      *ts;
AudioPlayer  *ta;

void setup() {
  tu = TeensyUtils::setup();
  ts = Sensors::setup(tu);
  ta = AudioPlayer::setup(tu);
}

void log(const char *action, int track, float volume) {
  Serial.print(action);
  Serial.print(track);
  Serial.print(", volume ");
  Serial.println(volume);
}

void loop() {

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    float p = ts->getProximityPercent(channel);
    int playing = ta->isPlaying(channel);
    if (p > 10.0) {
      if (!playing)
        ta->startTrack(channel);
      ta->setVolume(channel, p);
      log("play ", channel, p);
    } else {
      if (playing) {
        ta->stopTrack(channel);
        log("stop ", channel, p);
        ta->setVolume(channel, 0);
      }
    }
  }
}
    
