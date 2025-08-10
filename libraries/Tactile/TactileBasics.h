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

// How should audio tracks be selected?
// Each sensor can have either a single track or a directory
// of tracks associated with it.
//  playSingle      -- play the single track
//  playRandom      -- play a random track from the associated directory
//  playShuffled    -- shuffle the tracks from the associated directory and
//                     play them in shuffled order; when the end of the list
//                     is reached, go back and play it again
//  playReshuffled  -- like playShuffled, but each time the end of thelist is
//                     reached, re-shuffle and start again

enum playTrackActionType {playSingle, playRandom, playShuffled, playReshuffled};

#endif
