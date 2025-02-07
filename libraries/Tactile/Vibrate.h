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

#ifndef Vibrate_h
#define Vibrate_h 1

#include "TeensyUtils.h"
#include "AudioFileManager.h"

// A "vibration envelope" is a series of volume settings applied to a
// vibrator output over a specified period of time.

struct VibrationEnvelope {
  char name[20];
  int numberOfPoints;   // length of "volumes" array (cached)
  int msecPerPoint;     // msec per item in "volumes" array (cached)
  int msecTotal;        // time from start to finish
  boolean repeats;      // loops? or once-and-stop?
  int volumes[200];     // a list of volume values applied every msecPerPoint
};


// This module controls the vibrators: mode (VIBRATE, PULSE), volume,
// and timing.

class Vibrate
{
 public:

  // Constructors
  Vibrate(TeensyUtils* tc);
  static Vibrate* setup(TeensyUtils* tc);

  // channels are 0..N-1
  void start     (int channel);
  void stop      (int channel);
  bool isPlaying (int channel);

  // When proximity isn't used...
  void setVolume (int channel, int percent);
  void setVolume (int percent);

  // "Volume" controls volume, or controls speed (period)?
  void useVolumeAsSpeed(bool on);

  // built-in patterns, or pattern defined on the SD card
  void setVibrationEnvelope(int channel, const char *name);
  void setVibrationEnvelopeFile(int channel, const char *fileName);
  void overrideVibrationEnvelopeDuration(int channel, int msec);
  void overrideVibrationEnvelopeRepeats(int channel, bool repeat);
  void setVibrationFrequency(int channel, int frequency);

  void doTimerTasks();
  
  // Normally leave this at default.
  void setPwmFrequency(int frequency);

 private:

  TeensyUtils *_tc;

  // Each channel has a pair of pins assigned. These are selected for the
  // Teensy 4.1 because they have PWM (pulse-width modulation) capabilities
  // and because they are not used by the Teensy Audio Shield.
  //
  //    [ 0  1]
  //    [24 25]
  //    [28 29]
  //    [36 37]
  //    
  // Note the difference between "pin" and "channel". The pin corresponds
  // to a specific pin number on the Teensy device. A "channel" is an
  // abstraction associated with two pins to drive an external device
  // such as a motor or vibrator.

  static constexpr int _channelNumberToPinNumbers[] =
    { 0 |  1<<8,
     24 | 25<<8,
     28 | 29<<8,
     36 | 37<<8
    };
  int _convertChannelToPins(int channel);
  int _convertChannelToPin1(int channel);
  int _convertChannelToPin2(int channel);
  int _checkChannel(int channel);
  int _readln(File f, char *buf, int bufLen);

  void  _calculateLengthOfEnvelope(int channel);
  void _calculateMsecPerEnvelopePoint(int channel);

  int _calculateActualVolume(int channel, int volume);

  /*----------------------------------------------------------------------
   * Each channel is assigned a "vibration envelope", either from the
   * built-in list or from a file on the SD card. An envelope specifies a
   * changing volume over a fixed period of time.
   *
   * There are four time/frequency parameters that can be easily confused.
   *
   * Vibration frequency
   *   This is the frequency of the vibrator, typically 40-250 Hz.
   *
   * Envelope length
   *   Each volume-profile "envelope" has a length, the time between
   *   repeats of the envelope. For example, a single cycle "sawtooth"
   *   pattern might be one second, meaning the volume would rise to
   *   maximum for 1/2 second, then fall to zero for another 1/2 second,
   *   then repeat.
   *
   * msecPerPoint
   *   Envelopes have varying lengths (number of volume points). For example,
   *   a smoothly-descending volume might have 40 values, whereas
   *   a on/of (square wave) might just have two values. Imagine the
   *   envelope length (above) is one second. In that case, the two-point
   *   square wave gets 500 msec per point, whereas the 40-point smooth
   *   descending volume would get 25 msec per point.
   *   
   * PWM frequency
   *   The pulse-width modulation frequency is more-or-less irrelevant to
   *   everything else. It's typically in the neighborhood of 100KHz.
   *   Ignore it; it's rarely changed and irrelevant to how this module
   *   works.
   *
   * The following internal variables maintain these values and keep track
   * of the state of the vibrator output.
   ----------------------------------------------------------------------*/

  VibrationEnvelope _vibrationEnvelope[NUM_CHANNELS];
  int  _setVolume[NUM_CHANNELS]                     = {0, 0, 0, 0};
  int  _actualVolume[NUM_CHANNELS]                  = {0, 0, 0, 0};
  bool _isPlaying[NUM_CHANNELS]                     = {false, false, false, false};

  // managing volume envelope
  int _indexInEnvelope[NUM_CHANNELS]                = {0, 0, 0, 0};
  unsigned long _startTimeForPoint[NUM_CHANNELS]    = {0, 0, 0, 0};

  // managing vibration frequency
  bool _currentState[NUM_CHANNELS]                  = {false, false, false, false};
  int  _vibrationFrequency[NUM_CHANNELS]            = {0, 0, 0, 0};
  int  _vibrationPeriod[NUM_CHANNELS]               = {0, 0, 0, 0};     // period = 1/frequency
  unsigned long _startTimeForVibration[NUM_CHANNELS]= {0, 0, 0, 0};

  int _pwmFrequency;
  
};

#endif
