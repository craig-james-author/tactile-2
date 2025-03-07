#include <Arduino.h>
#include "Tactile.h"

Tactile *t;

void setup() {

  setLogLevel(1);

  t = Tactile::setup();

  // Which channels are disabled (inactive)?
  t->ignoreSensor(1, false);
  t->ignoreSensor(2, true);
  t->ignoreSensor(3, true);
  t->ignoreSensor(4, true);

  // Input sensor 1:
  t->useProximityAsVolumeMode(1, true);
  t->setTouchReleaseThresholds(1, 15, 10);

  // Output audio, vibration, or both?
  t->setOutputDestination(1, audioOutput, vibrationOutput);

  // Options that apply to all channels:
  t->setInactivityTimeout(60);
  t->setMultiTrackMode(false);

  // Audio channel 1:
  t->setVolume(1, 100);
  t->setFadeInTime(1, 0);
  t->setFadeOutTime(1, 0);
  t->setContinueTrackMode(1, true);
  t->setPlayRandomTrackMode(1, false);
  t->setLoopMode(1, false);

  

  // General (advanced) options (uncomment to change):
  // t->setProximityMultiplier(1, 1.0);
  // t->setProximityMultiplier(2, 1.0);
  // t->setProximityMultiplier(3, 1.0);
  // t->setProximityMultiplier(4, 1.0);
  // t->setAveragingStrength(200);
}

void loop() {
  t->loop();
}
