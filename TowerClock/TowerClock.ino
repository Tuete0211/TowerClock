/**
   Rheinturm Implementierung

   By: Fiete Hiersig
   
   Der Rheinturm Düsseldorf herbergt die größte dezimale Zeitskala entlang des Turmschaftes.
   Diese Uhr besteht im originalen aus 62 Bulaugen mit starken RGB-LEDs. Diese LEDs ermöglichen die Anzeige der Zeit.
   Für die Umsetztung dieses Projektes wird benötigt:
   - Arduino Nano
   - DS3231 RealTimeClock
   - Druckknopf
   - LED-Streifen
   - 5V-Stromversorgung

   Used Libraries:
   - DS3231   -> https://github.com/NorthernWidget/DS3231
   - FastLED  -> https://github.com/FastLED/FastLED
*/

#include <DS3231.h>
#include <FastLED.h>
#include "ButtonHandler.h"

#define VERSION 2.1     // This Script is created for Version 2 and above. Since v2 and v2.1 have slightly different wiring it's defined here.

#define DEBUG 0
#define INFO 0
#define NUM_LEDS 66
#define NUM_LEDS_CLOCK 55
#define NUM_MODES 6
#define LED_PIN 3
#define BRIGHTNESS_PIN A2
#define MODE_PIN 2

RTClib RTC;
DS3231 RTCHardware;
CRGB leds[NUM_LEDS];
ButtonHandler modeButton;

//colors
float brightness = 127; // 0-255
float displayedBrightness = 127;
struct CustomColors {
  CRGB Red = CRGB(255, 0, 0);
  CRGB Green = CRGB(0, 255, 0);
  CRGB Blue = CRGB(0, 0, 255);
  CRGB Yellow = CRGB(255, 255, 0);
  CRGB Cyan = CRGB(0, 255, 255);
  CRGB Magenta = CRGB(255, 0, 255);
  CRGB UpperDeck = CRGB(15, 0, 100);    //*/CRGB(20, 0, 200);
  CRGB Black = CRGB(0, 0, 0);
  CRGB White = CRGB(255, 255, 255);
} colors;

//Times
int currentDate = 111;
int currentHour = 111;
int currentMin = 111;
int currentSec = 111;
int DSTOffset = 0;

// Modes
int currentMode = 0;


