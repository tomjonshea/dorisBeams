// Animations for the D.O.R.I.S. Burning Man art car, 2016

/* TODO:
    - test downgrading all unnecessary 'uint8_t's to uintx_t has worked
    - build circular functions
    - incorporate palettes
    - add pattern selector control
    - fix tab/space conversion standard
    - add serial output debug for potentiometers and microphone
*/

#include "FastLED.h"

// Options
#define BRIGHTNESS_POTENTIOMETER_ON 0
#define BRIGHTNESS_POTENTIOMETER_PIN 18
#define BRIGHTNESS_POTENTIOMETER_MAX 1023 //test

#define PATTERN_POTENTIOMETER_ON 0
#define PATTERN_POTENTIOMETER_PIN  19
#define PATTERN_POTENTIOMETER_MAX 1023 //test

#define MICROPHONE_ON  0
#define MICROPHONE_PIN  20
#define MICROPHONE_MAX  1023 //test
#define MICROPHONE_THRESHOLD  900 //test

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120
#define RAINBOW_PHASE  7
#define PATTERN_CHANGE_PERIOD_SECONDS  10

// Installation dimensions
#define PIXELS_PER_STRIP  75
#define STRIPS_PER_BEAM  2
#define PIXELS_PER_BEAM  150
#define NUMBER_OF_BEAMS  17

// LED hardware parameters
#define CHIPSET WS2812B
#define COLOR_ORDER GRB

// Instantiate the array of colour values
CRGB leds[NUMBER_OF_BEAMS][PIXELS_PER_BEAM];

