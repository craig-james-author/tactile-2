/*----------------------------------------------------------------------
 * Simple test of Audio module's pause/resume feature.
----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TeensyUtils.h"
#include "Sensors.h"
#include "AudioPlayer.h"

int n = 0;
TeensyUtils *tc;
AudioPlayer *ta;

void setup() {
  tc = TeensyUtils::setup();
  ta = AudioPlayer::setup(tc);
}

void loop() {

  n++;
  switch (n) {
  case 1: ta->startTrack(0);  Serial.println("start 1");  break;
  case 2: ta->startTrack(1);  Serial.println("start 2");  break;
  case 3: ta->pauseTrack(0);  Serial.println("pause 1");  break;
  case 4: ta->pauseTrack(1);  Serial.println("pause 2");  break;
  case 5: ta->resumeTrack(0); Serial.println("resume 1"); break;
  case 6: ta->resumeTrack(1); Serial.println("resume 2"); break;
  case 7: ta->stopTrack(0);   Serial.println("stop 1");   break;
  case 8: ta->stopTrack(1);   Serial.println("stop 2");   break;
  default: n = 0;
  }
  Serial.print("isPlaying: ");
  Serial.print(ta->isPlaying(0));
  Serial.print(" ");
  Serial.println(ta->isPlaying(1));
  Serial.print("isPaused: ");
  Serial.print(ta->isPaused(0));
  Serial.print(" ");
  Serial.println(ta->isPaused(1));
                 
  delay(2000);
}