//                //
//  ### SETUP ### //
//                //
void setup() {
  delay(2000); // power-up safety delay
  if (DEBUG || INFO) {
    Serial.begin(9600);
    Serial.println("Initializing...");
  }

  // RTC
  Wire.begin();
  if (DEBUG)printTime();

  //FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();

  // ButtonHandler
  modeButton = ButtonHandler(MODE_PIN);

  // start LED action
  updateBrightness();
  welcomeProgram();

  // reset LED-Strip to black       --> not needed, since the welcomeprogramm stops with the static lights on
  //FastLED.clear(); // <--> fill_solid( leds, NUM_LEDS, CRGB::Red);

  if (DEBUG) Serial.println("Setup finished");
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
    delay(60);
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
  delay(60);
  for (int i = 23; i < 39; i++) {
    FastLED.clear();
    leds[i] = colors.White;
    FastLED.show();
    delay(60);
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
  delay(60);
  for (int i = 40; i < 55; i++) {
    FastLED.clear();
    leds[i] = colors.White;
    FastLED.show();
    delay(60);
  }
  FastLED.clear();
  leds[55] = colors.White;
  leds[61] = colors.White;
  FastLED.show();
  delay(100);
  leds[56] = colors.White;
  leds[60] = colors.White;
  FastLED.show();
  delay(120);
  leds[57] = colors.White;
  leds[59] = colors.White;
  FastLED.show();
  delay(150);
  leds[58] = colors.White;
  FastLED.show();
  delay(150);
  for(int i = 0; i < 100; i++) {
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
    delay(80);
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
  delay(100);

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
  delay(150);

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
  delay(200);

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
  delay(1000);
}

//               //
//  ### LOOP ### //
//               //
void loop() {
  delay(2);
  modeButton.loop();
  updateMode();
  delay(2);
  switch (currentMode) {
    case 0: {
        printTowerLight();
        printTowerTime(colors.White);
        break;
      }
    case 1: {
        printTowerLight();
        printTowerDate(colors.White);
        break;
      }
    case 2: {
        printTowerLight();
        printTowerTemperature();
        break;
      }
    case 3: {
        updateBrightness(); // change brightness depending on brightness of room (sensor)
        printTowerLight();
        break;
      }
    case 4: {
        updateBrightness();
        //static uint8_t startIndex = 0;
        static float startIndex = 0;
        startIndex = startIndex - 0.5f; /* motion speed */

        FillLEDsFromPaletteColors( (int)startIndex, RainbowStripeColors_p, LINEARBLEND); // RainbowColors_p

        FastLED.show();

        printTowerLight();
        break;
      }
    case 5: {
        updateBrightness();
        static float startIndex = 0;
        startIndex = startIndex + 0.8f; /* motion speed */

        FillLEDsFromPaletteColors( (int)startIndex, SetupBlackAndWhiteStripedPalette(), LINEARBLEND);

        FastLED.show();

        printTowerLight();
        break;
      }
    case 6: {
        updateBrightness();
        FastLED.clear();
        fill_solid(leds, NUM_LEDS_CLOCK, colors.White);
        printTimeStatic();
        printTowerLight();
        break;
      }
    default: {
        fill_solid( leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
      }
  }
}

//                        //
//  ### Mode Switcher ### //
//                        //
void updateMode() {
  bool modeChanged = false;
  if (modeButton.getPressed1x()) {
    // increment mode
    currentMode++;
    if (currentMode >= NUM_MODES) {
      currentMode = 0;
    }
    modeChanged = true;
  }
  else if (modeButton.getPressed2x()) {
    //decrement mode
    currentMode--;
    if (currentMode < 0) {
      currentMode = NUM_MODES - 1;
    }
    modeChanged = true;
  }
  else if (modeButton.getPressed3x()) {
    // surpise - teaching is amazing
    currentMode = 6;
    modeChanged = true;
  }
  else if (modeButton.getLongPressed()) {
    //reset to standard mode
    currentMode = 0;
    modeChanged = true;
  }
  if (modeChanged) {
    if (INFO)Serial.println(currentMode);
    FastLED.clear();
    delay(500);
    // indicate mode number
    leds[3] = colors.Green;
    Line(currentMode + 1, 4, colors.White);
    delay(2000);
    FastLED.clear();

    // reset times
    currentDate = 111;
    currentHour = 111;
    currentMin = 111;
    currentSec = 111;
  }
  /*
    if (DEBUG) {
      Serial.print("sensorWert: ");
      Serial.print(sensorWert);
      Serial.print(", ");
      Serial.println(currentMode);
    }
  */
}


//                              //
//  ### Brightness Watcher ###  //
//                              //
void updateBrightness() { // Read analog sensor to detect brightness
  int sensorWert = analogRead(BRIGHTNESS_PIN);
  int newBrightness;

  if (sensorWert < 30) {
    newBrightness = 1;
  } else if (sensorWert < 200) {
    newBrightness = map(sensorWert, 30, 200, 1, 150);
  } else if (sensorWert < 300) {
    newBrightness = map(sensorWert, 200, 300, 150, 255);
  } else {
    newBrightness = 255;
  }

  FastLED.setBrightness(newBrightness);

  brightness = newBrightness;

  if (DEBUG) {
    Serial.print("sensorWert: ");
    Serial.print(sensorWert);
    Serial.print(", newBrightness: ");
    Serial.println(brightness);
  }
}


//                //
//  ### Time ###  //
//                //
void printTowerTime(CRGB color) {
  DateTime now = RTC.now();

  int hour = now.hour() + DSTOffset;
  int minutes = now.minute();
  int seconds = now.second();

  // Rainbow on full and half hour
  if ((minutes == 59 || minutes == 29) && seconds >= 0 && seconds < 45 && brightness) {
    if (INFO)Serial.println("Rainbow");
    // start Rainbow
    //static uint8_t startIndex = 0;
    static float startIndex = 0;
    startIndex = startIndex - 0.5f; /* motion speed */

    FillLEDsFromPaletteColors( (int)startIndex, RainbowStripeColors_p, LINEARBLEND); // RainbowColors_p

    FastLED.show();
    printTowerLight();
    updateBrightness(); // change brightness depending on brightness of room (sensor)
    currentMin = 111;
    return;
  }

  if (currentMin != minutes) { // update hour and minute every minute
    clearClock();
    updateBrightness(); // change brightness depending on brightness of room (sensor)
    checkForDST(now);
    int tenHour = hour / 10;
    Line(2, 50, colors.Black);
    Line(tenHour, 50, color);
    Line(9, 40, colors.Black);
    Line((hour % 10), 40, color);
    currentHour = hour;

    int tenMin = minutes / 10;
    Line(5, 33, colors.Black);
    Line(tenMin, 33, color);
    Line(9, 23, colors.Black);
    Line((minutes % 10), 23, color);
    currentMin = minutes;
    if (INFO)Serial.println("Minutes");
  }

  if (currentSec != seconds) {
    int curSec = currentSec / 10;
    int tenSec = seconds / 10;
    if (curSec != tenSec) {
      Line(5, 16, colors.Black);
      Line(tenSec, 16, color);
    }
    Line(9, 6, colors.Black);
    Line((seconds % 10), 6, color);
    currentSec = seconds;
    if (INFO)Serial.println("Seconds");
  }

  if ((minutes == 59 || minutes == 29) && seconds >= 45) {
    printTimeStatic();
  }

  if (seconds == 59) {
    printTimeStatic();
  }

  if (INFO)printDateTime(now);
}

// automatic DST detection (summer- / wintertime
void checkForDST(DateTime now) {
  if (now.month() >= 4 && now.month() <= 9 && DSTOffset == 0) {
    // summertime
    DSTOffset = 1;
  }
  else if (now.month() == 3 && now.day() >= 25 && DSTOffset == 0) {
    switch (RTCHardware.getDoW()) {
      case 7:
        if (now.hour() >= 2)
          DSTOffset = 1;
        else
          DSTOffset = 0;
        break;
      case 1:
        if (now.day() >= 26)
          DSTOffset = 1;
        break;
      case 2:
        if (now.day() >= 27)
          DSTOffset = 1;
        break;
      case 3:
        if (now.day() >= 28)
          DSTOffset = 1;
        break;
      case 4:
        if (now.day() >= 29)
          DSTOffset = 1;
        break;
      case 5:
        if (now.day() >= 30)
          DSTOffset = 1;
        break;
      case 6:
        if (now.day() >= 31)
          DSTOffset = 1;
        break;
    }
  }
  else if (now.month() >= 3 && now.month() >= 11 && DSTOffset == 1) {
    // normal time (winter)
    DSTOffset = 0;
  }
  else if (now.month() == 10 && now.day() >= 25 && DSTOffset == 1) {
    switch (RTCHardware.getDoW()) {
      case 7:
        if (now.hour() >= 2)
          DSTOffset = 0;
        else
          DSTOffset = 1;
        break;
      case 1:
        if (now.day() >= 26)
          DSTOffset = 0;
        break;
      case 2:
        if (now.day() >= 27)
          DSTOffset = 0;
        break;
      case 3:
        if (now.day() >= 28)
          DSTOffset = 0;
        break;
      case 4:
        if (now.day() >= 29)
          DSTOffset = 0;
        break;
      case 5:
        if (now.day() >= 30)
          DSTOffset = 0;
        break;
      case 6:
        if (now.day() >= 31)
          DSTOffset = 0;
        break;
    }
  }
}

//                //
//  ### Date ###  //
//                //
void printTowerDate(CRGB color) {
  DateTime now = RTC.now();

  int minutes = now.minute();
  int day = now.day();
  int month = now.month();
  int year = now.year() - 2000;

  if (currentMin != minutes) {
    updateBrightness();
    currentMin = minutes;
  }

  if (currentDate != day || int(displayedBrightness * 10) != int(brightness * 10) ) {
    int tenDay = day / 10;
    //Line(4, 45, colors.Black);
    Line(tenDay, 45, color);
    //Line(9, 35, colors.Black);
    Line((day % 10), 35, color);
    currentDate = day;

    int tenMonth = month / 10;
    //Line(2, 32, colors.Black);
    Line(tenMonth, 32, color);
    //Line(9, 22, colors.Black);
    Line((month % 10), 22, color);

    int tenYear = year / 10;
    //Line(9, 11, colors.Black);
    Line(tenYear, 11, color);
    //Line(9, 1, colors.Black);
    Line((year % 10), 1, color);

    leds[21] = colors.Blue;
    leds[34] = colors.Blue;
    delay(2);
    FastLED.show();

    displayedBrightness = brightness;
  }

  if (INFO)printDateTime(now);
}

//                      //
//  ### Temperatur ###  //
//                      //
void printTowerTemperature() {
  DateTime now = RTC.now();
  int minutes = now.minute();
  if (currentMin != minutes) {
    updateBrightness();
    currentMin = minutes;
  }

  float currentTemp = RTCHardware.getTemperature();
  int positionTemp = map(currentTemp, 15 , 30, 0, NUM_LEDS_CLOCK);
  fill_gradient_RGB(leds, NUM_LEDS_CLOCK, CRGB::DarkBlue, CRGB::DeepSkyBlue, CRGB::DarkOrange, CRGB::DarkRed);
  leds[positionTemp] = CRGB::Black;
  if (leds[positionTemp - 1]) {
    leds[positionTemp - 1].fadeToBlackBy(192);
  }
  if (leds[positionTemp + 1]) {
    leds[positionTemp + 1].fadeToBlackBy(192);
  }
}


//                            //
//  ### Drawing Functions ### //
//                            //
void Line(int height, int pos, CRGB color) {
  if (INFO && DEBUG) {
    Serial.print(height);
    Serial.print(", ");
    Serial.print(pos);
    Serial.print(", ");
  }
  if (height == 0) return;
  for (int i = pos; i < (pos + height); i++) {
    leds[i] = color;
  }

  delay(2);
  FastLED.show();
  if (INFO)Serial.println("Draw");
}

void FillLEDsFromPaletteColors( uint8_t colorIndex, CRGBPalette16 currentPalette, TBlendType currentBlending)
{
  uint8_t brightness = 255;

  for ( int i = 0; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

CRGBPalette16 SetupBlackAndWhiteStripedPalette()
{
  CRGBPalette16 currentPalette;
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
  return currentPalette;
}

void towerRainbow(int wait) {
  int stopTime = millis() + wait;

  while (stopTime > millis()) {
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex, RainbowStripeColors_p, LINEARBLEND); // RainbowColors_p

    FastLED.show();
    printTowerLight();
    delay(20);
  }
}

void clearClock() {
  for (int i = 0; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = CRGB::Black;
  }
}

// print yellow time separators (appear close to full minutes and hours)
void printTimeStatic() {
  if (INFO)Serial.println("Time-Static");

  for (int i = 0; i <= 5; i++) {
    leds[i] = colors.Yellow;
  }
  leds[15] = colors.Yellow;
  leds[21] = colors.Yellow;
  leds[22] = colors.Yellow;
  leds[32] = colors.Yellow;
  leds[38] = colors.Yellow;
  leds[39] = colors.Yellow;
  leds[49] = colors.Yellow;

  for (int i = 52; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = colors.Yellow;
  }

  delay(2);
  FastLED.show();
}

// show red guide lights and illuminated upper deck (lila)
void printTowerLight() {
  if (INFO)Serial.println("Tower Lighting");

  // red guiding lights
  for (int i = 62; i < NUM_LEDS; i++) {
    leds[i] = colors.Red;
  }
  //blue-lila upper-deck
  for (int i = 55; i < 62; i++) {
    leds[i] = colors.UpperDeck;
  }
  delay(2);
  FastLED.show();
}


//                      //
//  ### DEBUG Stuff ### //
//                      //
void printTime() {

  DateTime now = RTC.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print("  -   ");
  Serial.print(now.unixtime());
  Serial.print("  -  DayofWeek: ");
  Serial.print(RTCHardware.getDoW());
  Serial.print("  -  Temp: ");
  Serial.print(RTCHardware.getTemperature());
  Serial.println();
}

void printDateTime(DateTime now) {

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print("  -   ");
  Serial.print(now.unixtime());
  Serial.println();
}
