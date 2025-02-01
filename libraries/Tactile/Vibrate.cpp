/* -*-C-*-
+======================================================================
| Copyright (c) 2025, Craig A. James
|
| This file is part of of the "Vibrate" library.
|
| Vibrate is free software: you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License (LGPL) as published by
| the Free Software Foundation, either version 3 of the License, or (at
| your option) any later version.
|
| Vibrate is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.
|
| You should have received a copy of the LGPL along with Vibrate. If not,
| see <https://www.gnu.org/licenses/>.
+======================================================================
*/

#include "Arduino.h"
#include "Vibrate.h"

#define DEFAULT_PWM_FREQUENCY 100000

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

  Vibrate* vo = new Vibrate(tc);
  vo->_tc->logAction2("Vibrate::setup: ", 0);

  for (int i = 0; i < NUM_CHANNELS; i++) {
    int pin1 = _convertChannelToPin1(i);
    int pin2 = _convertChannelToPin2(i);
    pinMode(pin1, OUTPUT);
    digitalWrite(pin1, LOW);
    tc->logAction2("Vibrate::setup(): Initialized pin1 to output: ", pin1);
    pinMode(pin2, OUTPUT);
    digitalWrite(pin1, LOW);
    tc->logAction2("Vibrate::setup(): Initialized pin2 to output: ", pin2);
  }

  /* for testing: a trigger square wave for the oscilloscope */
  pinMode(33, OUTPUT);

  vo->setPwmFrequency(DEFAULT_PWM_FREQUENCY);
  tc->logAction2("Vibrate::setup(): PWM frequency set: ", DEFAULT_PWM_FREQUENCY); 

  for (int c = 0; c < NUM_CHANNELS; c++) {
    vo->_vibMode[c]        = noVibrate;
    vo->_period[c]         = 0;
    vo->_startTime[c]      = 0;
    vo->_currentState[c]   = false;
    vo->_pulseWidth[c]     = 0;
    vo->_pulsePeriod[c]    = 0;
    vo->_pulseStartTime[c] = 0;
    vo->_currentVolume[c]  = 0;
  }
  return vo;
}

/*----------------------------------------------------------------------
 * This is the main method that controls the actual hardware: vibration,
 * pulse, and volume control. It should be part of the main loop() program
 * that runs Arduino systems.
 ----------------------------------------------------------------------*/

void Vibrate::doTimerTasks() {

  unsigned long timeNow = millis();

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    
    int timeInCycle = timeNow - _startTime[channel];
    int period = _period[channel];
    
    if (_vibMode[channel] == VibrateMode) {
      int pin1 = _convertChannelToPin1(channel);
      int pin2 = _convertChannelToPin2(channel);
      if (timeInCycle <= period/2) {
        if (_currentState[channel]) {
          _currentState[channel] = false;
          analogWrite(pin1, 127 + _currentVolume[channel]);
          analogWrite(pin2, 128 - _currentVolume[channel]);
          if (channel == 0)
            analogWrite(33, HIGH);        // oscilloscope trigger
        }
      } else if (timeInCycle <= period) {
        if (!_currentState[channel]) {
          _currentState[channel] = true;
          analogWrite(pin1, 128 - _currentVolume[channel]);
          analogWrite(pin2, 127 + _currentVolume[channel]);
          if (channel == 0)
            analogWrite(33, LOW);        // oscilloscope trigger
        }
      } else {
        _startTime[channel] = millis();
      }
    }

    else if (_vibMode[channel] == PulseMode) {

    }
  }
}

/*----------------------------------------------------------------------
 * Set parameters and activate actions.
 ----------------------------------------------------------------------*/

void Vibrate::setVolume(int channel, int percent) {
  if (channel < 0 || channel >= NUM_CHANNELS)
    return;
  int volume = int(0.5 + (float)percent * 128.0 / 100.0);    // convert 0-100 to 0-128
  _currentVolume[channel] = volume;

  // debug log
  if (_tc->getLogLevel() >= 2) {
    Serial.print("Vibrate::setVolume(");
    Serial.print(channel);
    Serial.print(", ");
    Serial.print(percent);
    Serial.print(") = ");
    Serial.println(volume);
  }
}

void Vibrate::setVolume(int percent) {
  for (int c = 0; c < NUM_CHANNELS; c++) {
    setVolume(c, percent);
  }
}

void Vibrate::vibrate(int channel, int frequency) {
  if (channel < 0 || channel >= NUM_CHANNELS)
    return;
  _period[channel] = _convertFrequencyToMsec(frequency);
  _startTime[channel] = millis();
  _currentState[channel] = false;
  _vibMode[channel] = VibrateMode;
}

void Vibrate::pulse(int channel, int frequency, int pulses, int period) {
  if (channel < 0 || channel >= NUM_CHANNELS)
    return;
  vibrate(channel, frequency);      // start by setting vibrate parameters
  _vibMode[channel] = PulseMode;    // but change mode to Pulse
  _pulseWidth[channel] = pulses;
  _pulsePeriod[channel] = period;

}

void Vibrate::stop(int channel) {
  if (channel < 0 || channel >= NUM_CHANNELS)
    return;
  _vibMode[channel] = noVibrate;
  int pin1 = _convertChannelToPin1(channel);
  int pin2 = _convertChannelToPin2(channel);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

bool Vibrate::isPlaying(int channel) {
  return _vibMode[channel] != noVibrate;
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

int Vibrate::_convertFrequencyToMsec(int f) {
  return (int)(0.5 + 1000.0/(float)f);
}

