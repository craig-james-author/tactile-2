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

#include "Arduino.h"

#include <AudioPlayer.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "AudioPlayer.h"

// GUItool: begin automatically generated code
AudioPlaySdWavPR           playSdWav1;     //xy=124,100
AudioPlaySdWavPR           playSdWav2;     //xy=124,160
AudioPlaySdWavPR           playSdWav3;     //xy=124,220
AudioPlaySdWavPR           playSdWav4;     //xy=124,280
AudioMixer4              mixer1;         //xy=470,160
AudioMixer4              mixer2;         //xy=470,280
AudioOutputI2S           i2s1;           //xy=650,220
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord3(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord4(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord5(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord6(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord7(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord8(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord9(mixer1, 0, i2s1, 0);
AudioConnection          patchCord10(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000;     //xy=127,379.111083984375
// GUItool: end automatically generated code

AudioPlayer::AudioPlayer(TeensyUtils *tc) {
  _tu = tc;
}

AudioPlayer* AudioPlayer::setup(TeensyUtils *tc) {

  AudioPlayer* t = new AudioPlayer(tc);

  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    t->setVolume(channel, 100);
    t->_fadeInTime[channel]            = 0;
    t->_fadeOutTime[channel]           = 0;
    t->_randomTrackMode[channel]       = false;
    t->_loopMode[channel]              = false;
    t->_targetVolume[channel]          = 100;
    t->_actualVolume[channel]          = 0;
    t->_lastStartTime[channel]         = 0;
    t->_lastStopTime[channel]          = 0;
    t->_thisFadeInTime[channel]        = 0;
    t->_thisFadeOutTime[channel]       = 0;
    t->_lastRandomTrackPlayed[channel] = -1;
    t->_isPaused[channel]              = false;
  }  

  // Initialization for the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
  AudioMemory(2*NUM_CHANNELS+4);
  sgtl5000.enable();
  sgtl5000.volume(0.90);
  delay(1000);  // wait for SGTL5000 to initialize

  for (int i = 0; i < 4; i++) {
    mixer1.gain(i, 1.0);
    mixer2.gain(i, 1.0);
  }

  t->_fm = new AudioFileManager(tc);
 
  tc->log2("AudioPlayer::setup() complete.");

  return t;
}

/*----------------------------------------------------------------------
 * Tracks
 ----------------------------------------------------------------------*/

const char *AudioPlayer::getTrackName(int channel) {
  if (channel < 0 || channel > NUM_CHANNELS)
    return NULL;
  return _fm->getFileName(channel);
}

/*----------------------------------------------------------------------
 * Volume controls
 ----------------------------------------------------------------------*/

void AudioPlayer::_setActualVolume(int channel, int percent) {
  _actualVolume[channel] = percent;
  float gain  = (float)percent/100.0;  // Convert percent (0-100) to gain (0-1.0)
  mixer1.gain(channel, gain);
  mixer2.gain(channel, gain);
}

void AudioPlayer::setVolume(int channel, int percent) {
  _targetVolume[channel] = percent;
  if (!_fadeInTime[channel])
    _setActualVolume(channel, percent);
}

void AudioPlayer::setFadeInTime(int channel, int milliseconds) {
  _fadeInTime[channel] = milliseconds;
  _tu->logAction2("AudioPlayer: setFadeInTime: ", milliseconds);
}

void AudioPlayer::setFadeOutTime(int channel, int milliseconds) {
  _fadeOutTime[channel] = milliseconds;
  _tu->logAction2("AudioPlayer: setFadeOutTime: ", milliseconds);
}

void AudioPlayer::cancelFades(int channel) {
  _lastStartTime[channel] = 0;
  _lastStopTime[channel] = 0;
  _setActualVolume(channel, 0);
}
  
int AudioPlayer::cancelAll() {
  int cancelled = 0;
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
    if (player) {
      if (player->isPlaying()) {
        player->stop();
        cancelled++;
      }
    }
    _lastStartTime[channel] = 0;
    _lastStopTime[channel] = 0;
  }
  return cancelled;
}    

/*----------------------------------------------------------------------
 * Play, pause, resume, and stop tracks
 ----------------------------------------------------------------------*/

void AudioPlayer::useRandomTracks(int channel, bool r) {
  _randomTrackMode[channel] = r;
}

void AudioPlayer::setLoopMode(int channel, bool on) {
  _loopMode[channel] = on;
}

AudioPlaySdWavPR *AudioPlayer::_getPlayerByTrack(int channel) {
  switch (channel) {
  case 0: return &playSdWav1;
  case 1: return &playSdWav2;
  case 2: return &playSdWav3;
  case 3: return &playSdWav4;
  }
  _tu->logAction("AudioPlayer: Invalid channel: ", channel);
  return NULL;  // to keep compiler happy, never happens
}

void AudioPlayer::startTrack(int channel) {
  if (_randomTrackMode[channel])
    _startRandomTrack(channel);
  else
    _startTrack(channel);
  if (_fadeInTime[channel] == 0)
    _setActualVolume(channel, _targetVolume[channel]);
  else
    _thisFadeInTime[channel] = _calculateFadeTime(channel, true);
  _lastStartTime[channel] = millis();
  _lastStopTime[channel] = 0;
}

void AudioPlayer::_startTrack(int channel) {
  const char *trackName = _fm->getFileName(channel);
  if (!trackName) {
    _tu->logAction("Can't find that track: ", channel);
    return;
  }
  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player) return;
  player->play(trackName);
  if (getLogLevel() > 1) {
    Serial.print("AudioPlayer: start track ");
    Serial.print(channel);
    Serial.print(", ");
    Serial.println(trackName);
  }
}

void AudioPlayer::_startRandomTrack(int channel) {
  // Selects a track randomly from directory EN, where "N"
  // is the track number (e.g. E0, E1, ...). However, it
  // tries to avoid replaying the last-played "random" track.

  _tu->logAction2("AudioPlayer: startRandomTrack ", channel);

  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player) return;

  int numFiles = _fm->getNumFiles(channel);
  _tu->logAction2("AudioPlayer: Files in directory: ", numFiles);
  if (numFiles < 1) return;

  int r;
  int tries = 0;
  while (tries < 30) {
    r = random(numFiles);
    if (r != _lastRandomTrackPlayed[channel])       // avoid last track played
      break;
    tries++;
  }
  _lastRandomTrackPlayed[channel] = r;
  _tu->logAction2("AudioPlayer: Random track selected: ", r);
  const char *fileName = _fm->getFileName(channel, r);
  if (!fileName) {
    _tu->logAction2("Error, couldn't get random filename (this shouldn't happen) for track ", channel);
    return;
  }

  char filePath[MAX_FILE_NAME+5] = "/Ex/";
  filePath[2] = '1' + channel;  // i.e. /E1/, /E2/, ...
  strcpy(filePath+4, fileName);
  _tu->log2(filePath);

  player->play(filePath);

  if (getLogLevel() > 1) {
    Serial.print("AudioPlayer: start random track: ");
    Serial.print(filePath);
    Serial.print(" (");
    Serial.print(channel);
    Serial.print(", ");
    Serial.print(r);
    Serial.println(")");
  }
}

void AudioPlayer::stopTrack(int channel) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player) return;
  if (_fadeOutTime[channel] == 0) {
    player->stop();
    _setActualVolume(channel, 0);
  } else {
    // If fade-out enabled, don't actually stop the track. That will happen
    // when fade-out finishes (see _doFadeInOut(), below).
    _thisFadeOutTime[channel] = _calculateFadeTime(channel, false);
  }

  _tu->logAction2("AudioPlayer: stop ", channel);
  _lastStopTime[channel] = millis();  // for calculating fade-out
  _lastStartTime[channel] = 0;
}  

bool AudioPlayer::isPlaying(int channel) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player) return 0;
  return player->isPlaying();
}

