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
  t->setOutputDestination(1, audioOutput);
  t->setVolume(1, 100);

  // Channel 2
  t->ignoreSensor(2, true);

  // Channel 3
  t->ignoreSensor(3, true);

  // Channel 4
  t->ignoreSensor(4, true);
}

void loop() {
  t->loop();
}
