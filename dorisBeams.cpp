// Animations for the D.O.R.I.S. Burning Man art car, 2016

#include "FastLED.h"

// Options
#define BRIGHTNESS_POTENTIOMETER_ON 1
#define BRIGHTNESS_POTENTIOMETER_PIN 18
#define BRIGHTNESS_POTENTIOMETER_MAX 1023

#define PATTERN_POTENTIOMETER_ON 1
#define PATTERN_POTENTIOMETER_PIN  19
#define PATTERN_POTENTIOMETER_MAX 1023

// #define MICROPHONE_ON  0
// #define MICROPHONE_PIN  20
// #define MICROPHONE_MAX  1023 //test
// #define MICROPHONE_THRESHOLD  900 //test

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120 //test
#define RAINBOW_PHASE  7 //probably good, but test
#define PATTERN_CHANGE_PERIOD_SECONDS  90

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

    // This cannot be iterated due to the addLeds function requiring constants as args.
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
    FastLED.addLeds<CHIPSET, 14, COLOR_ORDER>(leds[14],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<CHIPSET, 15, COLOR_ORDER>(leds[15],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<CHIPSET, 16, COLOR_ORDER>(leds[16],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<CHIPSET, 17, COLOR_ORDER>(leds[17],  PIXELS_PER_BEAM).setCorrection(TypicalLEDStrip);

    FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { testSweeper, juggle, circularRainbow, sinelon, rainbow, rainbowWithGlitter, glitterOnly, confetti, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // Rotating "base color" used by many of the patterns

// Required main loop
void loop() {
    FastLED.setBrightness(BRIGHTNESS);

    // Modulate brightness based on potentiometer value, if this contol is enabled
    if (BRIGHTNESS_POTENTIOMETER_ON) {
        int brightnessPotentiometerValue = analogRead(BRIGHTNESS_POTENTIOMETER_PIN);
        uint8_t newBrightness = map(brightnessPotentiometerValue, 0, BRIGHTNESS_POTENTIOMETER_MAX, 0, 255);
        FastLED.setBrightness(newBrightness);
    }

    // Swap between mood lighting and disco patterns
    if (PATTERN_POTENTIOMETER_ON) {
        int patternPotentiometerValue = analogRead(PATTERN_POTENTIOMETER_PIN);
        uint8_t patternSelector = map(patternPotentiometerValue, 0, PATTERN_POTENTIOMETER_MAX, 0, 255);
        if (patternSelector > 128) {
            gPatterns[gCurrentPatternNumber]();
        } else {
            moodLighting();
        }
    } else {
        gPatterns[gCurrentPatternNumber]();
    }

    // Modulate brightness based on microphone value, if this contol is enabled
    // Alternatively this could be set up to be a binary on/off based on the microphone exceeding a threshold value
    // if (MICROPHONE_ON) {
    //     int microphoneValue = analogRead(MICROPHONE_PIN);
    //     uint8_t newBrightness = map(microphoneValue, 0, MICROPHONE_MAX, 0, 255);
    //     FastLED.setBrightness(newBrightness);
    // }

    // Call the current pattern function once, updating the 'leds' array
    //gPatterns[gCurrentPatternNumber]();

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

void nextPattern() {
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void copyFirstStripToSecond(uint8_t beamId) {
    for (uint8_t i = 0; i < PIXELS_PER_STRIP; i++) {
        leds[beamId][PIXELS_PER_BEAM - i - 1] = leds[beamId][i];
    }
}

void copyBeamToAllBeams(uint8_t beamId) {
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
            memmove(&leds[i], &leds[beamId], PIXELS_PER_BEAM * sizeof(CRGB));
        }
    }
}

void copyFirstStripToAllBeams(uint8_t beamId) {
    copyFirstStripToSecond(beamId);
    copyBeamToAllBeams(beamId);
}

// ANIMATION FUNCTIONS
void rainbow() {
    uint8_t firstBeam = 0;
    FastLED.clear();

    // FastLED's built-in rainbow generator on the first strip of each beam
    fill_rainbow(leds[firstBeam], PIXELS_PER_STRIP, gHue, RAINBOW_PHASE);
    copyFirstStripToAllBeams(firstBeam);
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void glitterOnly() {
    FastLED.clear();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        if (random8() < chanceOfGlitter) {
          leds[i][random16(PIXELS_PER_BEAM)] += CRGB::White;
        }
    }
}

void confetti() {
    uint8_t firstBeam = 0;
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds[firstBeam], PIXELS_PER_BEAM, 10);
    uint8_t pos = random16(PIXELS_PER_BEAM);
    leds[firstBeam][pos] += CHSV(gHue + random8(64), 200, 255);
    copyBeamToAllBeams(firstBeam);
}

void sinelon() {
    uint8_t firstBeam = 0;
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds[firstBeam], PIXELS_PER_STRIP, 20);
    uint8_t pos = beatsin16(13, 0, PIXELS_PER_STRIP);
    leds[firstBeam][pos] += CHSV(gHue, 255, 192);
    copyFirstStripToAllBeams(firstBeam);
}

void bpm() {
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
    // Caution! May test limits of power supplies...
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
    FastLED.setBrightness(64); //migth not be necessary
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

// void beamMeUp(CRGB warpColour, uint8_t warpRings) {
//     FastLED.clear();
//     uint8_t ringModulator = warpRings/PIXELS_PER_STRIP;
//     uint8_t firstBeam = 0;
//     uint8_t ringHeight = map(gHue, 0, 255, 0, PIXELS_PER_STRIP);
//     for (uint8_t i = 0; i < PIXELS_PER_STRIP + 1; i++) {
//         if (i >= ringHeight && (i - ringHeight) % ringModulator == 0) {
//             if (i < PIXELS_PER_STRIP) {
//                 leds[firstBeam][i] = warpColour;
//             }
//             if (i >= 1) {
//                 leds[firstBeam][i - 1] = warpColour;
//             }
//         }
//     }
//     copyFirstStripToAllBeams(firstBeam);
// }
//
// void beamMeDown(CRGB warpColour, uint8_t warpRings) {
//     FastLED.clear();
//     uint8_t ringModulator = warpRings/PIXELS_PER_STRIP;
//     uint8_t firstBeam = 0;
//     uint8_t ringHeight = map(gHue, 0, 255, 0, PIXELS_PER_STRIP);
//     for (uint8_t i = 0; i < PIXELS_PER_STRIP + 1; i++) {
//         if (i >= ringHeight && (i - ringHeight) % ringModulator == 0) {
//             if (i < PIXELS_PER_STRIP) {
//                 leds[firstBeam][PIXELS_PER_STRIP - i] = warpColour;
//             }
//             if (i > 1) {
//                 leds[firstBeam][PIXELS_PER_STRIP - i + 1] = warpColour;
//             }
//         }
//     }
//     copyFirstStripToAllBeams(firstBeam);
// }
//
// void beamMeUpGreen() {
//     beamMeUp(CRGB::Green, 3);
// }
//
// void beamMeDownOrange() {
//     beamMeUp(CRGB::Orange, 5);
// }
//
// // Dubious math here - test thoroughly
// void spiralMeUp(CRGB warpColour, uint8_t warpRings) {
//     FastLED.clear();
//     uint8_t ringModulator = warpRings/PIXELS_PER_STRIP;
//     uint8_t ringHeight = map(gHue, 0, 255, 0, PIXELS_PER_STRIP);
//     for (uint8_t j = 0; j < NUMBER_OF_BEAMS; j++) {
//         for (uint8_t i = 0; i < PIXELS_PER_STRIP + 1; i++) {
//             uint8_t position = i + j;
//             if (position >= ringHeight && ((position - ringHeight) % ringModulator == 0 || (position - ringHeight - NUMBER_OF_BEAMS) % ringModulator == 0)) {
//                 if (position < PIXELS_PER_STRIP) {
//                     leds[j][position] = warpColour;
//                 }
//                 if (position >= 1) {
//                     leds[j][position - 1] = warpColour;
//                 }
//             }
//         }
//         copyFirstStripToSecond(j);
//     }
// }
//
// void spiralMeDown(CRGB warpColour, uint8_t warpRings) {
//     FastLED.clear();
//     uint8_t ringModulator = warpRings/PIXELS_PER_STRIP;
//     uint8_t ringHeight = map(gHue, 0, 255, 0, PIXELS_PER_STRIP);
//     for (uint8_t j = 0; j < NUMBER_OF_BEAMS; j++) {
//         for (uint8_t i = 0; i < PIXELS_PER_STRIP + 1; i++) {
//             uint8_t position = i + j;
//             if (position >= ringHeight && ((position - ringHeight) % ringModulator == 0 || (position - ringHeight - NUMBER_OF_BEAMS) % ringModulator == 0)) {
//                 if (position < PIXELS_PER_STRIP) {
//                     leds[j][PIXELS_PER_STRIP - position] = warpColour;
//                 }
//                 if (position > 1) {
//                     leds[j][PIXELS_PER_STRIP - position - 1] = warpColour;
//                 }
//             }
//         }
//         copyFirstStripToSecond(j);
//     }
// }
//
// void spiralMeUpBlue() {
//     spiralMeUp(CRGB::Blue, 3);
// }
//
// void spiralMeDownYellow() {
//     spiralMeDown(CRGB::Yellow, 10);
// }

void circularRainbow() {
    CRGB rainbowArray[NUMBER_OF_BEAMS];
    fill_rainbow(rainbowArray, NUMBER_OF_BEAMS, gHue, RAINBOW_PHASE);
    for (uint8_t i = 0; i < NUMBER_OF_BEAMS; i++) {
        fill_solid(leds[i], PIXELS_PER_BEAM, rainbowArray[i]);
    }
}
