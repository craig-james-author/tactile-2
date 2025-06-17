#include <Arduino.h>
#include "Tactile.h"

Tactile *t;

void addCustomVibrations();

void setup() {

  setLogLevel(1);

  t = Tactile::setup();
  t->setInactivityTimeout(60);

  // Channel 1
  t->setTouchReleaseThresholds(1, 85, 65);
  t->setOutputDestination(1, audioOutput, vibrationOutput);
  t->setVolume(1, 100);
  t->setVibrationEnvelope(1, "test1");
  t->setVibratorType(1, motorVibrator);

  // Channel 2
  t->setTouchReleaseThresholds(2, 85, 65);
  t->setOutputDestination(2, audioOutput, vibrationOutput);
  t->setVolume(2, 100);
  t->setVibrationEnvelope(2, "custom-2");
  t->setVibratorType(2, motorVibrator);

  // Channel 3
  t->setTouchReleaseThresholds(3, 85, 65);
  t->setOutputDestination(3, audioOutput);
  t->setVolume(3, 100);

  // Channel 4
  t->setTouchReleaseThresholds(4, 85, 65);
  t->setOutputDestination(4, audioOutput);
  t->setVolume(4, 100);

  // Install vibration envelopes
  addCustomVibrations();
}

void loop() {
  t->loop();
}

VibrationEnvelope customEnvelopes[] = {
  {{"test1"},20,75,1500,true,
   {0, 0, 0, 0, 0, 0, 0, 1, 6, 100, 71, 98, 71, 27, 6, 1, 0, 0, 0, 0, -1}
  },
  {{"test2"},20,75,1500,true,
   {28, 73, 100, 73, 28, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1}
  }
};

void addCustomVibrations() {
  for (int i = 0; i < 2; i++) {
    t->addCustomVibrationEnvelope(customEnvelopes[i]);
  }
}
