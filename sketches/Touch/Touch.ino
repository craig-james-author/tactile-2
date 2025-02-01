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

  t->setLogLevel(1);

  //----------------------------------------------------------------------
  // VOLUME CONTROL
  //
  // Normal values from 0 (silent) to 100 (full volume). You can go over
  // 100 if needed; for example, if you have quiet (undermodulated)
  // tracks, you could raise this to (for example) 200, which would
  // double the amplitude of your tracks.
  //
  // Note that if you're playing multiple simultaneous tracks (see below),
  // and your tracks are near 100% modulation, you may need to reduce this
  // to avoid oversaturation and distortion. Two tracks at 50% add up to
  // 100% total.

  t->setVolume(100);

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

  t->setTouchReleaseThresholds(85, 60);

  //----------------------------------------------------------------------
  // ALTERNATIVE touch/release threshold: Set the thresholds on a
  // per-sensor basis (the example above sets all of them at once to the
  // same values). Use one method or the other; uncomment these if you need
  // individual settings.

  // t->setTouchReleaseThresholds(1, 85, 60);
  // t->setTouchReleaseThresholds(2, 85, 60);
  // t->setTouchReleaseThresholds(3, 85, 60);
  // t->setTouchReleaseThresholds(4, 85, 60);

  
  //----------------------------------------------------------------------
  // LOOP MODE
  //
  // If true, then a track loops back to the beginning when the end is
  // reached. Default (false) is that the track just stops at the end.

  t->setLoopMode(true);

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
  // FADE-IN/FADE-OUT
  //
  // By default a track starts playing at full volume, and stops
  // immediately when the sensor is released. If you specify a fade-in
  // and/or fade-out time, then the track's volume fades or out for the
  // specified time (in milliseconds, e.g.  1500 is 1.5 seconds).
  
  t->setFadeInTime(0);
  t->setFadeOutTime(0);

  //----------------------------------------------------------------------
  // MULTI-TRACK MODE
  //
  // Setting this to "true" enables multiple simultaneous tracks.  The
  // default "false" means only one track plays at a time. (Note: type
  // the words "true" or "false" without quote markes.)

  t->setMultiTrackMode(true);

  //----------------------------------------------------------------------
  // CONTINUE-TRACK MODE
  //
  // When a track is playing and the sensor is released, then touched
  // again, does the track resume where it left off ("true"), or start from
  // the beginning ("false")?  (Note: type the words "true" or "false"
  // without quote markes.)

  t->setContinueTrackMode(true);

  //----------------------------------------------------------------------
  // INACTIVITY TIMEOUT
  //
  // If you set continue-track mode (above) to "true", then the inactivity
  // timeout specifies an idle time; if that time passes with no activity
  // (no sensors touched), then all tracks are reset and will start playing
  // from the beginning the next time a sensor is touched. Time is in
  // seconds.

  t->setInactivityTimeout(60);

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
  
  t->setPlayRandomTrackMode(false);

  //----------------------------------------------------------------------
  // TOUCH-TO-STOP MODE
  //
  // Normally the sensors operated as touch-play-release-stop. That is, the
  // track plays while the sensor is being touched, and stops when it's no
  // longer being touched. If you set touch-to-stop mode to "true", then it
  // will operate as touch-start-touch-stop; that is, releases are
  // ignored, and the track plays until the sensor is touched again.
  
  t->setTouchToStop(false);

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
  // NOTE: Fade-in and fade-out are ignored in proximity-as-volume mode.

  t->setProximityAsVolumeMode(false);

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

}

// Don't modify this loop() function.
void loop() {
  t->loop();
}
    
