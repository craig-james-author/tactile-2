/* -*-C-*-
+======================================================================
| Copyright (c) 2022, Craig A. James
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

/*======================================================================
|
| This class is essentially a full Arduino sketch wrapped up in a
| C++ class, including the setup() and loop() methods. The only thing
| needed to program a fully functioning app is to specify the options
| (behavior) desired, and forward the Arduino setup() and loop()
| functions to this class.
|
| NOTE: For this "wrapper" class, all sensor numbers go from 1 to 4 rather
|       than the internal numbering of 0 to 3. That is, incoming API calls
|       subtract 1 from the track number, and outgoing results (e.g.
|       logging to the serial monitor) add 1 to the track number. This
|       it to make it more "natural" for non-programmers, and to match
|       the numbering of the E1..E4 subdirectories used for random tracks.
|
+======================================================================*/

#include <Arduino.h>
#include "Tactile.h"

void Tactile::setLogLevel(int level) {
  _tu->setLogLevel(level);
}

static int channelExtern2Intern(int channel) {
  channel -= 1;
  if (channel < 0) return 0;
  if (channel >= NUM_CHANNELS) return NUM_CHANNELS - 1;
  return channel;
}

/*---------- Audio Sound controls ----------*/

void Tactile::setOutputDestination(int channel, OutputDest dest1, OutputDest dest2) {
  channel = channelExtern2Intern(channel);
  _useAudioOutput[channel] = false;
  _useVibrationOutput[channel] = false;
  if (dest1 == audioOutput || dest2 == audioOutput) {
    _useAudioOutput[channel] = true;
  }
  if (dest1 == vibrationOutput || dest2 == vibrationOutput) {
    _useVibrationOutput[channel] = true;
  }
}

void Tactile::setInputSource(int channel, InputSource source) {
  channel = channelExtern2Intern(channel);
  _useTouchInput[channel] = false;
  _useAudioInput[channel] = false;
  if (source == touchInput) {
    _useTouchInput[channel] = true;
  } else if (source == audioInput) {
    _useAudioInput[channel] = true;
  }
}

const char *Tactile::getTrackName(int channel) {
  channel = channelExtern2Intern(channel);
  return _ta->getTrackName(channel);
}

void Tactile::setVolume(int channel, int percent) {
  channel = channelExtern2Intern(channel);
  _ta->setVolume(channel, percent);
}

void Tactile::setVolume(int percent) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setVolume(ch, percent);
}

void Tactile::useRandomTracks(int channel, boolean on) {
  channel = channelExtern2Intern(channel);
  _ta->useRandomTracks(channel, on);
}

void Tactile::useRandomTracks(boolean on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    useRandomTracks(ch, on);
}

void Tactile::useProximityAsVolume(int channel, boolean on) {
  channel = channelExtern2Intern(channel);
  _useProximityAsVolume[channel] = on;
  if (on) {
    _ta->setFadeInTime(channel, 0);        // Fade in/out isn't compatible with proximity-as-volume
    _ta->setFadeOutTime(channel, 0);
  }
}

void Tactile::useProximityAsVolume(boolean on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    useProximityAsVolume(ch, on);
}

void Tactile::setFadeInTime(int channel, int milliseconds) {
  channel = channelExtern2Intern(channel);
  if (milliseconds > 0 && _useProximityAsVolume) {
    Serial.println("WARNING: proximity-as-volume mode is incompatible with fade-in/fade-out. "
                   "Fade-in time ignored.");
    return;
  }
  if (milliseconds < 0)
    milliseconds = 0;
  _ta->setFadeInTime(channel, milliseconds);
}

void Tactile::setFadeInTime(int milliseconds) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setFadeInTime(ch, milliseconds);
}

void Tactile::setFadeOutTime(int channel, int milliseconds) {
  channel = channelExtern2Intern(channel);
  if (milliseconds > 0 && _useProximityAsVolume) {
    Serial.println("WARNING: proximity-as-volume mode is incompatible with fade-in/fade-out. "
                   "Fade-out time ignored.");
    return;
  }
  if (milliseconds < 0)
    milliseconds = 0;
  _ta->setFadeOutTime(channel, milliseconds);
}

void Tactile::setFadeOutTime(int milliseconds) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setFadeOutTime(ch, milliseconds);
}

/*---------- Input sensor controls ----------*/

void Tactile::ignoreSensor(int channel, bool ignore) {
  channel = channelExtern2Intern(channel);
  _ts->ignoreSensor(channel, ignore);
}

void Tactile::setMultiTrackMode(boolean on) {
  _multiTrack = on;
}