/*----------------------------------------------------------------------
 * Pause and Resume tracks
 ----------------------------------------------------------------------*/

void AudioPlayer::pauseTrack(int channel) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player|| !player->isPlaying()) return; 
  if (_fadeOutTime[channel] == 0) {
    player->pause();
    _setActualVolume(channel, 0);
  } else {
    // If fade-out enabled, don't actually pause the track. That will happen
    // when fade-out finishes (see _doFadeInOut(), below).
    _thisFadeOutTime[channel] = _calculateFadeTime(channel, false);
  }
  _isPaused[channel] = true;
  _lastStartTime[channel] = 0;
  _lastStopTime[channel] = millis();     // for calculating fade-out
  _tu->logAction2("AudioPlayer: pause ", channel);
}

void AudioPlayer::resumeTrack(int channel) {

  AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
  if (!player) return;

  player->resume();
  _isPaused[channel] = false;
  if (_fadeInTime[channel] == 0)
    _setActualVolume(channel, _targetVolume[channel]);
  else
    _thisFadeInTime[channel] = _calculateFadeTime(channel, true);

  _lastStartTime[channel] = millis();
  _lastStopTime[channel] = 0;

  _tu->logAction2("AudioPlayer: resume ", channel);
}

bool AudioPlayer::isPaused(int channel) {
  return _isPaused[channel];
}

