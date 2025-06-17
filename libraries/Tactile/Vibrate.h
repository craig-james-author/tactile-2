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

// A "vibration envelope" is a series of intensity settings applied to a
// vibrator output over a specified period of time.

#define VIB_ENVELOPE_MAX_NAME 100
#define VIB_ENVELOPE_MAX_POINTS 200

struct VibrationEnvelope {
  char name[VIB_ENVELOPE_MAX_NAME];
  int numberOfPoints;   // length of "intensities" array (cached)
  int msecPerPoint;     // msec per item in "intensities" array (cached)
  int msecTotal;        // time from start to finish
  boolean repeats;      // loops? or once-and-stop?
  int intensities[VIB_ENVELOPE_MAX_POINTS]; // a list of intensity values applied every msecPerPoint
};

// This module controls the vibrators: mode (VIBRATE, PULSE), intensity,
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

  void addCustomVibrationEnvelope(VibrationEnvelope &ve);

  void setVibrationEnvelope(int channel, const char *name);
  void setIntensity (int channel, int percent);
  void setIntensity (int percent);
  void setSpeedMultiplier(int channel, int multiplierPercent);
  void overrideVibrationEnvelopeDuration(int channel, int msec);
  void overrideVibrationEnvelopeRepeats(int channel, bool repeat);
  void setVibratorType(int channel, VibratorType vibType);
  void setVibrationFrequency(int channel, int frequency);
  void doTimerTasks();
  
  // Custom (user-defined) patterns

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

  void _calculateLengthOfEnvelope(int channel);
  void _calculateMsecPerEnvelopePoint(int channel);

  int _calculateActualIntensity(int channel, int intensity);
  int _calculateActualPeriod(int channel, int period);

  /*----------------------------------------------------------------------
   * Each channel is assigned a "vibration envelope", either from the
   * built-in list or from a file on the SD card. An envelope specifies a
   * changing intensity over a fixed period of time.
   *
   * There are four time/frequency parameters that can be easily confused.
   *
   * Vibration frequency
   *   This is the frequency of the vibrator, typically 40-250 Hz.
   *
   * Envelope length
   *   Each intensity-profile "envelope" has a length, the time between
   *   repeats of the envelope. For example, a single cycle "sawtooth"
   *   pattern might be one second, meaning the intensity would rise to
   *   maximum for 1/2 second, then fall to zero for another 1/2 second,
   *   then repeat.
   *
   * msecPerPoint
   *   Envelopes have varying lengths (number of intensity points). For example,
   *   a smoothly-descending intensity might have 40 values, whereas
   *   a on/of (square wave) might just have two values. Imagine the
   *   envelope length (above) is one second. In that case, the two-point
   *   square wave gets 500 msec per point, whereas the 40-point smooth
   *   descending intensity would get 25 msec per point.
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
  int  _setIntensity[NUM_CHANNELS]                   = {100, 100, 100, 100};
  int  _actualIntensity[NUM_CHANNELS]                = {0, 0, 0, 0};
  bool _isPlaying[NUM_CHANNELS]                      = {false, false, false, false};

  // managing intensity envelope
  int _indexInEnvelope[NUM_CHANNELS]                 = {0, 0, 0, 0};
  unsigned long _startTimeForPoint[NUM_CHANNELS]     = {0, 0, 0, 0};

  // managing vibration frequency
  bool          _currentState[NUM_CHANNELS]          = {false, false, false, false};
  int           _vibrationPeriod[NUM_CHANNELS]       = {0, 0, 0, 0};     // period = 1/(frequency * speedMultiplier)
  int           _speedMultiplierPercent[NUM_CHANNELS]= {100, 100, 100, 100};
  unsigned long _startTimeForVibration[NUM_CHANNELS] = {0, 0, 0, 0};
  int           _vibrationFrequency[NUM_CHANNELS]    = {0, 0, 0, 0};
  VibratorType  _vibratorType[NUM_CHANNELS]          = {motorVibrator, motorVibrator, motorVibrator, motorVibrator};
  int _pwmFrequency;
  
};

#endif
