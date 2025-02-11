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
#include "Vibrate.h"
#include "VibrationEnvelopes.h"

#define DEFAULT_PWM_FREQUENCY 100000
#define DEFAULT_VIBRATOR_FREQUENCY 180

/*----------------------------------------------------------------------
 * Constructor
 ----------------------------------------------------------------------*/

Vibrate::Vibrate(TeensyUtils *tc) {
  _tc = tc;
}

/*----------------------------------------------------------------------
 * Initialization.
 ----------------------------------------------------------------------*/

Vibrate* Vibrate::setup(TeensyUtils *tc) {
  // Note: it might seem like this could be a static object declared at the
  // program start, but that doesn't work due to some out-of-sequence
  // operations that would occur before the hardware is ready. By creating
  // the Vibrate object dynamically during the Arduino setup()
  // function, we avoid those problems.

  Vibrate* v = new Vibrate(tc);
  tc->log2("Vibrate::setup()");

  for (int i = 0; i < NUM_CHANNELS; i++) {
    int pin1 = v->_convertChannelToPin1(i);
    int pin2 = v->_convertChannelToPin2(i);
    pinMode(pin1, OUTPUT);
    digitalWrite(pin1, LOW);
    tc->logAction2("Vibrate::setup(): Initialized pin1 to output: ", pin1);
    pinMode(pin2, OUTPUT);
    digitalWrite(pin1, LOW);
    tc->logAction2("Vibrate::setup(): Initialized pin2 to output: ", pin2);
  }

  /* For testing: a trigger square wave for the oscilloscope. The actual output
   * pins are hard to trigger on since they're using 100KHz pulse-width modulation */
  pinMode(33, OUTPUT);

  v->setPwmFrequency(DEFAULT_PWM_FREQUENCY);
  tc->logAction2("Vibrate::setup(): PWM frequency set: ", DEFAULT_PWM_FREQUENCY); 

  // Assign simple vibration to all channels
  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    v->setVibrationEnvelope(ch, "square");
    v->setVibrationFrequency(ch, DEFAULT_VIBRATOR_FREQUENCY);
  }

  return v;
}

/*----------------------------------------------------------------------
 * This is the main method that controls the actual hardware. It
 * creates the vibrator output and applies the volume envelope to it.
 *----------------------------------------------------------------------*/

void Vibrate::doTimerTasks() {

  unsigned long timeNow = millis();

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    
    int timeInCycle = timeNow - _startTimeForVibration[channel];
    int period = _vibrationPeriod[channel];
    
    if (_isPlaying[channel]) {
      if (timeInCycle <= period/2) {            // 1st half?
        if (_currentState[channel]) {           // just started 1st half? Turn the juice on
          _currentState[channel] = false;
          int pin1 = _convertChannelToPin1(channel);
          int pin2 = _convertChannelToPin2(channel);
          analogWrite(pin1, 127 + _actualVolume[channel]);
          analogWrite(pin2, 128 - _actualVolume[channel]);
          if (channel == 0)
            analogWrite(33, HIGH);        // oscilloscope trigger
        }
      } else if (timeInCycle <= period) {       // 2nd half?
        if (!_currentState[channel]) {          // just started 2nd half? Turn the juice off
          _currentState[channel] = true;
          int pin1 = _convertChannelToPin1(channel);
          int pin2 = _convertChannelToPin2(channel);
          analogWrite(pin1, 128 - _actualVolume[channel]);
          analogWrite(pin2, 127 + _actualVolume[channel]);
          if (channel == 0)
            analogWrite(33, LOW);        // oscilloscope trigger
        }
      } else {        // Finished 2nd half of cycle.
        _startTimeForVibration[channel] = timeNow; // restart cycle timer
      }

      // Now see if it's time to go to the next point in the volume envelope
      int timeInPoint = timeNow - _startTimeForPoint[channel];
      if (timeInPoint > _vibrationEnvelope[channel].msecPerPoint) {
        _indexInEnvelope[channel] += 1;         // next point in volume envelope
        if (_indexInEnvelope[channel] >= _vibrationEnvelope[channel].numberOfPoints) {
          _indexInEnvelope[channel] = 0;
        }
        _actualVolume[channel] = _calculateActualVolume(channel,_vibrationEnvelope[channel].volumes[_indexInEnvelope[channel]]);
        _startTimeForPoint[channel] = timeNow;
      }
    }
  }
}

/*----------------------------------------------------------------------
 * Set parameters and activate actions. Note that these start with
 * the external channel numbers 1..N, not 0..N-1.
 ----------------------------------------------------------------------*/

