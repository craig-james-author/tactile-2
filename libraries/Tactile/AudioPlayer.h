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

#ifndef AudioPlayer_h
#define AudioPlayer_h 1

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "TeensyUtils.h"
#include "AudioFileManager.h"
#include "AudioPlaySdWavPR.h"     // extension of AudioPlayer.h that adds pause/resume feature

class AudioPlayer
{
 public:

  AudioPlayer(TeensyUtils *tc);
  static AudioPlayer* setup(TeensyUtils *tc);

  const char *getTrackName(int trackNum);

  void setVolume(int percent);
  void setVolume(int trackNumber, int percent);
  void setFadeInTime(int milliseconds);
  void setFadeOutTime(int milliseconds);
  void cancelFades(int trackNumber);

  void setPlayRandomTrackMode(bool r);
  void setLoopMode(bool on);

  void startTrack(int sensorNumber);
  void stopTrack(int sensorNumber);
  bool isPlaying(int sensorNumber);

  void pauseTrack(int sensorNumber);
  void resumeTrack(int sensorNumber);
  bool isPaused(int sensorNumber);

  int  cancelAll();

  void doTimerTasks();
  
 private:

  TeensyUtils *_tu;
  AudioFileManager *_fm;

  // Volume control
  int _targetVolume[NUM_CHANNELS];
  int _actualVolume[NUM_CHANNELS];
  int _fadeInTime;
  int _fadeOutTime;

  bool _randomTrackMode;
  bool _loopMode;

  // Audio player status (per track)
  uint32_t _lastStartTime[NUM_CHANNELS];
  uint32_t _lastStopTime[NUM_CHANNELS];
  int      _thisFadeInTime[NUM_CHANNELS];
  int      _thisFadeOutTime[NUM_CHANNELS];
  int      _lastRandomTrackPlayed[NUM_CHANNELS];
  bool     _isPaused[NUM_CHANNELS];
  
  // Internal methods
  AudioPlaySdWavPR *_getPlayerByTrack(int trackNumber);
  uint8_t _volumePctToByte(int percent);
  void    _setActualVolume(int trackNum, int percent);
  int     _calculateFadeTime(int trackNumber, bool goingUp);
  void    _doFadeInOut(int trackNumber);
  void    _startTrack(int trackNumber);
  void    _startRandomTrack(int trackNumber);
};

#endif
