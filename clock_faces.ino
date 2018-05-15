////////////////////////////////////////////////////////////////////////////////////////////
//   CLOCK DISPLAY MODES
// Add any new display mode functions here.
////////////////////////////////////////////////////////////////////////////////////////////

extern DateTime now;
int (*clockFacesArray[])(long) = {minimalClock, basicClock, smoothSecond, outlineClock,   simplePendulum, breathingClock};
ModeChanger clockFaces (clockFacesArray, sizeof(clockFacesArray)/sizeof(clockFacesArray[0]));

//////////////////////////////////////////////////////////////////////////////////////////
returnValue minimalClock (long currentCallNumber) {
    if (currentCallNumber > 10) { // a dark screen at first few calls
        uint8_t hourPos = _hourPos (now.hour(), now.minute());
        findLED(hourPos)->r = 255;
        findLED(now.minute())->g = 255;
        findLED(now.second())->b = 255;
    }
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue basicClock(long currentCallNumber) {
    uint8_t hourPos = _hourPos (now.hour(), now.minute());

    // Hour (6 lines of code)
          findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
          findLED(hourPos-1)->g = findLED(hourPos+1)->g = 0;
          findLED(hourPos-1)->b = findLED(hourPos+1)->b = 0;

          findLED(hourPos)->r  = 200;
          findLED(hourPos)->g  =   0;
          findLED(hourPos)->b  =   0;

    // Minute  
          findLED(now.minute())->r =   0;
          findLED(now.minute())->g = 255;
          findLED(now.minute())->b =   0;

    // Second  
          findLED(now.second())->r =   0;
          findLED(now.second())->g =   0;
          findLED(now.second())->b = 255;

    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue smoothSecond (long currentCallNumber) {
    static unsigned long millisAtStart;
    static DateTime oldTime;
    static bool catchSeconds;
  
    if (currentCallNumber == 0) {
        oldTime = now;
        catchSeconds = false;
    } else {
        if (!catchSeconds) {  // Don't show seconds, wait for catching the beginning of new second
            if (now.second () != oldTime.second ()) {
                catchSeconds = true;
                millisAtStart = millis ();
            }
        } else {              // Usual operation
            if (now.second () != oldTime.second ()) {
                oldTime = now;
                millisAtStart = millis ();
            }

            // Hour (3 lines of code)
                  uint8_t hourPos = _hourPos (now.hour(), now.minute());
                  findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                  findLED(hourPos)->r = 170;

            // Minute  
                  findLED(now.minute())->g = 255;

            // Second (5 lines of code)
                  uint8_t delta = static_cast <uint8_t> (128F*(millis () - millisAtStart)/1000F);
                  uint8_t secondBrightness1 = NeoPixel_gamma8(NeoPixel_sine8(( 64+delta)); // 64 means from Max to Min
                  uint8_t secondBrightness2 = NeoPixel_gamma8(NeoPixel_sine8((192+delta)); // 192 means from Min to Max (same as sine8_0 ())

                  findLED(now.second ())->b =     secondBrightness1;
                  findLED(now.second () + 1)->b = secondBrightness2;    
        }
    }
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue outlineClock (long currentCallNumber) {
  for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
      findLED(i)->r = 100;
      findLED(i)->g = 100;
      findLED(i)->b = 100;
  }

  // Hour (3 lines of code)
          uint8_t hourPos = _hourPos (now.hour(), now.minute());
          findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
    
  // Second  
          findLED(now.second())->b = 255;

    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
/*returnValue minimalMilliSec(long currentCallNumber) {
  if (now.second()!=old.second())
    {
      old = now;
      cyclesPerSec = (millis() - newSecTime);
      newSecTime = millis();
    } 
  // set hour, min & sec LEDs
  uint8_t hourPos = _hourPos (now.hour(), now.minute());
  subSeconds = (((millis() - newSecTime)*60)/cyclesPerSec)%60;  // This divides by 733, but should be 1000 and not sure why???

  // Millisec lights are set first, so hour/min/sec lights override and don't flicker as millisec passes
          findLED(subSeconds)->r = 50;
          findLED(subSeconds)->g = 50;
          findLED(subSeconds)->b = 50;

  // The colours are set last, so if on same LED mixed colours are created
  // Hour (3 lines of code)
          findLED(hourPos-1)->r = 30;
          findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
  
  // Second  FIXED
          findLED(now.second())->b = 200;

    return returnValue::CONTINUE;
}*/

//////////////////////////////////////////////////////////////////////////////////////////
returnValue simplePendulum (long currentCallNumber) {
    const int halfAmplitude = 8;
    const uint8_t pendulumSpeed = 1;
    static unsigned long millisAtStart;
  
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = 10;
        findLED(i)->g = 10;
        findLED(i)->b = 100;
    }

    if (currentCallNumber == 0) {
        millisAtStart = millis ();
    } else {

        // Pendulum lights are set first, so hour/min/sec lights override and don't flicker as millisec passes

                uint8_t deltaS = (((millis () - millisAtStart)*pendulumSpeed)%1000)/4;  // = 0..255
                uint8_t pendulumPos = 30 + halfAmplitude - (2*halfAmplitude * static_cast <int> (sine8_0 (deltaS)))/256f; // = 38..22

                findLED(pendulumPos)->r = 100;
                findLED(pendulumPos)->g = 100;
                findLED(pendulumPos)->b = 100;

        // The colours are set last, so if on same LED mixed colours are created
        // Hour (3 lines of code)
                uint8_t hourPos = _hourPos (now.hour(), now.minute());
                findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                findLED(hourPos)->r  = 190;

        // Minute  
                findLED(now.minute())->g = 255;

        // Second  
                findLED(now.second())->b = 255;
    }
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue breathingClock (long currentCallNumber) {
    static unsigned long millisAtStart;
  
    if (currentCallNumber == 0) {
        millisAtStart = millis ();
    } 
    
    uint8_t breathBrightness = sine8_0 (millisAtStart%256) >> 2; // 0..64
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = breathBrightness;
        findLED(i)->g = breathBrightness;
        findLED(i)->b = breathBrightness;
    }
  
  // Hour (3 lines of code)
          uint8_t hourPos = _hourPos (now.hour(), now.minute());
          findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
    
  // Second  
          findLED(now.second())->b = 255;

    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Time adjustment routines
//returnValue (*_timeAdjustmentRoutines[])(long) = {adjustHours, adjustMinutes, adjustSeconds};
//ModeChanger timeAdjustmentRoutines (_timeAdjustmentRoutines, sizeof(_timeAdjustmentRoutines)/sizeof(_timeAdjustmentRoutines[0]));

void adjustTime () {
    static uint8_t h (now.hour ());
    static uint8_t m (now.minute ());
    static uint8_t s (now.second ());
    
    if (!currentCallNumber) {
        h = now.hour ();
        m = now.minute ();
        s = now.second ();
        timer.setInterval ("ms", 10000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
    } else {
        if (button.shortPress()) {
            RTC.adjust (DateTime (now.year (), now.month (), now.day (), h, m, s));
            return returnValue::NEXT;
        }
        if (button.longPress() || timer.needToTrigger()) { // Adjustment will be aborted 
            return returnValue::TERMINATE;
        }
    
        if (rotaryTurnLeft ()) {
            if (--h < 0 ) h = 23;
            timer.switchOn (); // user's activity - no termination is necessary
        }    
        if (rotaryTurnRight ()) {
            if (++h > 23 ) h = 0;
            timer.switchOn ();
        }    
        drawAdjustmentClock (h, m, s);
    }
    return returnValue::CONTINUE;
}
                                         
returnValue adjustMinutes (long currentCallNumber) {
    static uint8_t h (now.hour ());
    static uint8_t m (now.minute ());
    static uint8_t s (now.second ());
    
    if (!currentCallNumber) {
        h = now.hour ();
        m = now.minute ();
        s = now.second ();
        timer.setInterval ("ms", 10000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
    } else {
        if (button.shortPress()) {
            RTC.adjust (DateTime (now.year (), now.month (), now.day (), h, m, s));
            return returnValue::NEXT;
        }
        if (button.longPress() || timer.needToTrigger()) { // Adjustment will be aborted 
            return returnValue::TERMINATE;
        }
    
        if (rotaryTurnLeft ()) {
            if (--m < 0 ) {
                m = 59;
                if (--h < 0 ) h = 23;
            }
            timer.switchOn (); // user's activity - no termination is necessary
        }    
        if (rotaryTurnRight ()) {
            if (++m > 59 ) {
                m = 0;
                if (++h > 23 ) h = 0;
            }
            timer.switchOn ();
        }    
        drawAdjustmentClock (h, m, s);
    }
    return returnValue::CONTINUE;
}

void drawAdjustmentClock (int deltaSeconds) {
    
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r =  5;
        findLED(i)->g = 70;
        findLED(i)->b =  5;
    }
    
    int dh = deltaSeconds / 3600;
    int dm = (deltaSeconds - dh * 3600) / 60;
    int ds = (deltaSeconds - dh * 3600 - dm * 60);

    // Hour (3 lines of code)
          uint8_t hourPos = _hourPos (now.hour(), now.minute());
          findLED (hourPos)->r  = 190;
          if (h >= 12) 
              findLED (hourPos-1)->r = findLED(hourPos+1)->r = 30;

    // Minute  
          findLED (m)->g = 255;

    // Second  
          findLED (s)->b = 255;
}

/*
returnValue adjustSeconds (long currentCallNumber) {
    static int dh;
    static int dm;
    static int ds;
    
    if (!currentCallNumber) {
        dh = 0;
        dm = 0;
        ds = 0;
        timer.setInterval ("ms", 10000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();
    } else {
        if (button.shortPress()) {
            RTC.adjust (DateTime (now.year (), now.month (), now.day (), now.hour () + dh, now.minute () + dm, now.second () + ds));
            return returnValue::NEXT;
        }
        if (button.longPress() || timer.needToTrigger()) { // Adjustment will be aborted 
            return returnValue::TERMINATE;
        }
    
        if (rotaryTurnLeft ()) {
            if (--s < 0 ) {
                s = 59;
                if (--m < 0 ) m = 59;
            }
            timer.switchOn (); // user's activity - no termination is necessary
        }    
        if (rotaryTurnRight ()) {
            if (++s > 59 ) {
                s = 0;
                if (++m > 59 ) m = 0;
            }
            timer.switchOn ();
        }    
        drawAdjustmentClock (h, m, s);
    }
    return returnValue::CONTINUE;
}

*/