void Tactile::setTouchReleaseThresholds(int channel, int touch, int release) {
  channel = channelExtern2Intern(channel);
  if (touch > 100)
    touch = 100;
  else if (touch < 1)
    touch = 1;
  if (release >= touch)
    release = touch - 1;
  else if (release < 0)
    release = 0;
  _touchThreshold[channel] = touch;
  _releaseThreshold[channel] = release;
  _ts->setTouchReleaseThresholds(channel, (float)touch, (float)release);
}

void Tactile::setTouchReleaseThresholds(int touch, int release) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setTouchReleaseThresholds(ch, touch, release);
}

void Tactile::setTouchToStop(int channel, boolean on) {
  channel = channelExtern2Intern(channel);
  _touchToStop[channel] = on;
  _ts->setTouchToggleMode(channel, on);
}

void Tactile::setTouchToStop(boolean on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setTouchToStop(ch, on);
}

void Tactile::setContinueTrackMode(int channel, boolean on) {
  channel = channelExtern2Intern(channel);
  _continueTrack[channel] = on;
}

void Tactile::setContinueTrackMode(boolean on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setContinueTrackMode(ch, on);
}

void Tactile::setLoopMode(int channel, boolean on) {
  channel = channelExtern2Intern(channel);
  _ta->setLoopMode(channel, on);
}

void Tactile::setLoopMode(boolean on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setLoopMode(ch, on);
}

void Tactile::setInactivityTimeout(int seconds) {
  if (seconds < 0)
    seconds = 0;
  _restartTimeout = (uint32_t)(seconds * 1000);
}

void Tactile::setProximityMultiplier(int channel, float m) {
  channel = channelExtern2Intern(channel);
  _ts->setProximityMultiplier(channel, m);
}

void Tactile::setAveragingStrength(int samples) {
  _ts->setAveragingStrength(samples);
}

/*-------------------- vibration controls --------------------*/

void Tactile::setVibratorType(int channel, VibratorType vibType) {
  channel = channelExtern2Intern(channel);
  _v->setVibratorType(channel, vibType);
}
void Tactile::setVibrationFrequency(int channel, int frequency) {
  channel = channelExtern2Intern(channel);
  _v->setVibrationFrequency(channel, frequency);
}

void Tactile::setVibrationIntensity(int channel, int intensityPercent) {
  channel = channelExtern2Intern(channel);
  _v->setIntensity(intensityPercent);
}
void Tactile::setVibrationEnvelope(int channel, const char *name) {
  channel = channelExtern2Intern(channel);
  _v->setVibrationEnvelope(channel, name);
}
void Tactile::setVibrationEnvelopeFile(int channel, const char *fileName) {
  channel = channelExtern2Intern(channel);
  _v->setVibrationEnvelopeFile(channel, fileName);
}
void Tactile::overrideVibrationEnvelopeDuration(int channel, int msec) {
  channel = channelExtern2Intern(channel);
  _v->overrideVibrationEnvelopeDuration(channel, msec);
}
void Tactile::overrideVibrationEnvelopeRepeats(int channel, bool repeat) {
  channel = channelExtern2Intern(channel);
  _v->overrideVibrationEnvelopeRepeats(channel, repeat);
}
void Tactile::useProximityAsIntensity(int channel, bool on) {
  channel = channelExtern2Intern(channel);
  _proximityControlsIntensity[channel] = on;
  if (on) {
    _proximityControlsSpeed[channel] = false;
    _speedMultiplierPercent[channel] = 100;
  }
}

// Vibration speed factor to speedup. sf is the amount to speed up, e.g. 50
// means that at zero proximity (full contact), speed is 150% of base value.
//
// sf%     speedup
//   0 --> 1.0 - 1.0
//  50 --> 1.0 - 1.5
// 100 --> 1.0 - 2.0
// 500 --> 1.0 - 6.0
// -50 --> 1.0 - 0.5 (gets slower)  
//-100 --> 1.0 - 0.0 (stops)
//
// So speed factor is: 1 + (speedMultiplierPct/100 * proxPct)

void Tactile::useProximityAsSpeed(int channel, bool on, int multiplierPercent) {
  channel = channelExtern2Intern(channel);
  if (multiplierPercent > 1000)
    multiplierPercent = 100;
  else if (multiplierPercent < -100)
    multiplierPercent = -100;
  if (on) {
    _proximityControlsIntensity[channel] = false;
    _proximityControlsSpeed[channel] = true;
    _speedMultiplierPercent[channel] = multiplierPercent;
  } else {
    _speedMultiplierPercent[channel] = 100;
    _proximityControlsSpeed[channel] = false;
  }
}

/*-------------------- main functions --------------------*/

