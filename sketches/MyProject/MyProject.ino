#include <Arduino.h>
#include "Tactile.h"

Tactile *t;
void addCustomVibrations();

void setup() {

  setLogLevel(2);

  t = Tactile::setup();
  addCustomVibrations();
  t->setInactivityTimeout(60);

  // Channel 1
  t->setTouchReleaseThresholds(1, 85, 65);
  t->setOutputDestination(1, audioOutput, vibrationOutput);
  t->setVolume(1, 100);
  t->setVibrationEnvelope(1, "test");
  t->setVibratorType(1, linearVibrator);
  t->setVibrationFrequency(1, 150);

  // Channel 2
  t->setTouchReleaseThresholds(2, 85, 65);
  t->setOutputDestination(2, audioOutput);
  t->setVolume(2, 100);

  // Channel 3
  t->setTouchReleaseThresholds(3, 85, 65);
  t->setOutputDestination(3, audioOutput);
  t->setVolume(3, 100);
  t->useRandomTracks(3, true, true);

  // Channel 4
  t->ignoreSensor(4, true);
}

void loop() {
  t->loop();
}

VibrationEnvelope customEnvelopes[] = {
  {{"test"},43,35,1500,false,
   {100, 94, 77, 55, 35, 19, 10, 7, 6, 9, 15, 24, 32, 39, 41, 39, 32, 24, 15, 10, 8, 8, 11, 16, 24, 34, 41, 43, 41, 34, 24, 16, 10, 5, 4, 5, 9, 19, 35, 55, 77, 94, 100, -1}
  }
};

void addCustomVibrations() {
  for (int i = 0; i < 1; i++) {
    t->addCustomVibrationEnvelope(customEnvelopes[i]);
  }
}
