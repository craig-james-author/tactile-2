/* -*-C-*-
+======================================================================
| Copyright (c) 2025, Craig A. James
|
| This file is part of of the "TactileAudio" library.
|
| TactileAudio is free software: you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License (LGPL) as published by
| the Free Software Foundation, either version 3 of the License, or (at
| your option) any later version.
|
| TactileAudio is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.
|
| You should have received a copy of the LGPL along with TactileAudio. If not,
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

static int channelExtern2Intern(int channel) {
  channel -= 1;
  if (channel < 0) return 0;
  if (channel >= NUM_CHANNELS) return NUM_CHANNELS - 1;
  return channel;
}

void Tactile::setLogLevel(int level) {
  ::setLogLevel(level);
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

void Tactile::useRandomTracks(int channel, bool on, bool shuffle) {
  channel = channelExtern2Intern(channel);
  _ta->useRandomTracks(channel, on, shuffle);
}
void Tactile::useRandomTracks(int channel, bool on) {     // obsolete, backwards compatible
  useRandomTracks(channel, on, false);
}

void Tactile::useRandomTracks(bool on, bool shuffle) {
  for (int ch = 0; ch < NUM_CHANNELS; ch++)
    useRandomTracks(ch, on, false);
}
void Tactile::useRandomTracks(bool on) {                   // obsolete, backwards compatible
  useRandomTracks(on, false);
}

void Tactile::useProximityAsVolume(int channel, bool on) {
  channel = channelExtern2Intern(channel);
  _useProximityAsVolume[channel] = on;
  if (on) {
    _ta->setFadeInTime(channel, 0);        // Fade in/out isn't compatible with proximity-as-volume
    _ta->setFadeOutTime(channel, 0);
  }
}

void Tactile::useProximityAsVolume(bool on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    useProximityAsVolume(ch, on);
}

void Tactile::setFadeInTime(int channel, int milliseconds) {
  channel = channelExtern2Intern(channel);
  if (milliseconds > 0 && _useProximityAsVolume[channel]) {
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
  if (milliseconds > 0 && _useProximityAsVolume[channel]) {
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

void Tactile::setMultiTrackMode(bool on) {
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

void Tactile::setTouchToStop(int channel, bool on) {
  channel = channelExtern2Intern(channel);
  _touchToStop[channel] = on;
  _ts->setTouchToggleMode(channel, on);
}

void Tactile::setTouchToStop(bool on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setTouchToStop(ch, on);
}

void Tactile::setContinueTrackMode(int channel, bool on) {
  channel = channelExtern2Intern(channel);
  _continueTrack[channel] = on;
}

void Tactile::setContinueTrackMode(bool on) {
  for (int ch = 1; ch <= NUM_CHANNELS; ch++)
    setContinueTrackMode(ch, on);
}

void Tactile::setLoopMode(int channel, bool on) {
  channel = channelExtern2Intern(channel);
  _ta->setLoopMode(channel, on);
}

void Tactile::setLoopMode(bool on) {
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

void Tactile::addCustomVibrationEnvelope(VibrationEnvelope &ve) {
  _v->addCustomVibrationEnvelope(ve);
}
void Tactile::setVibrationEnvelope(int channel, const char *name) {
  channel = channelExtern2Intern(channel);
  _v->setVibrationEnvelope(channel, name);
}
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
    multiplierPercent = 1000;
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
  t->_ts = Sensors::setup(t->_tu);
  t->_ta = AudioPlayer::setup(t->_tu);
  t->_v  = Vibrate::setup(t->_tu);
  
  // Audio initialization
  for (int c = 1; c <= NUM_CHANNELS; c++) {
    t->setTouchReleaseThresholds(c, 95, 65);
    t->setContinueTrackMode(c, false);
    t->useRandomTracks(c, false, false);
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
  t->_lastActionTime = millis();
  for (int c = 0; c < NUM_CHANNELS; c++) {
    t->_isPlaying[c] = false;
  }
  
  // Generate a "random" seed for the random() function. See
  // Sensors.h for the definition of the unused analog pin.
  randomSeed(analogRead(UNUSED_ANALOG_INPUT));

  return t;
}

/*----------------------------------------------------------------------
 * TOUCH-MODE LOOP
 *
 * MULTI-TRACK MODE. Simple: if a sensor is touched, start playing the
 * track; if it's released, stop playing. Multiple tracks can go at
 * the same time.
 *
 * SINGLE-TRACK MODE. This is, surprisingly, a bit more complicated.
 * 
 * When a sensor is touched, play that track if nothing is already
 * playing, and as long as the sensor is still touched, keep playing.
 *
 * When a sensor is released, it's more complicated.
 *   - If no other sensor is being touched, just stop the track playing.
 *   - If one or more other sensors are being touched as this one
 *     is released, select the lowest, and consider it a "new touch",
 *     that is, start that track.
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

  if (numChanged > 0) {

    // Process releases first (makes bookkeeping easier for single-track mode).
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {

      if (sensorChanged[channel] == NEW_RELEASE) {

        _tu->logAction2("stop: continueTrack = ", _continueTrack[channel]);

        // Stop audio
        if (_useAudioOutput[channel]) {
          if (_isPlaying[channel]) {
            if (_continueTrack[channel]) {
              _tu->logAction("pause audio ", channel+1);
              _ta->pauseTrack(channel);
            } else {
              _tu->logAction("stop audio ", channel+1);
              _ta->stopTrack(channel);
            }
          }
        }

        // Stop vibration
        if (_useVibrationOutput[channel]) {
          _tu->logAction("stop vibrator ", channel+1);
          _v->stop(channel);
        }

        _isPlaying[channel] = false;
      }
    }

    // Now that we've processed releases, are there any still playing?
    // Need to know this for single-track mode.
    bool nothingIsPlaying = true;
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
      if (_isPlaying[channel])
        nothingIsPlaying = false;
    }

    // Process new touches
    if (_multiTrack | nothingIsPlaying) {

      for (int channel = 0; channel < NUM_CHANNELS; channel++) {

        // For multi-track, a "Touch" is simply a new touch.
        // For single-track, it can also include an existing touch if the
        // track isn't playing (i.e. the new touch came while another track
        // was playing).
        if (sensorChanged[channel] == NEW_TOUCH
            || (!_multiTrack && ((sensorStatus[channel] == IS_TOUCHED) && !_isPlaying[channel]))) {

          // Start or resume audio
          if (_useAudioOutput[channel]) {
            if (_continueTrack[channel]) {
              if (_ta->isPaused(channel)) {
                _tu->logAction("resume audio track ", channel+1);
                _ta->resumeTrack(channel);
              } else {
                _tu->logAction("restart audio track ", channel+1);
                _ta->startTrack(channel);
              }
            } else {
              if (!_isPlaying[channel]) {
                _tu->logAction("start audio track ", channel+1);
                _ta->cancelFades(channel);
                _ta->startTrack(channel);
              }
            }
          }
          _tu->logAction2("start: continueTrack = ", _continueTrack[channel]);


          // Start vibration. This is much simpler.
          if (_useVibrationOutput[channel]) {
            _tu->logAction("start vibrator ", channel+1);
            _v->start(channel);
          }

          _isPlaying[channel] = true;

          // If single-track mode, stop at the first NEW_TOUCH
          if (!_multiTrack)
            break;
        }
      }
    }

  }     // end of "if (numChanges > 0)"

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {

    // Proximity-as-volume for audio
    if (_isPlaying[channel] && _useAudioOutput[channel] && _useProximityAsVolume[channel]) {
      _ta->setVolume(channel, proximityValues[channel]);
    }

    if (_v->isPlaying(channel) && _useVibrationOutput[channel]) {
      // Proximity-as-speed for vibration: adjust speed
      if (_proximityControlsSpeed[channel]) {
        int multiplier = (int)(0.499 + (float)_speedMultiplierPercent[channel]/100.0 * proximityValues[channel]);
        _v->setSpeedMultiplier(channel, multiplier);
      }
      // Proximity-as-intensity for vibration: adjust intensity
      else if (_proximityControlsIntensity[channel]) {
        _v->setIntensity(channel, proximityValues[channel]);
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