void Vibrate::start(int channel) {
  channel -= 1;
  channel = _checkChannel(channel);
  if (_vibrationEnvelope[channel].numberOfPoints == -1) {
    _isPlaying[channel] = false;
    _tc->logAction("Error, can't start, no vibration envelope assigned, channel ", channel);
    return;
  }
  _isPlaying[channel] = true;
  _actualVolume[channel] = _calculateActualVolume(channel, _vibrationEnvelope[channel].volumes[0]);

  // setup for vibration timer
  unsigned long timenow = millis();
  _startTimeForVibration[channel] = timenow;
  _currentState[channel] = false;

  // set up for envelope timer
  _startTimeForPoint[channel] = timenow;
  _indexInEnvelope[channel] = 0;
  _tc->logAction2("Vibrate::start: ", channel);
}

void Vibrate::stop(int channel) {
  channel -= 1;
  channel = _checkChannel(channel);
  _isPlaying[channel] = false;
  int pin1 = _convertChannelToPin1(channel);
  int pin2 = _convertChannelToPin2(channel);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  _tc->logAction2("Vibrate::stop: ", channel);
}

bool Vibrate::isPlaying(int channel) {
  channel -= 1;
  channel = _checkChannel(channel);
  return _isPlaying[channel];
}

void Vibrate::setVibrationEnvelope(int channel, const char *name) {
  channel -= 1;
  channel = _checkChannel(channel);
  for (int i = 0; ; i++) {
    if (builtInEnvelopes[i].name[0] == 0) {     // end of list
      _vibrationEnvelope[channel].name[0] = '\0';
      _vibrationEnvelope[channel].numberOfPoints = 0;
      _vibrationEnvelope[channel].msecPerPoint = 0;
      _vibrationEnvelope[channel].msecTotal = 0;
      _vibrationEnvelope[channel].volumes[0] = -1;
      if (_tc->getLogLevel() > 0) {
        Serial.print("setVibrationEnvelope: name not found: "); Serial.println(name);
      }
      break;
    } else if (0 == strcmp(builtInEnvelopes[i].name, name)) {
      _vibrationEnvelope[channel] = builtInEnvelopes[i];
      _calculateLengthOfEnvelope(channel);   // in case definition is wrong or not set
      _calculateMsecPerEnvelopePoint(channel);
      break;
    }
  }
  if (_tc->getLogLevel() > 2) {
    Serial.print("setVibrationEnvelope(): channel "); Serial.println(channel);
    Serial.print("     name "); Serial.println( _vibrationEnvelope[channel].name);
    Serial.print("  nPoints "); Serial.println(_vibrationEnvelope[channel].numberOfPoints);
    Serial.print("  msecPer "); Serial.println(_vibrationEnvelope[channel].msecPerPoint);
    Serial.print("  msecTot "); Serial.println(_vibrationEnvelope[channel].msecTotal);
  }
}

int Vibrate::_readln(File f, char *buf, int bufLen) {
  int len = 0;
  while (len < bufLen-1) {
    if (f.available() <= 0) {
      break;
    }
    char c = f.read();
    if (c == '\n') {
      break;
    } else {
      buf[len++] = c;
    }
  }
  buf[len] = '\0';    // line too long for buffer
  return len;
}

void Vibrate::setVibrationEnvelopeFile(int channel, const char *name) {

  char buffer[101];

  channel -= 1;
  channel = _checkChannel(channel);
  
  File myFile = SD.open(name, FILE_READ);
  if (!myFile) {
    Serial.print("setVibrationEnvelopeFile: can't open file: ");
    Serial.println(name);
    return;
  }
  if (_tc->getLogLevel() > 1) {
    Serial.print("setVibrationEnvelopeFile: opened file: ");
    Serial.println(name);
  }
  // read parameters up to the "volumes:" line
  int   frequency = 0;
  int   numberOfPoints = 0;  
  float soundLength = 0;
  while (myFile.available()) {
    _readln(myFile, buffer, 100);
    char *nameEnd, *value;
    nameEnd = strchr(buffer, ':');
    if (!nameEnd) {
      Serial.print("setVibrationEnvelopeFile: Invalid line in file: ");
      Serial.println(name);
      myFile.close();
      return;
    }
    *nameEnd = '\0';
    value = nameEnd + 1;
    if        (0 == strcmp(buffer, "soundLength")) {
      soundLength = atof(value);
    } else if (0 == strcmp(buffer, "frequency")) {
      frequency = atoi(value);
    } else if (0 == strcmp(buffer, "numPoints")) {
      numberOfPoints = atoi(value);
    } else if (0 == strcmp(buffer, "volumes")) {
      break;
    }
  }

  VibrationEnvelope *ve = &_vibrationEnvelope[channel];

  int n = 0;
  while (myFile.available()) {
    int len = _readln(myFile, buffer, 100);
    if (len == 0)
      break;
    int volume = atoi(buffer);
    ve->volumes[n++] = volume;
  }
  myFile.close();

  if (n != numberOfPoints) {
    Serial.print("Error: numPoints doesn't match actual number of volume values: ");
    Serial.print(numberOfPoints); Serial.print(":"); Serial.println(n);
    numberOfPoints = n;
  }
  if (numberOfPoints <= 0) {    // invalid values, set simple default
    numberOfPoints = 1;
    ve->volumes[0] = 100;
  }

  ve->msecTotal = (int)(1000.0 * soundLength);
  ve->numberOfPoints = numberOfPoints;
  ve->msecPerPoint = (int)(0.5 + ve->msecTotal/numberOfPoints);

  setVibrationFrequency(channel+1, frequency);
}

