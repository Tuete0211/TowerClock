#ifndef ButtonHandler_H
#define ButtonHandler_H

#include <Arduino.h>

class ButtonHandler {
public:
  ButtonHandler();
  ButtonHandler(uint8_t pinNumber);
  //~ButtonHandler();

  bool error = false;

  enum ButtonState {
    None,
    Pressed1x,
    Pressed2x,
    Pressed3x,
    Pressed4x,
    PressedLong
  };

  void loop();
  ButtonState getButtonState();

private:
  uint8_t pin = 2;
  bool pressed = false;
  uint16_t pressStartTime = 0;
  uint16_t pressStopTime = 0;
  uint8_t pressedCounter = 0;

  ButtonState m_buttonState = ButtonState::None;
};

#endif