Tactile* Tactile::setup() {

  Tactile *t = new(Tactile);

  for (int c = 1; c <= NUM_CHANNELS; c++) {
    t->setInputSource(c, touchInput);
    t->setOutputDestination(c, audioOutput, vibrationOutput);
  }

  t->_tu = TeensyUtils::setup();
  t->_tu->setLogLevel(1);

  t->_ts = Sensors::setup(t->_tu);
  t->_ta = AudioPlayer::setup(t->_tu);
  t->_v  = Vibrate::setup(t->_tu);
  
  // Audio initialization
  for (int c = 1; c <= NUM_CHANNELS; c++) {
    t->setTouchReleaseThresholds(c, 95, 65);
    t->setContinueTrackMode(c, false);
    t->useRandomTracks(c, false);
    t->useProximityAsVolume(c, false);
  }
  t->setInactivityTimeout(0);
  t->setMultiTrackMode(false);

  // Vibration initialization
  for (int c = 1; c <= NUM_CHANNELS; c++) {
    t->useProximityAsSpeed(c, false, 100);
    t->useProximityAsIntensity(c, false);
  }

  // Bookkeeping
  t->_ledCycle = 0;
  t->_trackCurrentlyPlaying = -1;
  t->_lastActionTime = millis();
  
  return t;
}

/*----------------------------------------------------------------------
 * TOUCH-MODE LOOP
 ----------------------------------------------------------------------*/
    
void Tactile::_touchLoop() {
  float proximityValues[NUM_CHANNELS];
  int sensorStatus[NUM_CHANNELS];
  int sensorChanged[NUM_CHANNELS];

  int numChanged = _ts->getTouchStatus(proximityValues, sensorStatus, sensorChanged);

  int numTouched = 0;
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    if (sensorStatus[channel] == IS_TOUCHED)
      numTouched++;
  }
  if (numTouched > 0 || numChanged > 0)
    _lastActionTime = millis();

  if (numTouched > 0)
    _tu->turnLedOn();
  else
    _tu->turnLedOff();

  if (numChanged == 0)
    return;

  // MULTI-TRACK MODE. Simple: if a sensor is touched, start playing the
  // track; if it's released, stop playing. Multiple tracks can go at
  // the same time.

  if (_multiTrack) {
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {

      int isPlaying = false;
      if (_useAudioOutput[channel]) {
        isPlaying = _ta->isPlaying(channel);
      }

      // Start or resume audio
      if (sensorChanged[channel] == NEW_TOUCH) {
        if (_useAudioOutput[channel]) {
          if (!isPlaying) {
            if (!_continueTrack[channel])
              _ta->cancelFades(channel);
            _ta->startTrack(channel);
            _tu->logAction("start track ", channel+1);
          } else {
            if (_ta->isPaused(channel)) {
              _ta->resumeTrack(channel);
              _tu->logAction("resume track ", channel+1);
            } else {
              _ta->startTrack(channel);
              _tu->logAction("restart track (was paused?) ", channel+1);
            }
          }
          if (_useProximityAsVolume[channel])
            _ta->setVolume(channel, proximityValues[channel]);
        }

        // Start vibration
        if (_useVibrationOutput[channel]) {
          _v->start(channel);
          _tu->logAction("start vibrator ", channel+1);
        }
      }
      else if (sensorChanged[channel] == NEW_RELEASE) {

        if (_useAudioOutput[channel]) {
          if (isPlaying) {
            if (_continueTrack[channel]) {
              _ta->pauseTrack(channel);
              _tu->logAction("pause track ", channel+1);
            } else {
              _ta->stopTrack(channel);
              _tu->logAction("stop track ", channel+1);
            }
            _ta->setVolume(channel, 0);
          }
          if (_useVibrationOutput[channel]) {
            _v->stop(channel);
            _tu->logAction("stop vibrator ", channel+1);
          }
        }
      }
    }
  }

  // SINGLE-TRACK MODE. This is, surprisingly, a bit more complicated.
  // 
  // When a sensor is touched, play that track if nothing is already
  // playing, and as long as the sensor is still touched, keep playing.
  //
  // When a sensor is released, it's more complicated.
  //   - If no other sensor is being touched, just stop the track playing.
  //   - If one or more other sensors are being touched as this one
  //     is released, select the lowest, and consider it a "new touch",
  //     that is, start that track.
  
  else {

    // If the sensor for the track currently playing is still touched, keep playing (i.e. do nothing).
    if (_trackCurrentlyPlaying >= 0
        && sensorStatus[_trackCurrentlyPlaying] == IS_TOUCHED) {
      return;
    }      

    // If there's a release event, stop or pause that track.
    if (_trackCurrentlyPlaying >= 0
        && sensorChanged[_trackCurrentlyPlaying] == NEW_RELEASE) {
      if (_useAudioOutput[_trackCurrentlyPlaying]) {
        if (_continueTrack) {
          _ta->pauseTrack(_trackCurrentlyPlaying);
          _tu->logAction("pause track ", _trackCurrentlyPlaying+1);
        } else {
          _ta->stopTrack(_trackCurrentlyPlaying);
          _tu->logAction("stop track ", _trackCurrentlyPlaying+1);
        }
      }
      if (_useVibrationOutput[_trackCurrentlyPlaying]) {
        _v->stop(_trackCurrentlyPlaying);
        _tu->logAction("stop vibrator ", _trackCurrentlyPlaying+1);
      }
      _trackCurrentlyPlaying = -1;
    }

    // Is one or more other sensor being touched? The track for the lowest-numbered
    // touched sensor is played (whether it's a new touch or an ongoing touch
    // that started before the last release).

    for (int channel = 0; ; channel++) {
      if (channel >= NUM_CHANNELS) {
        _trackCurrentlyPlaying = -1;    // no other sensors are being touched, nothing is playing
        break;
      }
      if (sensorStatus[channel] == IS_TOUCHED) {
        if (_useAudioOutput[channel]) {
          if (_ta->isPaused(channel)) {
            _ta->resumeTrack(channel);
            _tu->logAction("Tactile: resume track ", channel+1);
          } else {
            if (!_continueTrack)
              _ta->cancelFades(channel);
            _ta->startTrack(channel);
            _tu->logAction("Tactile: start track ", channel+1);
          }
          _trackCurrentlyPlaying = channel;
        }
        if (_useVibrationOutput[channel]) {
          _v->start(channel);
          _tu->logAction("Tactile: start vibrator ", channel+1);
        }
        break;
      }
    }
  }
}
    
