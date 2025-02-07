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

#ifndef Tactile_h
#define Tactile_h 1

#include "TactileBasics.h"
#include "TeensyUtils.h"
#include "Sensors.h"
#include "AudioPlayer.h"
#include "Vibrate.h"

#define TOUCH_MODE 1
#define PROXIMITY_MODE 2

class Tactile
{
 public:

  static Tactile *setup();
  void loop(void);
  void setInactivityTimeout(int seconds);      // continueTrackMode: reset to beginning if idle this long

  // Input source: touchInput or audioInput (mutually exclusive)
  void setInputSource(int channel, InputSource iSource);

  // Output destination: audio or vibration. Can be both.
  void setOutputDestination(int channel, OutputDest dest1, OutputDest dest2 = noOutput);


  /*---------- This is forwarded to the TeensyUtils class ----------*/
  void setLogLevel(int level);

  /*---------- These are forwarded to the Sensors class ----------*/
  void setTouchReleaseThresholds(int touch, int release);
  void setTouchReleaseThresholds(int sensorNumber, int touch, int release);
  void ignoreSensor(int sensorNumber, bool ignore);
  void setTouchToStop(bool on);                // true == touch-on-touch-off (normally touch-on-release-off)
  void setMultiTrackMode(bool on);             // true == enable multiple simultaneous tracks
  void setContinueTrackMode(bool on);          // true == 2nd touch continues track where it left off
  void setLoopMode(bool on);                   // true == track restarts (loops) when end reached

  /*---------- These are forwarded to the AudioPlayer module ----------*/
  void setVolume(int percent);
  void setProximityAsVolumeMode(bool on);      // Proximity controls volume, or fixed volume
  void setProximityMultiplier(int sensorNumber, float m);  // 1.0 is no amplification, more increases sensitivity
  void setFadeInTime(int milliseconds);
  void setFadeOutTime(int milliseconds);
  void setAveragingStrength(int samples);      // more smooths signal, default is 200
  void setPlayRandomTrackMode(bool on);        // true == random selection from sensor's directory
  const char *getTrackName(int trackNum);
  
  /*---------- These are forwarded to the Vibrator module ----------*/
  void setVibrationEnvelope(int channel, const char *name);
  void setVibrationEnvelopeFile(int channel, const char *fileName);
  void overrideVibrationEnvelopeDuration(int channel, int msec);
  void overrideVibrationEnvelopeRepeats(int channel, bool repeat);
  void setVibrationFrequency(int channel, int frequency);

 private:
  TeensyUtils      *_tu;
  Sensors          *_ts;
  AudioPlayer      *_ta;
  AudioFileManager *_fm;
  Vibrate          *_v;

  bool     _useAudioOutput[NUM_CHANNELS];
  bool     _useVibrationOutput[NUM_CHANNELS];
  bool     _useTouchInput[NUM_CHANNELS];
  bool     _useAudioInput[NUM_CHANNELS];
  float    _touchThreshold[NUM_CHANNELS];
  float    _releaseThreshold[NUM_CHANNELS];
  bool     _touchToStop;
  bool     _multiTrack;
  bool     _continueTrack;
  uint32_t _restartTimeout;
  bool     _useProximityAsVolume;
  int      _trackCurrentlyPlaying;
  uint32_t _lastActionTime;
  int      _ledCycle;
  
  void _touchLoop();
  void _proximityLoop();
  void _doVolumeFadeInAndOut();
  void _startTrackIfStartDelayReached();
  void _doTimerTasks();
};

#endif
