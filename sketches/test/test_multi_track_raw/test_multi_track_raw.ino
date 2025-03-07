/*----------------------------------------------------------------------
 * This sketch tests the Teensy + Audio Board "raw", that is, without
 * using the Audio library file. It will confirm that the
 * hardware is working.
----------------------------------------------------------------------*/

//#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

class foo : public AudioPlaySdWav {
  int foo;
};

AudioPlaySdWav           playSdWav1;     //xy=124,100
AudioPlaySdWav           playSdWav2;     //xy=124,160
AudioPlaySdWav           playSdWav3;     //xy=124,220
AudioPlaySdWav           playSdWav4;     //xy=124,280
AudioMixer4              mixer1;         //xy=470,160
AudioMixer4              mixer2;         //xy=470,280
AudioOutputI2S           i2s1;           //xy=650,220
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord3(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord4(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord5(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord6(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord7(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord8(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord9(mixer1, 0, i2s1, 0);
AudioConnection          patchCord10(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000;     //xy=127,379.111083984375

void setup() {

  // Start the SD card. These definitions are specific to the
  // Teensy Audio Shield when mounted on the Teensy 4.1 computer.
#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN  13
#define SDCARD_CS_PIN   10
  Serial.println("AudioFileManager: Initializing SD card...");
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println("SD card initialization failed!");  // always print, even of logging turned off.
    while (1);
  }
  
  // Initialization for the Teensy Audio Shield
   AudioMemory(16);
  sgtl5000.enable();
  sgtl5000.volume(0.3);
  delay(1000);  // wait for SGTL5000 to initialize

  for (int i = 0; i < 4; i++) {
    mixer1.gain(i, 1.0);
    mixer2.gain(i, 1.0);
  }
  Serial.println("setup() done.\n");
}

void loop() {

  Serial.println("play 1");
  playSdWav1.play("SDTEST1.WAV");
  delay(2000);
  Serial.println(AudioMemoryUsageMax());

  Serial.println("play 2");
  playSdWav2.play("SDTEST2.WAV");
  delay(2000);
  Serial.println(AudioMemoryUsageMax());

  Serial.println("play 3");
  playSdWav3.play("SDTEST4.WAV");
  delay(2000);
  Serial.println(AudioMemoryUsageMax());

  Serial.println("play 4");
  playSdWav4.play("SDTEST4.WAV");
  delay(2000);
  Serial.println(AudioMemoryUsageMax());


  Serial.println("stop 1");
  playSdWav1.stop();
  delay(2000);

  Serial.println("stop 2");
  playSdWav2.stop();
  delay(2000);

  Serial.println("stop 3");
  playSdWav3.stop();
  delay(2000);

  Serial.println("stop 4");
  playSdWav4.stop();
  delay(2000);
}