// Required initial setup method
void setup() {
  delay(1000);

  FastLED.addLeds<CHIPSET, 0, COLOR_ORDER>(leds[0],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 1, COLOR_ORDER>(leds[1],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 2, COLOR_ORDER>(leds[2],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 3, COLOR_ORDER>(leds[3],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 4, COLOR_ORDER>(leds[4],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 5, COLOR_ORDER>(leds[5],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 6, COLOR_ORDER>(leds[6],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 7, COLOR_ORDER>(leds[7],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 8, COLOR_ORDER>(leds[8],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 9, COLOR_ORDER>(leds[9],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 10, COLOR_ORDER>(leds[10],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 11, COLOR_ORDER>(leds[11],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 12, COLOR_ORDER>(leds[12],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 13, COLOR_ORDER>(leds[13],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 14, COLOR_ORDER>(leds[14],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 15, COLOR_ORDER>(leds[15],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, 16, COLOR_ORDER>(leds[16],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { testSweeper, moodLighting, beamMeUpGreen, circularRainbow, rainbow, rainbowWithGlitter, glitterOnly, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // Rotating "base color" used by many of the patterns

void loop()
{
    // Modulate brightness based on potentiometer value, if this contol is enabled
    if (BRIGHTNESS_POTENTIOMETER_ON) {
        int brightnessPotentiometerValue = analogRead(BRIGHTNESS_POTENTIOMETER_PIN);
        uint8_t newBrightness = map(brightnessPotentiometerValue, 0, BRIGHTNESS_POTENTIOMETER_MAX, 0, 255);
        FastLED.setBrightness(newBrightness);
    }

    // Modulate brightness based on microphone value, if this contol is enabled
    // Alternatively this could be set up to be a binary on/off based on the microphone exceeding a threshold value
    if (MICROPHONE_ON) {
        int microphoneValue = analogRead(MICROPHONE_PIN);
        uint8_t newBrightness = map(microphoneValue, 0, MICROPHONE_MAX, 0, 255);
        FastLED.setBrightness(newBrightness);
    }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(PATTERN_CHANGE_PERIOD_SECONDS) { nextPattern(); } // change patterns periodically
}

// UTILITY FUNCTIONS
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void copyFirstStripToSecond(uint8_t beamId)
{
    for (uint8_t i = 0; i < PIXELS_PER_STRIP; i++) {
        leds[beamId][PIXELS_PER_BEAM - i] = leds[beamId][i];
    }
}

void copyBeamToAllBeams(uint8_t beamId)
{
    /*
    Fallback option:
    for (int i = 0; i < NUMBER_OF_BEAMS; i++) {
        if (i != beamId) {
            leds[i] = leds[beamId];
        }
    }
    */

    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        if (i != beamId) {
            memmove(&leds[i], &leds[beamId], 10 * sizeof(CRGB));
        }
    }
}

void copyFirstStripToAllBeams(uint8_t beamId)
{
    copyFirstStripToSecond(beamId);
    copyBeamToAllBeams(beamId);
}

// ANIMATION FUNCTIONS
void rainbow()
{
    uint8_t firstBeam = 0;

  // FastLED's built-in rainbow generator on the first strip of each beam
  fill_rainbow(leds[firstBeam], PIXELS_PER_STRIP, gHue, RAINBOW_PHASE);
  copyFirstStripToAllBeams(firstBeam);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void glitterOnly()
{
    FastLED.clear();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        if (random8() < chanceOfGlitter) {
          leds[i][random16(PIXELS_PER_BEAM)] += CRGB::White;
        }
    }
}

void confetti()
{
    uint8_t firstBeam = 0;
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds[firstBeam], PIXELS_PER_BEAM, 10);
  uint8_t pos = random16(PIXELS_PER_BEAM);
  leds[firstBeam][pos] += CHSV(gHue + random8(64), 200, 255);
  copyBeamToAllBeams(firstBeam);
}

void sinelon()
{
    uint8_t firstBeam = 0;
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds[firstBeam], PIXELS_PER_STRIP, 20);
  uint8_t pos = beatsin16(13, 0, PIXELS_PER_STRIP);
  leds[firstBeam][pos] += CHSV(gHue, 255, 192);
  copyFirstStripToAllBeams(firstBeam);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;

  CRGBPalette16 palette = PartyColors_p;

  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);

  uint8_t firstBeam = 0;
  for (uint8_t i = 0; i < PIXELS_PER_STRIP; i++) { //9948
    leds[firstBeam][i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  copyFirstStripToAllBeams(firstBeam);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  uint8_t firstBeam = 0;
  fadeToBlackBy(leds[firstBeam], PIXELS_PER_STRIP, 20);
  byte dothue = 0;
  for (uint8_t i = 0; i < 8; i++) {
    leds[firstBeam][beatsin16(i+7 , 0, PIXELS_PER_STRIP)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  copyFirstStripToAllBeams(firstBeam);
}

void fillSolidRed() {
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, CRGB::Red);
    }
}

void fillSolidBlue() {
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, CRGB::Blue);
    }
}

void fillSolidWhite() {
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, CRGB::White);
    }
}

void moodLighting() {
    // Mimics as 40W Tungsten bulb
    CRGB tungsten = CRGB(255, 197, 143);
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, tungsten);
    }
    FastLED.setBrightness(64);
}

void testSweeper() {
    // Sweeps though all LEDs in array to show their positioning from the perspective
    // of the program, and to test their color accuracy. Should be red, then green, then blue.
    FastLED.clear();
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        for (uint8_t j = 0; j < PIXELS_PER_BEAM; j++) {
            leds[i][j] = CRGB::Red;
            FastLED.show();
        }
    }
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        for (uint8_t j = 0; j < PIXELS_PER_BEAM; j++) {
            leds[i][j] = CRGB::Green;
            FastLED.show();
        }
    }
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        for (uint8_t j = 0; j < PIXELS_PER_BEAM; j++) {
            leds[i][j] = CRGB::Blue;
            FastLED.show();
        }
    }
}

void beamMeUp(CRGB warpColour) {
    uint8_t firstBeam = 0;
    for (uint8_t i = 0; i < PIXELS_PER_STRIP; i++) {
        FastLED.clear();
        leds[firstBeam][i] = warpColour;
    }
    copyFirstStripToAllBeams(firstBeam);
    FastLED.show();
}

void beamMeUpGreen() {
    beamMeUp(CRGB::Green);
}

void circularRainbow(){
    CRGB rainbowArray[NUMBER_OF_BEAMS];
    fill_rainbow(rainbowArray, NUMBER_OF_BEAMS, gHue, RAINBOW_PHASE);
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, rainbowArray[i]);
    }
}
