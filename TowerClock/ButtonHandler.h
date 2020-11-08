#ifndef ButtonHandler_H
#define ButtonHandler_H

#include <Arduino.h>

class ButtonHandler {
  public:
    ButtonHandler();
    ButtonHandler(uint8_t pinNumber);
    //~ButtonHandler();

    void loop();

    bool getPressed1x();
    bool getPressed2x();
    bool getPressed3x();
    bool getLongPressed();

    bool error = false;


  private:
    uint8_t pin = 2;
    bool pressed = false;
    uint16_t pressStartTime = 0;
    uint16_t pressStopTime = 0;
    uint8_t pressedCounter = 0;

    bool pressed1x = false;
    bool pressed2x = false;
    bool pressed3x = false;
    //bool pressed4x = false;
    bool longPressed = false;
};

#endif
