/*----------------------------------------------------------------------
 * Test program, for Audio and Sensors module.  uses input
 * sensors in touch/release mode to start/stop * audio tracks.
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TactileBasics.h"
#include "TeensyUtils.h"
#include "Vibrate.h"


TeensyUtils      *tu;
Vibrate          *v;

void setup() {

  setLogLevel(2);
  
  tu = TeensyUtils::setup();
  v  = Vibrate::setup(tu);

  for (int ch = 0; ch < NUM_CHANNELS; ch++)
    v->setVibratorType(ch, linearVibrator);
  v->setVibratorType(3, motorVibrator);

  v->setVibrationEnvelope(0, "square");
  v->setVibrationEnvelope(1, "sawtooth");
  v->setVibrationEnvelope(2, "continuous");
  v->setVibrationEnvelope(3, "pulse");

  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    v->setIntensity(ch, 100);
    v->start(ch);
  }
}

int i = 0;
int cycle = 0;
int on = false;

void loop() {
  v->doTimerTasks();
  if (cycle-- <= 0) {
    Serial.print(i++); Serial.print(" "); Serial.println(on);;
    on = !on;
    if (on)
      tu->turnLedOn();
    else
      tu->turnLedOff();
    cycle = 100000;
  }
}
