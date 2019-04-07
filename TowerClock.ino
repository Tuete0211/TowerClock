/**
   Rheinturm Implementierung
   Der Rheinturm Düsseldorf herbergt die größte dezimale Zeitskala entlang des Turmschaftes.
   Diese Uhr besteht im originalen aus 62 Bulaugen mit starken RGB-LEDs. Diese LEDs ermöglichen die Anzeige der Zeit.
   Für die Umsetztung dieses Projektes wird benötigt:
   - Arduino Nano
   - DS3231 RealTimeClock
   - Druckknopf
   - LED-Streifen
   - 5V-Stromversorgung
*/

#include <DS3231.h>
#include <FastLED.h>

#define DEBUG 1
#define INFO 0
#define NUM_LEDS 64
#define NUM_LEDS_CLOCK 50
#define NUM_MODES 6
#define LED_PIN 3
#define BRIGHTNESS_PIN A2
#define MODE_PIN A0

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
  CRGB UpperDeck = CRGB(20, 0, 200);
  CRGB Black = CRGB(0, 0, 0);
  CRGB White = CRGB(255, 255, 255);
} colors;

//Times
int currentDate = 111;
int currentHour = 111;
int currentMin = 111;
int currentSec = 111;

// Modes
int changeModeCounter = 0;
int currentMode = 0;


void setup() {
  delay(3000); // power-up safety delay
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Initializing...");
  }

  // RTC
  Wire.begin();
  if (DEBUG)printTime();

  //FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  updateBrightness();

  welcomeProgram();

  // reset LED-Strip to black
  FastLED.clear(); // <--> fill_solid( leds, NUM_LEDS, CRGB::Red);

  if (DEBUG) Serial.println("Setup finished");
}

void loop() {
  delay(2);
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
        printTowerLight();
        break;
      }
    case 4: {
        updateBrightness();
        static uint8_t startIndex = 0;
        startIndex = startIndex - 2; /* motion speed */

        FillLEDsFromPaletteColors( startIndex, RainbowStripeColors_p, LINEARBLEND);

        FastLED.show();
        break;
      }
    case 5: {
        updateBrightness();
        static uint8_t startIndex = 0;
        startIndex = startIndex + 1; /* motion speed */

        FillLEDsFromPaletteColors( startIndex, SetupBlackAndWhiteStripedPalette(), LINEARBLEND);

        FastLED.show();
        break;
      }
    default: {
        fill_solid( leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
      }
  }
}

void printTowerTemperature() {
  DateTime now = RTC.now();
  int minutes = now.minute();
  if (currentMin != minutes) {
    updateBrightness();
    currentMin = minutes;
  }
  
  float currentTemp = RTCHardware.getTemperature();
  int positionTemp = map(currentTemp, 0 , 35, 0, NUM_LEDS_CLOCK);
  fill_gradient_RGB(leds, NUM_LEDS_CLOCK, CRGB::DarkBlue, CRGB::DeepSkyBlue, CRGB::DarkOrange, CRGB::DarkRed);
  leds[positionTemp] = CRGB::Black;
  if(leds[positionTemp-1]) {
    leds[positionTemp-1].fadeToBlackBy(192);
  }
  if(leds[positionTemp+1]) {
    leds[positionTemp+1].fadeToBlackBy(192);
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex, CRGBPalette16 currentPalette, TBlendType currentBlending)
{
  uint8_t brightness = 255;

  for ( int i = 0; i < NUM_LEDS; i++) {
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

void welcomeProgram() { // start-up demo program
  FastLED.clear();
  delay(200);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    delay(20);
    FastLED.show();
  }
  delay(100);
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    leds[i] = CRGB::Green;
    delay(20);
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS / 10; i++) {
    leds[i] = CRGB::Blue;
    leds[10 + i] = CRGB::Blue;
    leds[20 + i] = CRGB::Blue;
    leds[30 + i] = CRGB::Blue;
    leds[40 + i] = CRGB::Blue;
    leds[50 + i] = CRGB::Blue;
    if (60 + i < NUM_LEDS) leds[60 + i] = CRGB::Blue; // Avoid mistakes with missing LEDs
    if (70 + i < NUM_LEDS) leds[70 + i] = CRGB::Blue; // Avoid mistakes with missing LEDs
    delay(60);
    FastLED.show();
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS / 2; i++) {
    leds[i] = colors.White;
    leds[NUM_LEDS - i - 1] = colors.White;
    delay(20);
    FastLED.show();
  }
  delay(200);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(((i * 2) & 255), 255, 255);
    delay(40);
    FastLED.show();
  }
  delay(500);
  FastLED.clear();
}

