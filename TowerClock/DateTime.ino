//                //
//  ### Time ###  //
//                //
void printTowerTime(CRGB color) {
    // get current time from RTC
  DateTime now = RTC.now();

  // define current time variables
  int hour = (now.hour() + DSTOffset) % 24;
  int minutes = now.minute();
  int seconds = now.second();


  // Running Lights before full and half hour
  if ((minutes == 59 || minutes == 29) && seconds >= 0 && seconds < 45) {
    updateBrightness(); // change brightness depending on brightness of room (sensor)
    
    static float startIndex = 0;
    startIndex = startIndex - 0.5f; /* motion speed */
    displayRunningLights((int)startIndex, true, true);
    
    printTowerLight();
    currentMin = 111;
    return;
  }
  
  // update clock every minute
  if (currentMin != minutes) {
    clearClock();
    updateBrightness(); // change brightness depending on brightness of room (sensor)
    checkForDST(now);
    Line(2, 50, colors.Black);
    Line((hour / 10), 50, color);
    Line(9, 40, colors.Black);
    Line((hour % 10), 40, color);
    currentHour = hour;

    Line(5, 33, colors.Black);
    Line((minutes / 10), 33, color);
    Line(9, 23, colors.Black);
    Line((minutes % 10), 23, color);
    currentMin = minutes;
  }
  
  // update seconds
  if (currentSec != seconds) {
    if (currentSec / 10 != seconds / 10) {
      Line(5, 16, colors.Black);
      Line((seconds /10), 16, color);
    }
    Line(9, 6, colors.Black);
    Line((seconds % 10), 6, color);
    currentSec = seconds;
  }

  // print yellow lights on every minute and half an hour
  if (((minutes == 59 || minutes == 29) && seconds >= 45) || seconds == 59) {
    printTimeStatic();
  }
}

// automatic DST detection (summer- / wintertime)
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
  else if (now.month() == 10) {
    if(now.day() >= 25 && DSTOffset == 1) {
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
    } else {
      DSTOffset = 1;
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
}
