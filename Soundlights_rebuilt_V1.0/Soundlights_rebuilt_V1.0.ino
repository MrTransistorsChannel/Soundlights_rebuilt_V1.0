#include <FastLED.h>

#define NUM_LEDS 240
#define DATA_PIN 3

//#define WITH_STROBO

#define LIN_OUT8 1 // use the log output function
#define SCALE   255
#define FHT_N   256 // set to 256 point fht

#include <FHT.h> // include the library

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

CRGB leds[NUM_LEDS];

#include "MODES.h"

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  randomSeed(analogRead(A1));
  analogReference(EXTERNAL);
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);
}

void loop() {

  analyzeSound();
#ifdef WITH_STROBO
  if (VU < 511 || MODE == 2) {

    switch (MODE) {
      case 0:
        generateRandomColor();
        VUMeter(CHSV(VUcolor, 255, 255));
        break;
      case 1:
        fullSizeSpectrum();
        break;
      case 2:
        peakFlow();
        break;
      case 3:
        threeBarSpectrum();
        break;
      case 4:
        fiveBarSpectrum();
        break;
      case 5:
        rainbowVU();
        break;
    }

    modeChangeHandler();



    FastLED.show();
  }
  else flowingStrobo();
#else
  switch (MODE) {
    case 0:
      generateRandomColor();
      VUMeter(CHSV(VUcolor, 255, 255));
      break;
    case 1:
      fullSizeSpectrum();
      break;
    case 2:
      peakFlow();
      break;
    case 3:
      threeBarSpectrum();
      break;
    case 4:
      fiveBarSpectrum();
      break;
    case 5:
      rainbowVU();
      break;
  }

  modeChangeHandler();
#endif

  FastLED.show();


}

void modeChangeHandler() {
  if (millis() - modeTime > timeForMode) {
    byte curMode = MODE;
    while (MODE == curMode) MODE = random(1000) % 6;
    Serial.println(MODE);
    timeForMode = random(4000, 10000);
    modeTime = millis();
  }
}

void generateRandomColor() {
  if (millis() - colorTime > timeForColor) {
    VUcolor = random(255);
    timeForColor = random(2500, 10000);
    colorTime = millis();
  }
}
