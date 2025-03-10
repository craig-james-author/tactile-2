//======================================================================
//
// Modify this file as you like using the TactileAudio Configurator:
//
//     https://tactileaudio.org/tactile-configurator/
//
// It is an easy-to-use web app that guides you through configuring the
// various options of the TactileAudio software, and then creates a sketch
// like this one but tailored to your needs.
//
//======================================================================


#include <Arduino.h>
#include "Tactile.h"

Tactile *t;

void setup() {

  setLogLevel(0);

  t = Tactile::setup();
  t->setInactivityTimeout(60);
  t->setMultiTrackMode(true);

  // Channel 1
  t->setTouchReleaseThresholds(1, 85, 65);
  t->setOutputDestination(1, audioOutput);
  t->setVolume(1, 100);

  // Channel 2
  t->setTouchReleaseThresholds(2, 85, 65);
  t->setOutputDestination(2, audioOutput);
  t->setVolume(2, 100);

  // Channel 3
  t->setTouchReleaseThresholds(3, 85, 65);
  t->setOutputDestination(3, audioOutput);
  t->setVolume(3, 100);

  // Channel 4
  t->setTouchReleaseThresholds(4, 85, 65);
  t->setOutputDestination(4, audioOutput);
  t->setVolume(4, 100);
}

void loop() {
  t->loop();
}
