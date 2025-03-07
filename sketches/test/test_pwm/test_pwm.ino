/*----------------------------------------------------------------------
 * Test sketch for learning about pulse-width modulation
 ----------------------------------------------------------------------*/

#include <Arduino.h>
#include "TeensyUtils.h"
#include "AudioFileManager.h"

TeensyUtils *tc;
AudioFileManager *fm;

#define LED_PIN 13

void setup() {
  Serial.begin(57600);
  Serial.println("Serial initialized");
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  analogWriteFrequency(0, 100000);
  analogWriteFrequency(1, 100000);
  pinMode(33, OUTPUT);  // for triggering oscilloscope
}

static int pwmValue = 0;
static boolean state = false;
static int period = 5;

static elapsedMillis elapsedMSec;
static unsigned long start = elapsedMSec;
static unsigned long startVolumeTime = start;

void loop() {
  int cycleTime = elapsedMSec - start;
  if (cycleTime <= period/2) {
    if (state) {
      state = false;
      digitalWrite(LED_PIN, LOW);
      analogWrite(0, 128 - pwmValue);
      analogWrite(1, 128 + pwmValue);
      digitalWrite(33, HIGH);
    }
  } else if (cycleTime <= period) {
    if (!state) {
      state = true;
      digitalWrite(LED_PIN, HIGH);
      analogWrite(0, 128 + pwmValue);
      analogWrite(1, 128 - pwmValue);
      digitalWrite(33, LOW);
    }
  } else {
    start = elapsedMSec;

    // every 1/100th second, increase volume
    int volumeTime = elapsedMSec - startVolumeTime;
    if (volumeTime > 10) {
      startVolumeTime = elapsedMSec;
      pwmValue += 1;
      if (pwmValue >= 128)
        pwmValue = 0;
    }
  }
 

}
