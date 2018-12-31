byte MODE, VUcolor, thisHue;

int VU;

uint64_t colorTime, modeTime, hueTime;
uint16_t timeForColor, timeForMode;

void analyzeSound() {
  VU = 0;
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
    int k = analogRead(A0);
    k -= 512; // form into a signed int
    VU = max(k, VU);
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins
  }
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  fht_mag_lin8(); // take the output of the fht
  sei();
}

void fullSizeSpectrum() {
  for (byte i = 0 ; i < NUM_LEDS / 2 ; i++) {
    leds[i] = CHSV(i * (FHT_N / NUM_LEDS) * 2, 255, fht_lin_out8[i * (FHT_N / NUM_LEDS)] * 8); // send out the data
  }
  for (byte i = NUM_LEDS / 2; i < NUM_LEDS; i++) {
    leds[i] = CHSV((NUM_LEDS - (i * (FHT_N / NUM_LEDS))) * 2, 255, fht_lin_out8[NUM_LEDS - (i * (FHT_N / NUM_LEDS))] * 8); // send out the data
  }
}

/*void VUMeter(CRGB color) {
  int val = constrain(map(VU, 0, 500, 0, NUM_LEDS / 2), 0, NUM_LEDS / 2);
  for (int i = NUM_LEDS / 2; i < (NUM_LEDS / 2) + val; i++) {
    leds[i] = color;
  }

  for (int i = (NUM_LEDS / 2) - val; i < NUM_LEDS / 2; i++) {
    leds[i] = color;
  }


  for (int i = val + (NUM_LEDS / 2); i < NUM_LEDS; i++) {
    leds[i] = 0;
  }
  for (int i = 0; i < (NUM_LEDS / 2) - val; i++) {
    leds[i] = 0;
  }
}*/

void VUMeter(CRGB color) {
  int val = constrain(map(VU, 0, 500, 0, NUM_LEDS), 0, NUM_LEDS);
  for (int i = 0; i < val; i++) {
    leds[i] = color;
  }

  for (int i = val; i < NUM_LEDS; i++) {
    leds[i] = 0;
  }
}

void peakFlow() {
  delay(5);
  for (int i = NUM_LEDS - 2; i >= 0; i--) {
    leds[i + 1] = leds[i];
  }
  if (VU > 500) leds[0] = CRGB::White;
  else leds[0] = 0;
}

void threeBarSpectrum() {
  for (byte i = 0; i < NUM_LEDS / 3; i++) {
    leds[i] = CHSV(0, 255, fht_lin_out8[0] * 4); // send out the data
  }
  for (byte i = NUM_LEDS / 3; i < (NUM_LEDS / 3) * 2; i++) {
    leds[i] = CHSV(85, 255, fht_lin_out8[FHT_N / 6] * 5); // send out the data
  }
  for (byte i = (NUM_LEDS / 3) * 2; i < NUM_LEDS; i++) {
    leds[i] = CHSV(171, 255, fht_lin_out8[(FHT_N / 6) * 2] * 6); // send out the data
  }

}

void fiveBarSpectrum() {
  for (byte i = 0; i < NUM_LEDS / 5; i++) {
    leds[i] = CHSV(160, 255, fht_lin_out8[(FHT_N / 6) * 2] * 6); // send out the data
  }
  for (byte i = NUM_LEDS / 5; i < (NUM_LEDS / 5) * 2; i++) {
    leds[i] = CHSV(96, 255, fht_lin_out8[FHT_N / 6] * 5); // send out the data
  }
  for (byte i = (NUM_LEDS / 5) * 2; i < (NUM_LEDS / 5) * 3; i++) {
    leds[i] = CHSV(0, 255, fht_lin_out8[0] * 4); // send out the data
  }
  for (byte i = (NUM_LEDS / 5) * 3; i < (NUM_LEDS / 5) * 4; i++) {
    leds[i] = CHSV(96, 255, fht_lin_out8[FHT_N / 6] * 5); // send out the data
  }
  for (byte i = (NUM_LEDS / 5) * 4; i < NUM_LEDS; i++) {
    leds[i] = CHSV(160, 255, fht_lin_out8[(FHT_N / 6) * 2] * 6); // send out the data
  }
}

/*void rainbowVU() {
  int val = constrain(map(VU, 0, 500, 0, NUM_LEDS / 2), 0, NUM_LEDS / 2);

  fill_rainbow(leds, NUM_LEDS, thisHue);
  if (millis() - hueTime > 5) thisHue++;

  for (int i = val + (NUM_LEDS / 2); i < NUM_LEDS; i++) {
    leds[i] = 0;
  }
  for (int i = 0; i < (NUM_LEDS / 2) - val; i++) {
    leds[i] = 0;
  }
}*/

void rainbowVU() {
  int val = constrain(map(VU, 0, 500, 0, NUM_LEDS), 0, NUM_LEDS);

  fill_rainbow(leds, NUM_LEDS, thisHue);
  thisHue++;

  for (int i = val; i < NUM_LEDS; i++) {
    leds[i] = 0;
  }
}

#ifdef WITH_STROBO
void flowingStrobo() {
  for (int i = NUM_LEDS / 2; i < NUM_LEDS; i += 2) {
    leds[i] = CRGB::White;
    leds[NUM_LEDS - i] = CRGB::White;
    //delayMicroseconds(300);
    FastLED.show();
  }
  for (int i = NUM_LEDS / 2; i < NUM_LEDS; i += 2) {
    leds[i] = 0;
    leds[NUM_LEDS - i] = 0;
    //delayMicroseconds(300);
    FastLED.show();
  }
}
#endif
