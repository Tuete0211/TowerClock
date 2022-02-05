/**
   Rheinturm Implementierung

   @author Fiete Hiersig

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

#include "ButtonHandler.h"
#include <DS3231.h>
#include <FastLED.h>

#define VERSION 2.1     // This Script is created for Version 2 and above. Since v2 and v2.1 have slightly different wiring it's defined here.

//pin config
#define LED_PIN 3
#define BRIGHTNESS_PIN A2
#define MODE_PIN 2
// led config
#define NUM_LEDS 66
#define NUM_LEDS_CLOCK 55

#define NUM_MODES 6

ButtonHandler modeButton;
RTClib RTC;
DS3231 RTCHardware;
CRGB leds[NUM_LEDS];

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
  CRGB White = CRGB(180, 180, 180);
} colors;

//Times
int DSTOffset = 0;
int currentDate = 111;
int currentHour = 111;
int currentMin = 111;
int currentSec = 111;

// Modes
enum Mode {
  Time = 0,
  Date,
  Temperature,
  Idle,
  Rainbow,
  Stripes,
  Lesson,
  SerialConnection
};
Mode currentMode = Mode::Time;
bool isSetClock = false;

char serialBuffer[20];


//               //
//  ### LOOP ### //
//               //
void loop() {
  delay(2);
  serialLoop();
  //invoke ButtonHandler
  modeButton.loop();
  updateMode();
  delay(2);
  switch (currentMode) {
    case Mode::Time: {                 // display time
        printTowerLight();
        printTowerTime(colors.White);
        break;
      }
    case Mode::Date: {                 // display date
        printTowerLight();
        printTowerDate(colors.White);
        break;
      }
    case Mode::Temperature: {                 // display temperature
        printTowerLight();
        printTowerTemperature();
        break;
      }
    case Mode::Idle: {                 // display without time
        updateBrightness(); // change brightness depending on brightness of room (sensor)
        printTowerLight();
        break;
      }
    case Mode::Rainbow: {                 // display rainbow
        updateBrightness();
        static float startIndex = 0;
        startIndex = startIndex - 0.5f; /* motion speed */
        displayRunningLights((int)startIndex, false, true);

        printTowerLight();
        break;
      }
    case Mode::Stripes: {                 // display stripes
        updateBrightness();
        static float startIndex = 0;
        startIndex = startIndex + 0.8f; /* motion speed */
        displayRunningLights((int)startIndex, false, false);

        printTowerLight();
        break;
      }
    case Mode::Lesson: {                 // display clock to learn
        updateBrightness();
        FastLED.clear();
        fill_solid(leds, NUM_LEDS_CLOCK, colors.White);
        printTimeStatic();
        printTowerLight();
        break;
      }
    case Mode::SerialConnection: {           // set time via Serial Connection
        //setClockSerial();
        static float startIndex = 0;
        startIndex = startIndex + 0.3f; /* motion speed */
        FillLEDsFromPaletteColors( (int)startIndex, SetupRedAndGreenStripedPalette(), LINEARBLEND);
        FastLED.show();
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
  static bool modeChanged = false;
  switch (modeButton.getButtonState()) {
    case ButtonHandler::ButtonState::Pressed1x:
      // increment mode
      currentMode = Mode(currentMode + 1);
      if (currentMode >= NUM_MODES)
          currentMode = Mode::Time;
      modeChanged = true;
      break;
    case ButtonHandler::ButtonState::Pressed2x:
      //decrement mode
      currentMode = Mode(currentMode - 1);
      if ((int)currentMode < 0)
        currentMode = Mode(NUM_MODES - 1);
      modeChanged = true;
      break;
    case ButtonHandler::ButtonState::Pressed3x:
      // surpise - teaching is amazing
      currentMode = Mode::Lesson;
      modeChanged = true;
      break;
    case ButtonHandler::ButtonState::Pressed4x:
      // setting the clock
      currentMode = Mode::SerialConnection;
      modeChanged = true;
      break;
    case ButtonHandler::ButtonState::PressedLong:
      //reset to standard mode (time)
      currentMode = Mode::Time;
      modeChanged = true;
      break;
  }

  if (modeChanged) {
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
    modeChanged = false;
  }
}


//                              //
//  ### Brightness Watcher ###  //
//                              //
void updateBrightness() { // Read analog sensor to detect brightness
  int sensorWert = analogRead(BRIGHTNESS_PIN);
  int newBrightness;

  if (sensorWert < 30) {
    newBrightness = 1;
  } else if (sensorWert < 150) {
    newBrightness = map(sensorWert, 30, 150, 1, 150);
  } else if (sensorWert < 250) {
    newBrightness = map(sensorWert, 150, 250, 150, 255);
  } else {
    newBrightness = 255;
  }

  FastLED.setBrightness(newBrightness);

  brightness = newBrightness;

  /*
  if (DEBUG) {
    Serial.print("sensorWert: ");
    Serial.print(sensorWert);
    Serial.print(", newBrightness: ");
    Serial.println(brightness);
  }
  */
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
  if (height == 0) return;
  for (int i = pos; i < (pos + height); i++) {
    leds[i] = color;
  }
  delay(1);
  FastLED.show();
}

void FillLEDsFromPaletteColors( uint8_t colorIndex, CRGBPalette16 currentPalette, TBlendType currentBlending)
{
  for ( int i = 0; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, /*brightness*/ 255, currentBlending);
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

CRGBPalette16 SetupRedAndGreenStripedPalette()
{
  CRGBPalette16 currentPalette;
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::Red;
  currentPalette[4] = CRGB::Green;
  currentPalette[8] = CRGB::Red;
  currentPalette[12] = CRGB::Green;
  return currentPalette;
}

void displayRunningLights(int startIndex, bool brightnessSensitive, bool rainbow){
  if (brightnessSensitive && brightness == 1 || !rainbow)
      FillLEDsFromPaletteColors( (int)startIndex, SetupBlackAndWhiteStripedPalette(), LINEARBLEND);
    else
      FillLEDsFromPaletteColors( (int)startIndex, RainbowStripeColors_p, LINEARBLEND); // RainbowColors_p

   FastLED.show();
}

void clearClock() {
  for (int i = 0; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = CRGB::Black;
  }
}

// print yellow time separators (appear close to full minutes and hours)
void printTimeStatic() {
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