void updateMode() {
  int sensorWert = digitalRead(MODE_PIN);
  if (sensorWert == 1) {
    changeModeCounter++;
  } else {
    changeModeCounter = 0;
  }
  if (changeModeCounter > 20) {
    int newMode = currentMode + 1;
    if (currentMode >= NUM_MODES) {
      newMode = 0;
    }
    Serial.println(newMode);
    FastLED.clear();
    delay(1000);
    // indicate mode number
    leds[3] = colors.Green;
    Line(newMode + 1, 4, colors.White);
    delay(3500);
    FastLED.clear();
    currentMode = newMode;

    // reset values
    // Times
    currentDate = 111;
    currentHour = 111;
    currentMin = 111;
    currentSec = 111;

    // Modes
    changeModeCounter = 0;
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

void printTowerTime(CRGB color) {
  DateTime now = RTC.now();

  int hour = now.hour();
  int minutes = now.minute();
  int seconds = now.second();

  // Rainbow on full hour
  if ((minutes == 59 || minutes == 29) && seconds >= 0 && seconds < 45 && brightness) {
    if (INFO)Serial.println("Rainbow");
    // start Rainbow /* NYI */
    currentMin = 111;
    return;
  }

  if (currentMin != minutes) { // update hour and minute every minute
    FastLED.clear();
    updateBrightness(); // change brightness depending on brightness of room (sensor)
    int tenHour = hour / 10;
    Line(2, 45, colors.Black);
    Line(tenHour, 45, color);
    Line(9, 35, colors.Black);
    Line((hour % 10), 35, color);
    currentHour = hour;

    int tenMin = minutes / 10;
    Line(5, 28, colors.Black);
    Line(tenMin, 28, color);
    Line(9, 18, colors.Black);
    Line((minutes % 10), 18, color);
    currentMin = minutes;
    if (INFO)Serial.println("Minutes");
  }

  if (currentSec != seconds) {
    int curSec = currentSec / 10;
    int tenSec = seconds / 10;
    if (curSec != tenSec) {
      Line(5, 11, colors.Black);
      Line(tenSec, 11, color);
    }
    Line(9, 1, colors.Black);
    Line((seconds % 10), 1, color);
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

void printTimeStatic() {
  if (INFO)Serial.println("Time-Static");
  leds[0] = colors.Yellow;
  leds[10] = colors.Yellow;
  leds[16] = colors.Yellow;
  leds[17] = colors.Yellow;
  leds[27] = colors.Yellow;
  leds[33] = colors.Yellow;
  leds[34] = colors.Yellow;
  leds[44] = colors.Yellow;

  for (int i = 47; i < NUM_LEDS_CLOCK; i++) {
    leds[i] = colors.Yellow;
  }

  delay(2);
  FastLED.show();
}

void printTowerLight() {
  if (INFO)Serial.println("Tower Lighting");

  // red guiding lights
  for (int i = 60; i < NUM_LEDS; i++) {
    leds[i] = colors.Red;
  }
  //blue-lila upper-deck
  for (int i = 56; i < 60; i++) {
    leds[i] = colors.UpperDeck;
  }
  delay(2);
  FastLED.show();
}

void updateBrightness() { // Read analog sensor to detect brightness
  int sensorWert = analogRead(BRIGHTNESS_PIN);
  int newBrightness;

  if (sensorWert < 250) {
    newBrightness = 1;
  } else if (sensorWert < 300) {
    newBrightness = map(sensorWert, 250, 300, 1, 150);
  } else if (sensorWert < 500) {
    newBrightness = map(sensorWert, 300, 500, 150, 255);
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

