//                                   //
//  ### Serial ConnectionHandler ### //
//                                   //
void serialLoop() {
  static bool connected = false;
  static bool syncing = false;
  if (readSerial()) {
    // listen for identifier
    if (serialBuffer[0] == 'T' && serialBuffer[1] == 'C' && serialBuffer[2] == 'T' && serialBuffer[3] == 'S') {
      // identified --> respond
      Serial.println("--TowerClock--");
      connected = true;
    }
    if (serialBuffer[0] == 'A' && serialBuffer[1] == 'B' && connected) {  // set ready to sync
      // init time transfer via AB<>
      Serial.println("--RTS--");
      // time wave on the tower (green?!)
      currentMode = Mode::SerialConnection;
      syncing = true;
    }
    if (serialBuffer[0] == 'C' && serialBuffer[1] == 'D' && connected && syncing) {  // timestring incomming
      // listen for time string CD<YYMMDDwHHMMSS>
      if (setRTC()) {
        // go back to time display.
        Serial.println("--TCS--");
        syncing = false;
        FastLED.clear();
        currentMin = 111;
        currentMode = Mode::Time;
      }
    }
    if (serialBuffer[0] == 'E' && serialBuffer[1] == 'F') {  //disconnect
      // go back to time display.
      Serial.println("EF");
      connected = false;
      syncing = false;
      FastLED.clear();
      currentMin = 111;
      currentMode = Mode::Time;
    }
  }
}

bool setRTC() {
  if (serialBuffer[0] == 'C' && serialBuffer[1] == 'D' && serialBuffer[15] == 'x') {
    byte Temp1, Temp2;

    RTCHardware.setClockMode(false);  // set to 24h
    //setClockMode(true); // set to 12h

    //Read year first
    Temp1 = (byte)serialBuffer[2] - 48;
    Temp2 = (byte)serialBuffer[3] - 48;
    RTCHardware.setYear(Temp1 * 10 + Temp2);
    // now month
    Temp1 = (byte)serialBuffer[4] - 48;
    Temp2 = (byte)serialBuffer[5] - 48;
    RTCHardware.setMonth(Temp1 * 10 + Temp2);
    // now date
    Temp1 = (byte)serialBuffer[6] - 48;
    Temp2 = (byte)serialBuffer[7] - 48;
    RTCHardware.setDate(Temp1 * 10 + Temp2);
    // now Day of Week
    Temp1 = (byte)serialBuffer[8] - 48;
    RTCHardware.setDoW(Temp1);
    // now Hour
    Temp1 = (byte)serialBuffer[9] - 48;
    Temp2 = (byte)serialBuffer[10] - 48;
    RTCHardware.setHour(Temp1 * 10 + Temp2);
    // now Minute
    Temp1 = (byte)serialBuffer[11] - 48;
    Temp2 = (byte)serialBuffer[12] - 48;
    RTCHardware.setMinute(Temp1 * 10 + Temp2);
    // now Second
    Temp1 = (byte)serialBuffer[13] - 48;
    Temp2 = (byte)serialBuffer[14] - 48;
    RTCHardware.setSecond(Temp1 * 10 + Temp2 + 1);
    return true;
  }
  return false;
}

// read command from serial interface (USB)
// syntax: <data>\r\n
// save the data in global serialBuffer variable
bool readSerial() {
  static uint8_t idx = 0;
  static char charBuffer[20];

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (idx > 1 && c == '\n' && charBuffer[idx - 1] == '\r') {
      charBuffer[idx - 1] = '\0';
      idx = 0;
      strcpy(serialBuffer, charBuffer);
      return true;  // complete command found
    } else if (idx < 20) {
      charBuffer[idx++] = c;
      return false;
    } else if (idx >= 20) {
      idx = 0;
    }
  }
  return false;
}