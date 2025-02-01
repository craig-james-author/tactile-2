This Arduino library turns a Teensy 4.x microcontroller and audio card into
a touch-sensitive .WAV player, capable of taking input from four capacitive
sensors (implemented by Adafruit Gemma-M0 wearable microcontrollers) and
playing up to four simulaneous .WAV tracks.

A simple configuration tool is available; just open it (double-click) with
your browser. It contains detailed instructions and a description of each
feature.

   sketches/tactile-builder.htm

A key feature of this library is that you don't have to write an Arduino
sketch at all. The following trivial sketch is a complete, working version
of a touch-sensitive .WAV player.

    #include <Arduino.h>
    #include "Tactile.h"

    Tactile *t;

    void setup() {
      t = Tactile::setup();
    }

    void loop() {
      t->loop();
    }

The file

   sketches/Touch/Touch.ino

is a "template" and also has a detailed explanation of every option.

---------------------------------------------------------------------------
Copyright (c) 2022, Craig A. James

This file is part of of the "Tactile" library.

Tactile is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License (LGPL) as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

Tactile is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.

You should have received a copy of the LGPL along with Tactile. If not,
see <https://www.gnu.org/licenses/>.
---------------------------------------------------------------------------