int AudioPlayer::_calculateFadeTime(int channel, bool goingUp) {
  int deltaVolume;
  int fadeTime;
  if (goingUp) {
    deltaVolume = 100 - _actualVolume[channel];
    fadeTime = _fadeInTime[channel];
  } else {
    deltaVolume = _actualVolume[channel];
    fadeTime = _fadeOutTime[channel];
  }
  int time = (int)(0.499 + (float)fadeTime * (float)deltaVolume/100.0);
  return time;
}

void AudioPlayer::_doFadeInOut(int channel)
{
  int targetVol = _targetVolume[channel];
  int actualVol = _actualVolume[channel];

  // Do fade-in? When fade-in is enabled, tracks are started at zero volume,
  // so they're initially in the "is playing" state even though the volume
  // is zero.

  if (_lastStartTime[channel] > 0
      && _fadeInTime[channel] != 0
      && actualVol < targetVol
      && isPlaying(channel)) {

    // Calculate the target volume based on how much elapsed time since the track started playing.
    unsigned long elapsedTime = millis() - _lastStartTime[channel];

    // But if _thisFadeInTime is different that _fadeInTime, it means we started in the middle
    // of a fade-out (i.e. the volume wasn't zero yet), so push the elapsed time forward by
    // difference of the two.
    if (_fadeInTime[channel] != _thisFadeInTime[channel])
      elapsedTime += _fadeInTime[channel] - _thisFadeInTime[channel]; // push elapsed time forward
    
    int newVolumePercent = int((float)targetVol*(float)elapsedTime/(float)_fadeInTime[channel]);
    
    // Time to increase volume?
    if (newVolumePercent != actualVol) {
      if (newVolumePercent >= targetVol) {
	newVolumePercent = targetVol;
      }
      _setActualVolume(channel, newVolumePercent);
    }
  }

  // Else -- do fade-out? When fade-out is enabled, a track can be in the "is stopped"
  // state but still be actually playing. Only when the target volume gets to zero
  // is the track actually stopped.

  else if (_fadeOutTime[channel] != 0               // fade-out is enabled
           && _lastStopTime[channel] > 0            // the track is stopped or paused...
           && isPlaying(channel)                    // ... but the track is still playing
	   && actualVol > 0) {                      // and volume hasn't reached zero yet

    // Calculate the target volume based on how much elapsed time since the track stopped playing.
    unsigned long elapsedTime = millis() - _lastStopTime[channel];

    // But if _thisFadeOutTime is different than _fadeOutTime, it means we started with volume less
    // than 100%, so push elapsed time forward by the difference of the two.
    if (_fadeOutTime[channel] != _thisFadeOutTime[channel]) {
      elapsedTime += _fadeOutTime[channel] - _thisFadeOutTime[channel];
    }

    int newVolumePercent = targetVol - int((float)(targetVol)*(float)elapsedTime/(float)_fadeOutTime[channel]);

    if (newVolumePercent != actualVol) {

      // Time to decrease volume.
      if (newVolumePercent < 0)
	newVolumePercent = 0;
      _setActualVolume(channel, newVolumePercent);

      // If fade-out reached zero volume, actually stop or pause the track.
      // Note that _isPaused is true as soon as pauseTrack()
      // is called, but the track keeps playing until this fade-out finishes.
      if (newVolumePercent == 0) {
        AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
        if (!player) return;
	if (isPaused(channel)) {
          player->pause();
	  _tu->logAction2("AudioPlayer: fade-out done, track paused: ", channel);
	} else {
	  player->stop();
          _tu->logAction2("AudioPlayer: fade-out done, track stopped: ", channel);
	}
        _lastStopTime[channel] = 0;
      }
    }
  }
}

void AudioPlayer::doTimerTasks()
{
  for (int channel = 0; channel < NUM_CHANNELS; channel++)
    _doFadeInOut(channel);

  // If a track that was playing reached the end of the track, change its status.
  for (int channel = 0; channel < NUM_CHANNELS; channel++) {
    if (_lastStartTime[channel] > 0) {
      AudioPlaySdWavPR *player = _getPlayerByTrack(channel);
      if (!player) return;
      uint32_t now = millis();
      if (now - _lastStartTime[channel] > 50) {  // Player doesn't reliably report isPlaying() for a
        if (!player->isPlaying()) {                  // few msec, so if it just started playing, skip this.
          if (_loopMode[channel]) {
            startTrack(channel);
            _tu->logAction2("end of track, looping: ", channel);
          } else {
            _lastStartTime[channel] = 0;
            _lastStopTime[channel] = now;
            _tu->logAction2("end of track ", channel);
            stopTrack(channel);
          }
        }
      }
    }
  }
}
