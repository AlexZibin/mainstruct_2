/////////////////////////////////////////////////////
//BRT
///////////////////////

//const unsigned long demoTime = 1500;

returnValue adjustBrightness (long currentCallNumber) {
    static unsigned long millisAtStart;
    static Timer timer;
    static uint8_t savedBrightness;
    const uint8_t brtStep = 20;

    if (!currentCallNumber) { // First-time entry
        timer.setInterval ("aT", 5000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
        millisAtStart = millis ();
        savedBrightness = getBrightness ();
    } else if (millis () - millisAtStart > demoTime) {
        if (rotaryTurnLeft ()) {
            decr0_255 (getBrightness (), brtStep);
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (rotaryTurnRight ()) {
            incr0_255 (getBrightness (), brtStep);
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (button.shortPress()) {
            writeEeprom ();
            return returnValue::SHORTPRESS; 
        }
        if (timer.needToTrigger() || button.longPress()) { // Adjustment will be aborted 
            //Serial.println("\n setBrightness (savedBrightness)");
            setBrightness (savedBrightness);
            timer.switchOff ();
            return returnValue::LONGPRESS; // no adjustments saved
        }
    }
    drawBrtClock (millis () - millisAtStart);
    return returnValue::CONTINUE; // continue next loop
}

uint8_t decr0_255 (uint8_t &value, uint8_t step) {
    const int minVal = 2;
    
    int tt = static_cast<int>(value) - step;
    
    if (tt < minVal) 
        value = minVal;
    else
        value -= step;
    return value;
}

uint8_t incr0_255 (uint8_t &value, uint8_t step) {
    int tt = static_cast<int>(value) + step;
    
    if (tt > 255) 
        value = 255;
    else
        value += step;
    return value;
}

void drawBrtClock (unsigned long _millis) {
    uint8_t a255 = 255;
    int brtMapped = getBrightness ()/255.0*30 + 30;

    if (_millis < demoTime) {
        a255 = NeoPixel_gamma8 (sine8_0 (static_cast<uint8_t>(_millis*3*128.0/demoTime)));  // = 0..255
        brtMapped = a255/255.0*30 + 30;
    }
    
    for (int i = 30; i < brtMapped; i++) {
        findLED(i)->r = findLED(i)->g = findLED(i)->b = 128;
    }

    // 5-minute marks
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r =  9;
        findLED(i)->g =  15;
        findLED(i)->b =  9;
    }

    for (int i = 0; i < startingLEDs; i++) {
        /*_leds[i].g = 5;
        _leds[i].r = 5;*/
        _leds[i].g = a255;
    }
    
    findLED(7)->r = findLED(8)->r = findLED(9)->r = a255;
    findLED(12)->g = findLED(13)->b = a255;

}

extern uint8_t *brightnessPtr;
uint8_t &getBrightness (void) {
    return *brightnessPtr; //eepromData.dayBrightness;
}

void setBrightness (uint8_t brt) {
    *brightnessPtr = brt;
    //eepromData.dayBrightness = brt;
}

