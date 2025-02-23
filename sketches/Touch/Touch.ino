//======================================================================
//
// Examples of all Tactile-project configurable options.
//
//    -----------------------------------------------------------------
//    NOTE: IT IS STRONGLY RECOMMENDED THAT YOU IGNORE THIS FILE. IT IS
//    HERE FOR HISTORICAL REASONS.
//    -----------------------------------------------------------------
//
// Instead of copying and editing this file directly, please use the
// HTML app that came with your software:
//
//     tactile-builder.html
//
// It is an easy-to-use HTML form that guides you through configuring
// an app for the Tactile project, and then creates a sketch like
// this one but tailored to your needs.
//
//======================================================================


#include <Arduino.h>
#include "Tactile.h"

Tactile *t;

void setup() {

  // Don't modify this setup() statement.
  t = Tactile::setup();

  //----------------------------------------------------------------------
  // LOG LEVEL
  //
  // How much is printed on the Arduino Serial Monitor window?
  //   0: nothing
  //   1: normal info
  //   2: verbose (for code development and debugging)

  setLogLevel(1);


  //======================================================================
  // OPTIONS THAT CONTROL THE SENSORS
  //======================================================================

  //----------------------------------------------------------------------
  // MULTI-TRACK MODE
  //
  // Setting this to "true" enables multiple simultaneous audio tracks
  // and/or haptic (vibrator) outputs.  The default "false" means only one
  // track plays at a time. (Note: type the words "true" or "false" without
  // quote markes.)

  t->setMultiTrackMode(true);

  //----------------------------------------------------------------------
  // IGNORE SENSOR
  //
  // If a sensor is not connected, or it's connected but you want your
  // sketch to ignore it, set ignore to "true". An ignored sensor won't
  // report a touch, and it's proximity value will always be zero.

  t->ignoreSensor(1, false);
  t->ignoreSensor(2, false);
  t->ignoreSensor(3, false);
  t->ignoreSensor(4, false);

  //----------------------------------------------------------------------
  // Audio, Haptic, or both?
  //
  // Each channel can control an audio track, a haptic output, or both.

  t->setOutputDestination(1, audioOutput, vibrationOutput);
  t->setOutputDestination(2, audioOutput, vibrationOutput);
  t->setOutputDestination(3, audioOutput);
  t->setOutputDestination(4, vibrationOutput);

  //----------------------------------------------------------------------
  // INACTIVITY TIMEOUT
  //
  // If you set continue-track mode (above) to "true", then the inactivity
  // timeout specifies an idle time; if that time passes with no activity
  // (no sensors touched), then all tracks are reset and will start playing
  // from the beginning the next time a sensor is touched. Time is in
  // seconds. Note that this single option applies to ALL channels.

  t->setInactivityTimeout(60);

  //----------------------------------------------------------------------
  // TOUCH/RELEASE THRESHOLD
  //
  // The first number controls the threshold at which a track starts
  // playing, and the second number controls the threshold at which the
  // track stops playing. The range is from zero (no touch/proximity) to
  // 100 (strong touch).
  //
  // Note that the first (touch) MUST be bigger than the second (release);
  // it doesn't make sense to stop before you've started!
  //
  // For simple touch sensing, (95, 60) is a good choice. For proximity-
  // as-volume (see below), (10, 5) is a good starting place. The first
  // number (Touch) must always be higher than the second number (Release).

  t->setTouchReleaseThresholds(1, 85, 60);
  t->setTouchReleaseThresholds(2, 95, 85);
  t->setTouchReleaseThresholds(3, 85, 60);
  t->setTouchReleaseThresholds(4, 85, 60);

  
  //----------------------------------------------------------------------
  // TOUCH-TO-STOP MODE
  //
  // Normally the sensors operated as touch-play-release-stop. That is, the
  // track plays while the sensor is being touched, and stops when it's no
  // longer being touched. If you set touch-to-stop mode to "true", then it
  // will operate as touch-start-touch-stop; that is, releases are
  // ignored, and the track plays until the sensor is touched again.
  
  t->setTouchToStop(1, false);
  t->setTouchToStop(1, false);
  t->setTouchToStop(1, false);
  t->setTouchToStop(1, false);

  //----------------------------------------------------------------------
  // AVERAGING (SMOOTHING) STRENGTH
  //
  // You probably don't need to modify this. This is the number of sensor
  // readings that are averaged together to get the sensor's value. Higher
  // numbers mean more smoothing, but also mean a slower response
  // time. Lower numbers mean less smoothing (noiser signal) and faster
  // response. Only change this if you have a noisy situation, usually
  // indicated if your audio tracks "stutter" (start and stop rapidly).
  
  t->setAveragingStrength(200);

  //----------------------------------------------------------------------
  // PROXIMITY MULTIPLIER
  //
  // NOTE: THIS FEATURE IS EXPERIMENTAL, and may change or be removed
  // in future versions of the TactileAudio library.
  // 
  // The proximity-multiplier feature can be used to make the sensor
  // more or less sensitive. Each sensor is specified separately.
  // A value greater than 1 increases sensitivity, and less than one
  // decreases it. For example 2.0 will double sensitivity; 0.5 will
  // cut sensitivity in half. NOTE: reducing sensitivity also reduces
  // the total volume you'll ever get, e.g. a multiplier of 0.5 means
  // you'll never get more than 50% volume.
  //
  // Note that the multiplier also affects touch mode and the
  // touch/release threshold.

  t->setProximityMultiplier(1, 1.0);     // 1.0 is no amplification, more increases sensitivity.
  t->setProximityMultiplier(2, 1.0);     // Sensors can be individually amplified or attenuated.
  t->setProximityMultiplier(3, 1.0);
  t->setProximityMultiplier(4, 1.0);


  //======================================================================
  // OPTIONS THAT CONTROL AUDIO OUTPUT
  //======================================================================

  //----------------------------------------------------------------------
  // PROXIMITY-AS-VOLUME MODE
  //
  // When set to "true", the proximity-as-volume mode, is enabled. The
  // track's audio track starts playing as soon as your hand gets near the
  // sensor (rather than requiring direct touch), and the proximity of your hand
  // controls the volume. The closer your hand gets,
  // the louder the volume.
  //
  // NOTE: You should set the touch-release threshold (above) to fairly low
  // numbers for proximity-as-volume mode. A good touch-release threshold
  // is (10,5) for this mode.
  //
  // NOTE: Volume, Fade-in, and fade-out are ignored in proximity-as-volume mode.

  t->useProximityAsVolume(1, false);
  t->useProximityAsVolume(1, false);
  t->useProximityAsVolume(1, false);
  t->useProximityAsVolume(1, false);

  //----------------------------------------------------------------------
  // VOLUME CONTROL
  //
  // Normal values from 0 (silent) to 100 (full volume). You can go over
  // 100 if needed; for example, if you have quiet (undermodulated)
  // tracks, you could raise this to (for example) 200, which would
  // double the amplitude of your tracks.
  //
  // If you're playing multiple simultaneous tracks (see below), and your
  // tracks are near 100% modulation, you may need to reduce this to avoid
  // oversaturation and distortion. Two tracks at 50% add up to 100% total.
  //
  // Note: setVolume() is ignored when "useProximityAsVolume" (above) is true.

  t->setVolume(1, 100);
  t->setVolume(2, 50);
  t->setVolume(4, 80);
  t->setVolume(4, 100);

  //----------------------------------------------------------------------
  // FADE-IN/FADE-OUT
  //
  // By default a track starts playing at full volume, and stops
  // immediately when the sensor is released. If you specify a fade-in
  // and/or fade-out time, then the track's volume fades or out for the
  // specified time (in milliseconds, e.g.  1500 is 1.5 seconds).
  //
  // Note: Fade-in/out are ignored when "useProximityAsVolume" (above) is true.
  
  t->setFadeInTime (1, 0);       // no fade-in/out
  t->setFadeOutTime(1, 0);
  t->setFadeInTime (2, 500);     // 1/2 second fade-in/out
  t->setFadeOutTime(2, 500);
  t->setFadeInTime (3, 1000);    // 1 second fade in, 2 second fade out
  t->setFadeOutTime(3, 2000);
  t->setFadeInTime (4, 0);       // no fade in/out
  t->setFadeOutTime(4, 0);

  //----------------------------------------------------------------------
  // LOOP MODE
  //
  // If true, then a track loops back to the beginning when the end is
  // reached. Default (false) is that the track just stops at the end.

  t->setLoopMode(1, true);
  t->setLoopMode(2, false);
  t->setLoopMode(3, true);
  t->setLoopMode(4, true);

  //----------------------------------------------------------------------
  // CONTINUE-TRACK MODE
  //
  // When a track is playing and the sensor is released, then touched
  // again, does the track resume where it left off ("true"), or start from
  // the beginning ("false")?  (Note: type the words "true" or "false"
  // without quote markes.)

  t->setContinueTrackMode(1, true);
  t->setContinueTrackMode(2, true);
  t->setContinueTrackMode(3, true);
  t->setContinueTrackMode(4, true);

  //----------------------------------------------------------------------
  // RANDOM-TRACK MODE
  //
  // Normally a single track corresponds to each sensor, and is played when
  // the sensor is touched. If random-track mode is enabled ("true"), then
  // a touched sensor will cause a track to be randomly selected from a the
  // corresponding folder on the micro-SD card (which must be named E1, E2,
  // E3, and E4).  Each folder can contain from one to 100 tracks. The
  // selection is random, except that the most-recently-played track is
  // avoided (i.e. the same track won't play twice in a row, unless there's
  // only one track in the folder).
  
  t->useRandomTracks(1, false);
  t->useRandomTracks(2, false);
  t->useRandomTracks(3, false);
  t->useRandomTracks(4, false);


  //======================================================================
  // OPTIONS THAT CONTROL HAPTIC OUTPUT
  //======================================================================

  //----------------------------------------------------------------------
  // VIBRATOR TYPE
  //
  // - Motor vibrators are small motors with an eccentric (out of balance)
  //   weight. They are powered by a DC voltage.
  //
  // - Linear vibrators are like a small speaker, but without the speaker
  //   cone. They usually have a resonant frequency (e.g. 150 Hz) where
  //   they operate best, and should be driven by an AC voltage at or near
  //   that frequency.

  t->setVibratorType(1, linearVibrator);
  t->setVibratorType(2, linearVibrator);
  t->setVibratorType(3, motorVibrator);
  t->setVibratorType(4, motorVibrator);

  //----------------------------------------------------------------------
  // PROXIMITY-AS-INTENSITY MODE
  //
  // When set to "true", the haptic device (vibrator) starts as soon as
  // your hand gets near the sensor (rather than requiring direct touch),
  // and the proximity of your hand controls the intensity of the vibration.
  //
  // NOTE: You should set the touch-release thresholds (above) to fairly low
  // numbers for proximity-as-volume mode. A good touch-release threshold
  // is (10,5) for this mode.
  //
  // NOTE: setVibrationIntensity() is ignored if proximity-as-intensity is enabled.
  // NOTE: You can either specify useProximityAsIntensity or useProximityAsSpeed, not both.

  t->useProximityAsIntensity(1, true);
  t->useProximityAsIntensity(2, true);
  t->useProximityAsIntensity(3, false);
  t->useProximityAsIntensity(4, false);

  //----------------------------------------------------------------------
  // PROXIMITY-AS-SPEED MODE
  //
  // When set to "true", the haptic device (vibrator) starts as soon as
  // your hand gets near the sensor (rather than requiring direct touch),
  // and the proximity of your hand controls the speed of the vibration
  // pattern (see setVibrationEnvelope() and setVibrationEnvelopeFile(),
  // below).
  //
  // The third number to the option is the percent speedup. For example,
  // a value of 100 means 100% speedup (twice as fast), and 500% means
  // a factor of 6 speedup.
  //
  // NOTE: You should set the touch-release thresholds (above) to fairly low
  // numbers for proximity-as-speed mode. A good touch-release threshold
  // is (10,5) for this mode.
  //
  // NOTE: setVibrationIntensity() is ignored if proximity-as-intensity is enabled.
  // NOTE: You can either specify useProximityAsIntensity or useProximityAsSpeed, not both.
  
  t->useProximityAsSpeed(1, false, 0);
  t->useProximityAsSpeed(2, false, 0);
  t->useProximityAsSpeed(3, true, 500);
  t->useProximityAsSpeed(4, true, 500);

  //----------------------------------------------------------------------
  // VIBRATION INTENSITY
  //
  // Controls the strength of the signal sent to the vibrator. Range is
  // 0 (no vibration) to 100 (maximum vibration).
  //
  // Note: this is ignored if useProximityAsIntensity (above) is used.

  t->setVibrationIntensity(1, 100);
  t->setVibrationIntensity(2, 100);
  t->setVibrationIntensity(3, 100);
  t->setVibrationIntensity(4, 100);

  //----------------------------------------------------------------------
  // VIBRATION "ENVELOPE" (OR "SHAPE")
  //
  // Haptic output (vibration) can be continuous, or you can select from
  // several built-in "envelopes":
  //
  //    "continuous"  The vibration goes continuously.
  //
  //    "square"      The vibration goes on-off, repeating continuously
  //                  once per second.
  //
  //    "sawtooth"    The vibration intensity goes linearly from zero to
  //                  100% and back to zero, repeating once per second.
  //
  //    "pulse"       The vibration pulses for 1/10th a second, and repeats
  //                  once per second.
  //
  //    "pulse-fade"  The vibration pulses for about 1/10th second then
  //                  fades to zero, then repeats every second.

  t->setVibrationEnvelope(1, "continuous");
  t->setVibrationEnvelope(2, "square");
  t->setVibrationEnvelope(3, "sawtooth");
  t->setVibrationEnvelope(4, "pulse");

  //----------------------------------------------------------------------
  // CUSTOM VIBRATION ENVELOPE
  //
  // In addition to the above five built-in vibration patterns, you can
  // design your own with the web app "vibration-designer.html" that is
  // included with this software and also available on tactileproject.com.
  // Save your "vibration envelope" in a text file, store it on the
  // micro-SD card on the Teensy device's audio shield, and put the
  // file's name here. Uncomment these options to use a custom file.

  // t->setVibrationEnvelopeFile(1, "myfile.txt");
  // t->setVibrationEnvelopeFile(2, filename);
  // t->setVibrationEnvelopeFile(3, filename);
  // t->setVibrationEnvelopeFile(4, filename);

  //----------------------------------------------------------------------
  // VIBRATION FREQUENCY
  //
  // Linear vibrators (see setVibratorType() above) often have an optimal
  // frequency which you specify here. Typical values are 30 to 400 Hz.
  //
  // NOTE: The built-in and setVibrationFile() envelopes have a
  // frequency in them; this option overrides it.
  //
  // NOTE: This doesn't apply to motorVibrator types.

  t->setVibrationFrequency(1, 100);
  t->setVibrationFrequency(2, 30);
  t->setVibrationFrequency(3, 400);
  t->setVibrationFrequency(4, 200);

  //----------------------------------------------------------------------
  // OVERRIDE VIBRATION DURATION
  //
  // Each vibration envelope (see setVibrationEnvelope() and
  // setVibrationEnvelopeFile(), above) has a built-in duration. This
  // option overrides it and sets a new duration.
  //
  // NOTE: This is ignored if you specify useProximityAsSpeed(), above.

  t->overrideVibrationEnvelopeDuration(1, 1000);
  t->overrideVibrationEnvelopeDuration(2, 1000);
  t->overrideVibrationEnvelopeDuration(3, 500);
  t->overrideVibrationEnvelopeDuration(4, 2000);

  //----------------------------------------------------------------------
  // OVERRIDE VIBRATION REPEATS
  //
  // Vibration envelopes can either be single events or repeat indefinitely.
  // This overrides the envelope's default; true means repeat forever, and
  // false means the vibrator will do the intensity envelope once then stop.

  t->overrideVibrationEnvelopeRepeats(1, false);
  t->overrideVibrationEnvelopeRepeats(2, false);
  t->overrideVibrationEnvelopeRepeats(3, false);
  t->overrideVibrationEnvelopeRepeats(4, false);

}



// Don't modify this loop() function.
void loop() {
  t->loop();
}
    
