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

#ifndef TactileBasics_h
#define TactileBasics_h 1

// Number of "channels". Each channel corresponds to linked input and
// output devices. E.g. Channel 1 might have a touch sensor input and
// a vibrator output.

#define NUM_CHANNELS 4

// Where does the input come from?

enum InputSource { noInput, touchInput, audioInput };

// Where does the output go?

enum OutputDest { noOutput, audioOutput, vibrationOutput };

// What types of vibrators are supported?
//  motor  -- takes a simple DC voltage
//  linear -- uses AC signal at vibration frequency

enum VibratorType { motorVibrator, linearVibrator };

#endif