void Vibrate::overrideVibrationEnvelopeDuration(int channel, int msec) {
  channel -= 1;
  channel = _checkChannel(channel);
  if (_vibrationEnvelope[channel].numberOfPoints == 0)
    return;
  if (msec < 1) msec = 1;
  else if (msec > 100000) msec = 100000;
  _vibrationEnvelope[channel].msecTotal = msec;
  _calculateMsecPerEnvelopePoint(channel);
}

void Vibrate::overrideVibrationEnvelopeRepeats(int channel, bool repeats) {
  channel -= 1;
  channel = _checkChannel(channel);
 _vibrationEnvelope[channel].repeats = repeats;
}

void Vibrate::setVibrationFrequency(int channel, int frequency) {
  channel -= 1;
  channel = _checkChannel(channel);
  if (frequency < 20)
    frequency = 20;
  else if (frequency > 400)
    frequency = 400;
  _vibrationFrequency[channel] = frequency;
  _vibrationPeriod[channel] = (int)(0.5 + 1000.0/frequency);     // convert frequency to msec
}

void Vibrate::setVolume(int channel, int percent) {
  channel -= 1;
  channel = _checkChannel(channel);
  if      (percent > 100) percent = 100;
  else if (percent < 0)   percent = 0;
  _setVolume[channel] = percent;
}

void Vibrate::setVolume(int percent) {
  for (int c = 1; c <= NUM_CHANNELS; c++) {
    setVolume(c, percent);
  }
}

void Vibrate::setPwmFrequency(int frequency) {
  if (frequency < 100)
    _pwmFrequency = 100;
  else if (frequency > 300000)
    _pwmFrequency = 300000;
  else
    _pwmFrequency = frequency;
  for (int c = 0; c < NUM_CHANNELS; c++) {
    int pin1 = _convertChannelToPin1(c);
    int pin2 = _convertChannelToPin2(c);
    analogWriteFrequency(pin1, _pwmFrequency);
    analogWriteFrequency(pin2, _pwmFrequency);
  }
}
  
/*----------------------------------------------------------------------
 * Internal methods.
 ----------------------------------------------------------------------*/

int Vibrate::_checkChannel(int channel) {
  if (channel < 0) return 0;
  if (channel >= NUM_CHANNELS) return NUM_CHANNELS - 1;
  return channel;
}

int Vibrate::_convertChannelToPins(int channel) {
  if (channel < 0)
    return _channelNumberToPinNumbers[0];
  if (channel >= NUM_CHANNELS)
    return _channelNumberToPinNumbers[NUM_CHANNELS - 1];
  return _channelNumberToPinNumbers[channel];
}

int Vibrate::_convertChannelToPin1(int channel) {
  int pin = _convertChannelToPins(channel);
  return pin & 0xFF;
}

int Vibrate::_convertChannelToPin2(int channel) {
  int pin = _convertChannelToPins(channel);
  return (pin & 0xFF00)>>8;
}

void Vibrate::_calculateLengthOfEnvelope(int channel) {
  int length = 0;
  int *v = _vibrationEnvelope[channel].volumes;
  for (length = 0; ; length++) {
    if (v[length] < 0) {
      _vibrationEnvelope[channel].numberOfPoints = length;
      break;
    }
  }
}

void Vibrate::_calculateMsecPerEnvelopePoint(int channel) {
  VibrationEnvelope *ve = &_vibrationEnvelope[channel];
  ve->msecPerPoint = (int) (0.5 + (float)ve->msecTotal / (float)ve->numberOfPoints);
}

int Vibrate::_calculateActualVolume(int channel, int volume) {
  // setVolume() and requested volume both specify 0-100%.
  // Requested "volume" is adjusted by overall volume setting, i.e. if an
  // envelope's point is at 50 and the set volume is at 50, then the output
  // volume will be 25 (50*50/100). This is multiplied by 128/100 to convert
  // to integer in range 0..128.
  return int(0.5 + (float)(volume * _setVolume[channel]) * 128.0 / 10000.0);
}
