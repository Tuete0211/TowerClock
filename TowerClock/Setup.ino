//                //
//  ### SETUP ### //
//                //
void setup() {
  delay(1000); // power-up safety delay

  // Open Serial to be able to connect from computer
  Serial.begin(9600);

  Serial.println("Welcome to Tower Clock - Serial Interface");
  Serial.println("Please use the corresponding application to make changes in the configuration");

  // ButtonHandler
  modeButton = ButtonHandler(MODE_PIN);
  
  // RTC
  Wire.begin();
  //if (DEBUG)printTime();

  //FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();

  // start LED action
  updateBrightness();
  welcomeProgram();

  // check for daylight saving time (DTS)
  DateTime now = RTC.now();
  checkForDST(now);

  //Serial.println("Setup finished");
}


//                              //
//  ### StartUp LED action ###  //
//                              //
void welcomeProgram() {
  FastLED.clear();
  delay(20);
  
  // FIRST: white dot wandering up the tower
  for (int i = 0; i < 22; i++) {
    FastLED.clear();
    leds[i] = colors.White;
    FastLED.show();
    delay(50);
  }
  FastLED.clear();
  leds[22] = colors.White;
  if (VERSION == 2) {
    leds[64] = colors.White;
    leds[63] = colors.White;
  } else if (VERSION == 2.1) {
    leds[64] = colors.White;
    leds[65] = colors.White;
  }
  FastLED.show();
  delay(50);
  for (int i = 23; i < 39; i++) {
    FastLED.clear();
    leds[i] = colors.White;
    FastLED.show();
    delay(50);
  }
  FastLED.clear();
  leds[39] = colors.White;
  if (VERSION == 2) {
    leds[65] = colors.White;
    leds[62] = colors.White;
  } else if (VERSION == 2.1) {
    leds[62] = colors.White;
    leds[63] = colors.White;
  }
  FastLED.show();
  delay(50);
  for (int i = 40; i < 55; i++) {
    FastLED.clear();
    leds[i] = colors.White;
    FastLED.show();
    delay(50);
  }
  FastLED.clear();
  leds[55] = colors.White;
  leds[61] = colors.White;
  FastLED.show();
  delay(80);
  leds[56] = colors.White;
  leds[60] = colors.White;
  FastLED.show();
  delay(110);
  leds[57] = colors.White;
  leds[59] = colors.White;
  FastLED.show();
  delay(150);
  leds[58] = colors.White;
  FastLED.show();
  delay(150);
  for (int i = 0; i < 100; i++) {
    fadeToBlackBy(leds, NUM_LEDS, 10);
    FastLED.show();
    delay(15);
  }
  FastLED.clear();
  
  // SECOND: wandering lila and red dots to fill tower lights
  for (int i = 0; i < NUM_LEDS_CLOCK; i++) {
    FastLED.clear();
    leds[i] = colors.UpperDeck;
    if (i - 1 > 0) leds[i - 1] = colors.UpperDeck;
    if (i - 2 > 0) leds[i - 2] = colors.UpperDeck;
    if (i - 3 > 0) leds[i - 3] = colors.UpperDeck;
    if (i < 39 + 4) {
      if (i - 4 > 0) leds[i - 4] = colors.Red;
    } else {
      if (VERSION == 2) {
        leds[62] = colors.Red;
        leds[65] = colors.Red;
      } else if (VERSION == 2.1) {
        leds[62] = colors.Red;
        leds[63] = colors.Red;
      }
    }
    if (i < 22 + 5) {
      if (i - 5 > 0) leds[i - 5] = colors.Red;
    } else {
      if (VERSION == 2) {
        leds[63] = colors.Red;
        leds[64] = colors.Red;
      } else if (VERSION == 2.1) {
        leds[64] = colors.Red;
        leds[65] = colors.Red;
      }
    }
    FastLED.show();
    delay(50);
  }
  FastLED.clear();
  leds[55] = colors.UpperDeck;
  leds[61] = colors.UpperDeck;
  leds[54] = colors.UpperDeck;
  leds[53] = colors.UpperDeck;
  leds[52] = colors.UpperDeck;
  leds[62] = colors.Red;
  leds[65] = colors.Red;
  leds[63] = colors.Red;
  leds[64] = colors.Red;
  FastLED.show();
  delay(80);

  FastLED.clear();
  leds[56] = colors.UpperDeck;
  leds[60] = colors.UpperDeck;
  leds[55] = colors.UpperDeck;
  leds[61] = colors.UpperDeck;
  leds[54] = colors.UpperDeck;
  leds[53] = colors.UpperDeck;
  leds[62] = colors.Red;
  leds[65] = colors.Red;
  leds[63] = colors.Red;
  leds[64] = colors.Red;
  FastLED.show();
  delay(120);

  FastLED.clear();
  leds[57] = colors.UpperDeck;
  leds[59] = colors.UpperDeck;
  leds[56] = colors.UpperDeck;
  leds[60] = colors.UpperDeck;
  leds[55] = colors.UpperDeck;
  leds[61] = colors.UpperDeck;
  leds[54] = colors.UpperDeck;
  leds[62] = colors.Red;
  leds[65] = colors.Red;
  leds[63] = colors.Red;
  leds[64] = colors.Red;
  FastLED.show();
  delay(160);

  FastLED.clear();
  leds[58] = colors.UpperDeck;
  leds[57] = colors.UpperDeck;
  leds[59] = colors.UpperDeck;
  leds[56] = colors.UpperDeck;
  leds[60] = colors.UpperDeck;
  leds[55] = colors.UpperDeck;
  leds[61] = colors.UpperDeck;
  leds[62] = colors.Red;
  leds[65] = colors.Red;
  leds[63] = colors.Red;
  leds[64] = colors.Red;
  FastLED.show();
  delay(500);
}
