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

#ifndef Sensors_h
#define Sensors_h 1

#include "TeensyUtils.h"

// Touches to the electrodes
#define IS_TOUCHED 1
#define IS_RELEASED 0
#define TOUCH_NO_CHANGE 0
#define NEW_TOUCH 1
#define NEW_RELEASE 2


class Sensors
{
 public:

  // Constructors
  Sensors(TeensyUtils* tc);
  static Sensors* setup(TeensyUtils* tc);

  // Touch and proximity sensing
  void  setTouchReleaseThresholds(float touchThreshold, float releaseThreshold);
  void  setTouchReleaseThresholds(int channel, float touchThreshold, float releaseThreshold);
  void  ignoreSensor(int channel, bool ignore);
  void  setTouchToggleMode(int channel, bool on);
  int   getTouchStatus(float proximityValues[], int sensorStatus[], int sensorChanges[]);
  float getProximityPercent(int channel);
  void  setAveragingStrength(int samples);
  void  setProximityMultiplier(int channel, float m);

 private:

  const int _channelToPinNumber[NUM_CHANNELS] = {A14, A15, A16, A17};

  TeensyUtils *_tu;
  
  // General controls
  bool _touchToggleMode;        // touch-on-touch-off rather than touch-on-release-off

  // Touch sensors
  int   _lastSensorTouched;
  float _touchThreshold[NUM_CHANNELS];          // Percent, 0..100
  float _releaseThreshold[NUM_CHANNELS];
  bool  _ignoreSensor[NUM_CHANNELS];
  int   _lastSensorStatus[NUM_CHANNELS];
  int   _lastSensorPseudoStatus[NUM_CHANNELS];    // for touchToggleMode only
  unsigned long _lastActionTime[NUM_CHANNELS];
  float _proximityMultiplier[NUM_CHANNELS];

  // Proximity detection and smoothing
  float _filteredSensorValue[NUM_CHANNELS];
  int   _averagingSamples;

  int _checkSensorRange(int channel);

};

#endif
