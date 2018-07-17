//struct EEPROMdata { + 
//DateTime lastClockManualCorrectionTime;

extern ControlStruct clockFacesControlStruct;
/*
////////////////////
returnValue (*enterCodeArray[])(long) = {enterCode};
const int len_enterCodeArray = 1;
ControlStruct enterCodeControlStruct {enterCodeArray, len_enterCodeArray, nullptr, 
                                       LoopMode::INFINITE, &clockFacesControlStruct, &clockFacesControlStruct, 0, 0};


//const uint16_t correctMagicValue = 0xE3D3+unlockCodeLen; // Move to mainstruct!

returnValue enterCode (long currentCallNumber) {
    static unsigned long millisAtStart;
    static Timer timer;
    static int value;
    static int count;
    static int tempBuf[unlockCodeLen];
    
    if (!currentCallNumber) { // First-time entry
        timer.setInterval ("aC", 9000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
        millisAtStart = millis ();
        count = value = 0;
    } else if (millis () - millisAtStart > demoTime) {
        if (rotaryTurnLeft ()) {
            if (value > 0) --value;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (rotaryTurnRight ()) {
            ++value;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (button.shortPress()) {
            tempBuf[count++] = value;
            
            if (count > unlockCodeLen) {
                for (int i = 0; i < unlockCodeLen; i++) {
                    eepromData.correctTotalUnlockCode[i] = tempBuf[i];
                }
                writeEeprom ();
            }
            return returnValue::SHORTPRESS; 
        }
        if (timer.needToTrigger() || button.longPress()) { // Adjustment will be aborted 
            timer.switchOff ();
            return returnValue::LONGPRESS; // no adjustments saved
        }
    }
    drawCode (millis () - millisAtStart, value, count);
    return returnValue::CONTINUE; // continue next loop
}

void drawCode (unsigned long _millis, int value, int count) {
    uint8_t brt = 255;

    // 5-minute marks
    findLED(0)->r = findLED(5)->r = findLED(10)->r = findLED(15)->r = 50;

    brt = NeoPixel_gamma8 (sine8_0 (static_cast<uint8_t>(_millis*9*128.0/demoTime)));  // = 0..255
    if (_millis < demoTime) {
        findLED(2)->r = findLED(2)->g = findLED(4)->r = findLED(4)->g = findLED(6)->r = findLED(6)->g = brt;
    } else {

        // Display currently entered digit
        for (int i = 0; i <= value; i++) {
            findLED(i)->r = findLED(i)->g = brt;
        }

        // Count of entered digits
        const int startingPlace = 45;
        for (int i = startingPlace; i <= startingPlace + count; i++) {
            findLED(i)->b = brt;
        }

        for (int i = 0; i < startingLEDs; i++) {
            //_leds[i].g = 5; 
            _leds[i].r = 128;
            _leds[i].g = 128;
        }
    }
}
/*
void handleUnlockCode (void) {
    if (correctTotalUnlockCode[0] != 0) { // First "0" stands for totally unlocked clock
    }
}
*/
