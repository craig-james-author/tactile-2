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
  void setMultiTrackMode(bool on);             // true == enable multiple simultaneous tracks
  void ignoreSensor(int channel, bool ignore);
  void setTouchReleaseThresholds(int touch, int release);
  void setTouchReleaseThresholds(int channel, int touch, int release);
  void setTouchToStop(int channel, bool on);            // true == touch-on-touch-off (normally touch-on-release-off)
  void setTouchToStop(bool on);
  void setContinueTrackMode(int channel, bool on);      // true == 2nd touch continues track where it left off
  void setContinueTrackMode(bool on);
  void setLoopMode(int channel, bool on);               // true == track restarts (loops) when end reached
  void setLoopMode(bool on);

  /*---------- These are forwarded to the AudioPlayer module ----------*/
  void setVolume(int channel, int percent);
  void setVolume(int percent);
  void useProximityAsVolume(int channel, bool on);    // Proximity controls volume, or fixed volume
  void useProximityAsVolume(bool on);
  void setProximityMultiplier(int channel, float m);  // 1.0 is no amplification, more increases sensitivity
  void setFadeInTime(int channel, int milliseconds);
  void setFadeInTime(int milliseconds);
  void setFadeOutTime(int channel, int milliseconds);
  void setFadeOutTime(int milliseconds);
  void useRandomTracks(int channel, bool on);         // true == random selection from sensor's directory
  void useRandomTracks(bool on);
  void setAveragingStrength(int samples);             // more smooths signal, default is 200
  const char *getTrackName(int channel);

  // renamed -- use #define so that Tactile v1 sketches will work
#define setProximityAsVolumeMode useProximityAsVolume
#define setPlayRandomTrackMode useRandomTracks
  
  /*---------- These are forwarded to the Vibrator module ----------*/
  void setVibratorType(int channel, VibratorType vibType);
  void setVibrationIntensity(int channel, int intensity);
  void setVibrationEnvelope(int channel, const char *name);
  void setVibrationEnvelopeFile(int channel, const char *fileName);
  void useProximityAsIntensity(int channel, bool on);
  void useProximityAsSpeed(int channel, bool on, int multiplierPercent);
  void overrideVibrationEnvelopeDuration(int channel, int msec);
  void overrideVibrationEnvelopeRepeats(int channel, bool repeat);
  void setVibrationFrequency(int channel, int frequency);

 private:
  TeensyUtils      *_tu;
  Sensors          *_ts;
  AudioPlayer      *_ta;
  AudioFileManager *_fm;
  Vibrate          *_v;

  // Options set via API calls
  bool     _useAudioOutput[NUM_CHANNELS];
  bool     _useVibrationOutput[NUM_CHANNELS];
  bool     _useTouchInput[NUM_CHANNELS];
  bool     _useAudioInput[NUM_CHANNELS];
  float    _touchThreshold[NUM_CHANNELS];
  float    _releaseThreshold[NUM_CHANNELS];
  bool     _touchToStop[NUM_CHANNELS];
  bool     _continueTrack[NUM_CHANNELS];
  bool     _useProximityAsVolume[NUM_CHANNELS];
  bool     _proximityControlsIntensity[NUM_CHANNELS];
  bool     _proximityControlsSpeed[NUM_CHANNELS];
  int      _speedMultiplierPercent[NUM_CHANNELS];
  bool     _multiTrack;

  // Bookkeeping while playing
  bool     _isPlaying[NUM_CHANNELS];
  uint32_t _restartTimeout;
  uint32_t _lastActionTime;
  int      _ledCycle;
  
  void _touchLoop();
  void _proximityLoop();
  void _doVolumeFadeInAndOut();
  void _startTrackIfStartDelayReached();
  void _doTimerTasks();
};

#endif
