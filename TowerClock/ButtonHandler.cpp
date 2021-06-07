#include "ButtonHandler.h"

//constructor
ButtonHandler::ButtonHandler() {
}

ButtonHandler::ButtonHandler(uint8_t pinNumber) {
  pin = pinNumber;
  pinMode(pin, INPUT);
}

void ButtonHandler::loop() {
  // check if pressed
  bool input = digitalRead(pin); // == HIGH ? true : false;                //check at PinOut
  uint16_t time = millis();

  if (!pressed && input) {          // start button press
    pressStartTime = time;
    pressed = true;
  }
  else if (pressed && input) {
    // check for longPressed
    if (time - pressStartTime > 3000) {    //long press threshold
      longPressed = true;
      pressStartTime = time;
      pressedCounter = -1;
      // TODO: nothing else should be checked
    }
  }
  else if (pressed && !input) {     // stop button press
    pressStopTime = time;
    pressed = false;
    pressedCounter++;
  }
  else if (!pressed && !input) {
    if (pressStopTime + 500 < time) {      // threshold between short button presses
      switch (pressedCounter) {
        case 1:
          pressed1x = true;
          break;
        case 2:
          pressed2x = true;
          break;
        case 3:
          pressed3x = true;
          break;
        case 4:
          pressed4x = true;
          break;
        default:
          error = true;
          break;
      }
      pressedCounter = 0;
    }
    if (pressStopTime + 1000 < time) {    // reset options after threshold (long threshold)
      // reset every option
      if (pressed1x) pressed1x = false;
      if (pressed2x) pressed2x = false;
      if (pressed3x) pressed3x = false;
      if (pressed4x) pressed4x = false;
      if (longPressed) longPressed = false;

    }
  }
}

bool ButtonHandler::getPressed1x() {
  bool p = pressed1x;
  if (p) pressed1x = false;
  return p;
}

bool ButtonHandler::getPressed2x() {
  bool p = pressed2x;
  if (p) pressed2x = false;
  return p;
}

bool ButtonHandler::getPressed3x() {
  bool p = pressed3x;
  if (p) pressed3x = false;
  return p;
}

bool ButtonHandler::getPressed4x() {
  bool p = pressed4x;
  if (p) pressed4x = false;
  return p;
}

bool ButtonHandler::getLongPressed() {
  bool p = longPressed;
  if (p) longPressed = false;
  return p;
}
