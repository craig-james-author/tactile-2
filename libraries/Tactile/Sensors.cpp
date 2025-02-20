/* -*-C-*-
+======================================================================
| Copyright (c) 2025, Craig A. James
|
| This file is part of of the "Tactile" library.
|
| Tactile is free software: you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License (LGPL) as published by
| the Free Software Foundation, either version 3 of the License, or (at
| your option) any later version.
|
| Tactile is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.
|
| You should have received a copy of the LGPL along with Tactile. If not,
| see <https://www.gnu.org/licenses/>.
+======================================================================
*/

#include "Arduino.h"
#include "Sensors.h"

/*----------------------------------------------------------------------
 * Initialization.
 ----------------------------------------------------------------------*/

Sensors::Sensors(TeensyUtils *tc) {
  _tu = tc;
}

Sensors* Sensors::setup(TeensyUtils *tc) {

  // Note: it might seem like this could be a static object declared at the
  // program start, but that doesn't work due to some out-of-sequence
  // operations that would occur before the hardware is ready. By creating
  // the Sensors object dynamically during the Arduino setup()
  // function, we avoid those problems.

  Sensors* t = new Sensors(tc);

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    t->_lastActionTime[channel] = 0;
    t->_lastSensorStatus[channel] = IS_RELEASED;
    t->_lastSensorPseudoStatus[channel] = IS_RELEASED;
    t->_filteredSensorValue[channel] = 0.0;
    t->_ignoreSensor[channel] = false;
    t->setProximityMultiplier(channel, 1.0);
    t->setTouchReleaseThresholds(channel, 95.0, 65.0);
  }
  t->_lastSensorTouched = -1;
  t->_touchToggleMode = false;

  t->setAveragingStrength(200);

  return t;
}


/*----------------------------------------------------------------------
 * Touch system: was a key touched or released?
 *
 * If multi-touch is not enabled, touch is exclusive; only one sensor will
 * be considered touched at a time. If two or more are touched, the
 * following rules are used:
 *
 *   1. First touched excludes others until it is released
 *   2. Simultaneous touch (two or more): the lowest-numbered sensor
 *      wins, and others are excluded intil it is released.
 *   3. When a sensor is released, if another sensor is already touched,
 *      it will be returned as a new touch the moment the first is
 *      released. If two or more other sensors are touched, the lowest-
 *      numbered one wins.
 *
 * The touchThreshold is in percent, 0.0 to 100.0%. 
 ----------------------------------------------------------------------*/

void Sensors::setTouchReleaseThresholds(float touchThreshold, float releaseThreshold) {
  for (int ch = 0; ch < NUM_CHANNELS; ch++)
    setTouchReleaseThresholds(ch, touchThreshold, releaseThreshold);
}

void Sensors::setTouchReleaseThresholds(int channel, float touchThreshold, float releaseThreshold) {

  if (touchThreshold < 0)
    _touchThreshold[channel] = 1;
  else if (touchThreshold > 100)
    _touchThreshold[channel] = 100;
  else
    _touchThreshold[channel] = touchThreshold;

  if (releaseThreshold >= _touchThreshold[channel])
    _releaseThreshold[channel] = _touchThreshold[channel] - 1;
  else if (releaseThreshold < 0)
    _releaseThreshold[channel] = 0;
  else
    _releaseThreshold[channel] = releaseThreshold;

  if (_tu->getLogLevel() > 1) {
    Serial.print(channel);
    Serial.print(": ");
  }
  _tu->logAction2("Sensors: Touch threshold: ", _touchThreshold[channel]);
  _tu->logAction2("Sensors: Release threshold: ", _releaseThreshold[channel]);
}  

void Sensors::ignoreSensor(int channel, bool ignore) {
  if (channel < 0 || channel >= NUM_CHANNELS)
    return;
  _ignoreSensor[channel] = ignore;
}

void Sensors::setTouchToggleMode(int channel, bool on) {
  _touchToggleMode = on;
  _tu->logAction2("Sensors: touchToggleMode: ", on ? 1 : 0);
}

/* Returns number of changes since the last call.
 *   - Array sensorChanges is filled with NEW_TOUCH, NEW_RELEASE, or TOUCH_NO_CHANGE.
 *   - Array sensorStatus[] is filled with true/false (1/0) indicating if the sensor it touched or not
 */

