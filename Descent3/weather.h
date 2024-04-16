#ifndef WEATHER_H
#define WEATHER_H

#define WEATHER_FLAGS_RAIN 1
#define WEATHER_FLAGS_LIGHTNING 2
#define WEATHER_FLAGS_SNOW 4

#define MAX_RAIN_INTENSITY 50
#define MAX_SNOW_INTENSITY 200

typedef struct {
  int flags; // see weather flags, above

  float snow_intensity_scalar; // how hard it is snowing
  float rain_intensity_scalar; // how hard its raining
  int rain_color;              // the color of the rain
  int lightning_color;         // the color of the lightning
  int sky_flash_color;         // the color of the sky when lightning occurs

  ubyte lightning_sequence;

  float last_lightning_evaluation_time;
  float lightning_interval_time;
  int lightning_rand_value;

  int snowflakes_to_create;

} weather;

extern weather Weather;

// resets the weather so there is nothing happening
void ResetWeather();

// Makes droplets appear on the windshield, plus makes rain fall in the distance
void DoRainEffect();

// does all the weather stuff that is going to be done for this frame
void DoWeatherForFrame();

// Sets the state of the rain to on or off, plus sets the intensity of the rain (0 to 1)
void SetRainState(int on, float intensity);

// Sets the state of lightning to on or off, plus allows the setting of how often to check
// for lightning and the randomness at which lightning happens
void SetLightningState(int on, float interval_time, int randval);

// Sets the state of the snow to on or off, plus sets the intensity of the snow (0 to 1)
void SetSnowState(int on, float intensity);

#endif
