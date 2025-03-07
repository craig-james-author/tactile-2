/*----------------------------------------------------------------------
 * Simple test program for Sensors module "proximity" feature.
 * Prints a stream of numbers, one column per track, of what it senses
 * on the four input channels.
 *
 * Note: this is intended to be used with the Arduino IDE's "serial
 * monitor" window, which will graph the proximity values.
 ----------------------------------------------------------------------*/

#include "TeensyUtils.h"
#include "Sensors.h"

int led_cycle   = 0;
Sensors *t;

void setup() {
  TeensyUtils *tu = TeensyUtils::setup();
  t = Sensors::setup(tu);
}

void loop() {
  float sensorValues[NUM_CHANNELS];
  float maxSensorValue = 0.0;
  int   maxSensorNumber = -1;

  for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
    sensorValues[sensorNumber] = t->getProximityPercent(sensorNumber);
    if (sensorValues[sensorNumber] > maxSensorValue) {
      maxSensorValue = sensorValues[sensorNumber];
      maxSensorNumber = sensorNumber;
    }
  }

  int led_percent = int(sensorValues[1]);
  if (led_cycle < led_percent)
    digitalWrite(TC_LED_PIN, HIGH);
  else
    digitalWrite(TC_LED_PIN, LOW);
  led_cycle++;
  if (led_cycle > 99) {
    led_cycle = 0;
    for (int sensorNumber = 0; sensorNumber < NUM_CHANNELS; sensorNumber++) {
      Serial.print("pin");
      Serial.print(sensorNumber);
      Serial.print(":");
      Serial.print(sensorValues[sensorNumber]);
      Serial.print("  ");
    }
    Serial.print("  x:100  max pin:");         // prevents Arduino IDE serial monitor from auto-scaling
    Serial.println(maxSensorNumber);
  }
  delay(1);
}
    
