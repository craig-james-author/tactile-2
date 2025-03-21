
THE TACTILE PROJECT LIBRARY
---------------------------

For an explanation of what the Tactile project is, see

  https://docs.google.com/document/d/1PwvoPVsNonqkqNiFn8hyfcOJ5IofLOvvq3fhWHgsmE4/edit?usp=sharing

When learning about this software, start with Tactile.h. Everything else in
this library is "wrapped" by Tactile.h and Tactile.cpp.

The Tactile class is designed to make Arduino sketches really
simple. Arduino "sketches" (programs) have only two functions:

  setup() -- initialization, called once at the program start
  loop() -- called repeatedly once running

The Tactile module has these same two methods. So the general way to write
an Arduino sketch for the Tactile project is like this:

  #include <Arduino.h>
  #include "Tactile.h"
  Tactile *t;
  void setup() {
    setLogLevel(0);
    t = Tactile::setup();
    // set up a bunch of options, see Tactile.h
    t->ignoreSensor(1, false);
    ... etc.
  }
  void loop() {
    t->loop();
  }

That's it. However, you should never need to do this. There are two HTML
apps that write the sketch for you. See the software distribution page.

Design "standards"

  Most of the error checking (e.g. integer out-of-bounds problems) are
  checked by Tactile.cpp. The lower-level modules (e.g. Vibration.cpp,
  AudioPlayer.cpp, etc.) assume that the parameters passed in have already
  been checked.

  Although this is C++ code, it has a strong C flavor. That is, the code
  uses classes with public/private methods and state variables, but down in
  the actual code, it looks more like regular C. For example, there is no
  use of stdlib things like Vectors or Strings; instead, the library uses
  good old integer[] and char[] arrays. This is to keep things simple and
  more maintainable, but primarily to keep the code light without pulling
  in big libraries.

  All private function and variable names start with an underscore. All
  functions and variables use "studly caps" naming, e.g. getLogLevel().
  Class names are capitalized, e.g. Tactile, Vibrate, etc.


----------------------------------------------------------------------

Copyright (c) 2025, Craig A. James

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
