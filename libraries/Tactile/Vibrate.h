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

// This module controls the vibrators: mode (VIBRATE, PULSE), volume,
// and timing.

class Vibrate
{
 public:


  // Constructors
  Vibrate(TeensyUtils* tc);
  static Vibrate* setup(TeensyUtils* tc);

  // channels are 0..N-1
  void vibrate   (int channel, int frequency);
  void pulse     (int channel, int frequency, int pulses, int period);
  void stop      (int channel);
  bool isPlaying (int channel);
  void setVolume (int channel, int percent);
  void setVolume (int percent);

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

  static int _convertChannelToPins(int channel);
  static int _convertChannelToPin1(int channel);
  static int _convertChannelToPin2(int channel);

  // Convert frequency to period in msec
  int _convertFrequencyToMsec(int frequency);

  // What is this channel doing?
  VibModes _vibMode[NUM_CHANNELS] = {noVibrate, noVibrate, noVibrate, noVibrate};
  
  // Vibration data. A vibrater switches its pair of outputs' polarity
  // every N milliseconds.
  int           _period[NUM_CHANNELS];
  unsigned long _startTime[NUM_CHANNELS];       // when this cycle started
  boolean       _currentState[NUM_CHANNELS];    // true = high, false = low

  // Pulse data. Pulsing is controlled by three parameters:
  //    frequency - the frequence of the pulse while it's on
  //    width     - how wide the pulse (on period) is
  //    period    - how often to send a pulse
  // A pulse is the same as vibration, but for a very brief time,
  // so pulses use the vibration parameters, above, plus these:

  int _pulseWidth[NUM_CHANNELS];
  int _pulsePeriod[NUM_CHANNELS];
  int _pulseStartTime[NUM_CHANNELS];

  // Volume (PWM, Pulse-width modulation) data.
  // 255 == full volume, 0 == off
  int _currentVolume[NUM_CHANNELS];

  int _pwmFrequency;
  
};

#endif