int Sensors::getTouchStatus(float proximityValues[], int sensorStatus[], int sensorChanges[]) {

  int numChanges = 0;

  for (int i = 0; i < NUM_CHANNELS; i++) {
    sensorChanges[i] = TOUCH_NO_CHANGE;
    float prox = getProximityPercent(i);
    proximityValues[i] = prox;
    if (prox >= _touchThreshold[i]) {
      sensorStatus[i] = IS_TOUCHED;
      if (_lastSensorStatus[i] != sensorStatus[i]) {
        sensorChanges[i] = NEW_TOUCH;
        numChanges++;
        _lastActionTime[i] = millis();
      }
    } else if (prox < _releaseThreshold[i]) {
      sensorStatus[i] = IS_RELEASED;
      if (_lastSensorStatus[i] != sensorStatus[i]) {
        sensorChanges[i] = NEW_RELEASE;
        numChanges++;
        _lastActionTime[i] = millis();
      }
    } else {
      sensorStatus[i] = _lastSensorStatus[i];
    }
    
    // Record for next time. Note that this is *before* the touch-toggle
    // mode below since we need to know the actual status, not the "pseudo"
    // status of the touch-toggle mode (see below).
    _lastSensorStatus[i] = sensorStatus[i];
    
    // Touch-toggle mode. This uses touch-on-touch-off rather than the standard
    // touch-on-release-off, i.e. each touch toggles the touched/released state.
    // So we convert releases to no-change, and we convert touches alternately
    // to touch/release.

    if (_touchToggleMode) {
      if (sensorChanges[i] == NEW_RELEASE) {
        sensorStatus[i] = _lastSensorPseudoStatus[i];
        sensorChanges[i] = TOUCH_NO_CHANGE;             // ignore all releases
        numChanges--;
      } else if (sensorChanges[i] == NEW_TOUCH) {
        if (_lastSensorPseudoStatus[i] == IS_TOUCHED) { // alternate touches converted to toggle on/off
          sensorChanges[i] = NEW_RELEASE;
          sensorStatus[i] = IS_RELEASED;
          _lastSensorPseudoStatus[i] = IS_RELEASED;
        } else {
          sensorChanges[i] = NEW_TOUCH;
          sensorStatus[i] = IS_TOUCHED;
          _lastSensorPseudoStatus[i] = IS_TOUCHED;
        }
      }
    }
  }

  if (numChanges > 0 && _tu->getLogLevel() > 1) {
    Serial.print("Sensors: ");
    Serial.print(numChanges);
    Serial.print(" changed, ");
    for (int i = 0; i < NUM_CHANNELS; i++) {
      if      (sensorChanges[i] == NEW_TOUCH)   Serial.print("T");
      else if (sensorChanges[i] == NEW_RELEASE) Serial.print("R");
      else if (sensorStatus[i]  == IS_TOUCHED)  Serial.print("t");
      else if (sensorStatus[i]  == IS_RELEASED) Serial.print("r");
    }
    Serial.println();
  }

  return numChanges;
}

/*----------------------------------------------------------------------
 * Proximity sensor.
 ----------------------------------------------------------------------*/

void Sensors::setAveragingStrength(int samples) {
  if (samples < 0)
    samples = 0;
  _averagingSamples = samples;
  _tu->logAction2("Sensors: averaging: ", _averagingSamples);
}

void Sensors::setProximityMultiplier(int channel, float m) {
  channel = _checkSensorRange(channel);
  _proximityMultiplier[channel] = m;
}

float Sensors::getProximityPercent(int channel) {
  if (_ignoreSensor[channel])
    return 0.0;
  channel = _checkSensorRange(channel);
  int p = analogRead(_channelToPinNumber[channel]);
  if (_averagingSamples > 0) {
    // Simple infinite-impulse-response filter, e.g. if averaging "strength" is 10 and "p"
    // is the new value, then new average "pavg" value is (9*(pavg) + p)/10.
    _filteredSensorValue[channel] =
      (_filteredSensorValue[channel] * (_averagingSamples - 1) + (float)p) / (float)_averagingSamples;
    p = (int)_filteredSensorValue[channel];
  }
  p = p * 100.0 * _proximityMultiplier[channel] / 1024;  // convert digital signal to percent
  if (p > 100.0)
    p = 100.0;
  return p;
}

int Sensors::_checkSensorRange(int channel) {
  if (channel < 0)
    return 0;
  if (channel >= NUM_CHANNELS)
    return NUM_CHANNELS - 1;
  return channel;
}