/*----------------------------------------------------------------------
* PROXIMITY LOOP: When touch proximity controls the volume.
----------------------------------------------------------------------*/

void Tactile::_proximityLoop() {
  float sensorValues[NUM_CHANNELS];
  float maxSensorValue = 0.0;
  int   maxSensorNumber = -1;

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    sensorValues[channel] = _ts->getProximityPercent(channel);
    if (sensorValues[channel] > maxSensorValue) {
      maxSensorValue = sensorValues[channel];
      maxSensorNumber = channel;
    }
  }

  // Set the LED brightness proportional to whichever sensor has the highest value
  int led_percent = int(maxSensorValue);
  if (_ledCycle < led_percent)
    _tu->turnLedOn();
  else
    _tu->turnLedOff();
  _ledCycle++;

  // Log sensor values (once every 100 loops)
  if (_ledCycle > 99) {
    _ledCycle = 0;
  }

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    if (_multiTrack || channel == maxSensorNumber) {

      float sensorValue = sensorValues[channel];

      /* Audio: start/stop and control volume */
      if (_useAudioOutput[channel]) {
        int playing = _ta->isPlaying(channel) && !_ta->isPaused(channel);
        if (sensorValue > _touchThreshold[channel]) {
          if (!playing) {
            if (_ta->isPaused(channel)) {
              _ta->resumeTrack(channel);
              _tu->logAction("resume ", channel+1);
            } else {
              _ta->startTrack(channel);
              _tu->logAction("play ", channel+1);
            }
          }
          _ta->setVolume(channel, sensorValue);
          _lastActionTime = millis();
        } else if (sensorValue < _releaseThreshold[channel]) {
          if (playing) {
            if (_continueTrack) {
              _ta->pauseTrack(channel);
              _tu->logAction("pause ", channel+1);
            } else {
              _ta->stopTrack(channel);
              _tu->logAction("stop ", channel+1);
            }
            _ta->setVolume(channel, 0);
            _lastActionTime = millis();
          }
        }
      }

      // Vibration: start/stop and control intensity OR control speed.
      if (_useVibrationOutput[channel]) {
        if (sensorValue > _touchThreshold[channel]) {
          if (_proximityControlsSpeed[channel]) {
            int multiplier = (int)(0.499 + (float)_speedMultiplierPercent[channel]/100.0 * (float)sensorValue);
            _v->setSpeedMultiplier(channel, multiplier);
          } else if (_proximityControlsIntensity[channel]) {
            _v->setIntensity(channel, sensorValue);
          }
          if (!_v->isPlaying(channel))
            _v->start(channel);
        }
        if (sensorValue < _releaseThreshold[channel]) {
          if (_v->isPlaying(channel)) {
            _v->stop(channel);
            _tu->logAction("stop vibrator ", channel+1);
          }
        }
      }

    }
  }
}

void Tactile::loop() {

  // Respond to sensor touch/proximity
  _touchLoop();

  // Do fade-in/out
  _ta->doTimerTasks();
  
  // Do vibrator tasks
  _v->doTimerTasks();

  // If the idle-time has expired, reset the continue-track feature to start over
  uint32_t elapsed = millis() - _lastActionTime;
  if (elapsed > _restartTimeout) {
    if (_ta->cancelAll()) {
      _tu->logAction("Inactivity timeout: ", _restartTimeout);
      _lastActionTime = millis();
    }
  }
}


