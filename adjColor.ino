/////////////////////////////////////////////////////
// Color of digits
///////////////////////

const unsigned long demoTime = 3500;

returnValue adjustColor (long currentCallNumber) {
    static unsigned long millisAtStart;
    static Timer timer;
    const uint8_t wheelStep = 20;
    static int dColor;

    if (!currentCallNumber) { // First-time entry
        timer.setInterval ("aT", 8000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
        millisAtStart = millis ();
        dColor = eepromData.digitsColor;
    } else if (millis () - millisAtStart > demoTime) {
        if (rotaryTurnLeft ()) {
            dColor -= wheelStep;
            while (dColor < 0) dColor += 384;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (rotaryTurnRight ()) {
            dColor += wheelStep*1.5;
            while (dColor > 384) dColor -= 384;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (button.shortPress()) {
            eepromData.digitsColor = dColor;
            writeEeprom ();
            return returnValue::SHORTPRESS; 
        }
        if (timer.needToTrigger() || button.longPress()) { // Adjustment will be aborted 
            //timer.switchOff ();
            return returnValue::LONGPRESS; // no adjustments saved
        }
    }
    drawColorDigits (millis () - millisAtStart, dColor);
    return returnValue::CONTINUE; // continue next loop
}

void drawColorDigits (unsigned long _millis, uint16_t dColor) {

    if (_millis < 1000) 
        for (int i = 0; i < startingLEDs; i++) {
            _leds[i].r = 255;
            _leds[i].g = 255;
            _leds[i].b = 255;
        }
    else {
        if (_millis < demoTime) {
            dColor += _millis * 400.0 / demoTime;
            log ("\tdColor: "); logln (dColor);
        }
        
        byte r, g, b;
        Wheel (dColor, r, g, b);
        if (_millis < demoTime) {
            incr0_255 (r, 20);
            incr0_255 (g, 20);
            incr0_255 (b, 20);
        }
        
        for (int i = 0; i < startingLEDs; i++) {
            _leds[i].r = r;
            _leds[i].g = g;
            _leds[i].b = b;
        }
    }
}

