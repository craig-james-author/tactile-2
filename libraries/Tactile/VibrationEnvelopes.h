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

#ifndef VibrationEnvelopes_h
#define VibrationEnvelopes_h 1

#include "Vibrate.h"

static VibrationEnvelope builtInEnvelopes[] = {
  
  {{"continuous"}, 0, 0, 0, true,
   {100, -1}
  },

  { {"square"}, 2, 500, 1000, true,
    {100, 0, -1}
  },

  { {"sawtooth"}, 50, 20, 1000, true,
    {100, 96, 92, 88, 84, 80, 76, 72,68, 64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0,
     4, 8, 12, 16,20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, -1}
  },
  
  { {"pulse"}, 10, 100, 1000, true,
    {100, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1}
  },

  { {"pulse-fade"}, 44, 50, 2200, true,
    {78, 100, 100, 100, 100, 100, 96, 89, 84, 78, 73, 71, 66, 61, 58,
     54, 49, 46, 42, 40, 38, 36, 34, 32, 30, 28, 26, 23, 21, 19, 18,
     16, 14, 12, 11, 10, 9, 7, 5, 4, 3, 2, 1, 0, -1}
  },

  {{""},0,0,0,false,{-1}}
};

#endif
