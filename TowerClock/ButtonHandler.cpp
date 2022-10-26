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
  bool input = digitalRead(pin);  // == HIGH ? true : false;                //check at PinOut
  uint16_t time = millis();

  if (!pressed && input) {  // start button press
    pressStartTime = time;
    pressed = true;
  } else if (pressed && input) {
    // check for longPressed
    if (time - pressStartTime > 3000) {  //long press threshold
      m_buttonState = ButtonState::PressedLong;
      pressStartTime = time;
      pressedCounter = -1;
      // TODO: nothing else should be checked
    }
  } else if (pressed && !input) {  // stop button press
    pressStopTime = time;
    pressed = false;
    pressedCounter++;
  } else if (!pressed && !input) {
    if (pressStopTime + 500 < time) {  // threshold between short button presses
      switch (pressedCounter) {
        case 1:
          m_buttonState = ButtonState::Pressed1x;
          break;
        case 2:
          m_buttonState = ButtonState::Pressed2x;
          break;
        case 3:
          m_buttonState = ButtonState::Pressed3x;
          break;
        case 4:
          m_buttonState = ButtonState::Pressed4x;
          break;
        default:
          m_buttonState = ButtonState::None;
          error = true;
          break;
      }
      pressedCounter = 0;
    }
    if (pressStopTime + 1000 < time) {  // reset options after threshold (long threshold)
      // reset every option
      m_buttonState = ButtonState::None;
    }
  }
}

ButtonHandler::ButtonState ButtonHandler::getButtonState() {
  ButtonState bs = m_buttonState;
  if (bs != ButtonState::None) m_buttonState = ButtonState::None;
  return bs;
}