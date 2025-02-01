/* -*-C-*-
+======================================================================
| Copyright (c) 2022, Craig A. James
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

#include "Arduino.h"
#include "TeensyUtils.h"

TeensyUtils* TeensyUtils::setup() {
  TeensyUtils* tcpu = new(TeensyUtils);
  //  pinMode(TC_LED_PIN, OUTPUT);    // declare the TC_LED_PIN as an OUTPUT:
  Serial.begin(57600);
  tcpu->_logLevel = 1;
  return tcpu;
}

/*----------------------------------------------------------------------
 * Simple utility functions
 ----------------------------------------------------------------------*/

void TeensyUtils::setLogLevel(int level) {
  _logLevel = level;
}

int TeensyUtils::getLogLevel() {
  return _logLevel;
}

void TeensyUtils::log(const char *msg) {
  if (_logLevel < 1) return;
  if (msg == NULL)
    Serial.println("TeensyUtils::log: NULL message!");
  else
    Serial.println(msg);
}

void TeensyUtils::logAction(const char *msg, int track) {
  if (_logLevel < 1) return;
  if (msg == NULL)
    Serial.println("TeensyUtils::logAction: NULL message!");
  else {
    Serial.print(msg);
    Serial.println(track);
  }
}

void TeensyUtils::log2(const char *msg) {
  if (_logLevel < 2) return;
  if (msg == NULL)
    Serial.println("TeensyUtils::log2: NULL message!");
  else
    Serial.println(msg);
}

void TeensyUtils::logAction2(const char *msg, int track) {
  if (_logLevel < 2) return;
  if (msg == NULL)
    Serial.println("TeensyUtils::logAction2: NULL message!");
  else {
    Serial.print(msg);
    Serial.println(track);
  }
}

void TeensyUtils::turnLedOn() {
  //  digitalWrite(TC_LED_PIN, HIGH);
}

void TeensyUtils::turnLedOff() {
  //  digitalWrite(TC_LED_PIN, LOW);
}

void TeensyUtils::sleep(int milliseconds) {
  delay(milliseconds);
}
