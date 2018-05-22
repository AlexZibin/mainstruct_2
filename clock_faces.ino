////////////////////////////////////////////////////////////////////////////////////////////
//   CLOCK DISPLAY MODES
//   Add any new display mode functions here.
////////////////////////////////////////////////////////////////////////////////////////////

extern ControlStruct adjustTimeControlStruct;

returnValue (*clockFacesArray[])(long) = {minimalClock, spacerShortDemo, basicClock, spacerShortDemo, 
                                          smoothSecond, spacerShortDemo, outlineClock, spacerShortDemo,   
                                          simplePendulum, spacerShortDemo, breathingClock, spacerShortDemo};
const int len_clockFacesArray = sizeof (clockFacesArray) / sizeof (clockFacesArray[0]);
ControlStruct clockFacesControlStruct {clockFacesArray, len_clockFacesArray, backlightLEDsEndingFunc, 
//                                       LoopMode::INFINITE, &longDemoControlStruct, &adjustTimeControlStruct};
                                       LoopMode::INFINITE, &shortIntroControlStruct, &adjustTimeControlStruct, 0};
/*    fPtr *funcArray;
    int funcArrayLen;
    fPtr endingFunction;
    LoopMode loopMode;
    ControlStruct *nextPress;
    ControlStruct *nextLongPress;
    int startMode;
};*/


//extern DateTime now;

//////////////////////////////////////////////////////////////////////////////////////////
returnValue minimalClock (long currentCallNumber) {
    if (currentCallNumber > 10) { // a dark screen at first few calls
        uint8_t hourPos = _hourPos (now.hour(), now.minute());
        findLED(hourPos)->r = 255;
        findLED(now.minute())->g = 160;
        findLED(now.second())->b = 160;
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

            // Hour
                  uint8_t hourPos = _hourPos (now.hour(), now.minute());
                  findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                  findLED(hourPos)->r = 170;
                  findLED(hourPos-1)->g = findLED(hourPos+1)->g = findLED(hourPos)->g = 0;
                  findLED(hourPos-1)->b = findLED(hourPos+1)->b = findLED(hourPos)->b = 0;

            // Minute  
                  findLED(now.minute())->g = 255;

            // Second (5 lines of code)
                  uint8_t delta = static_cast <uint8_t> (128.0*(millis () - millisAtStart)/1000.0); // 0..128
                  uint8_t secondBrightness1 = NeoPixel_gamma8 (NeoPixel_sine8( 64+delta)); // 64 means from Max to Min
                  uint8_t secondBrightness2 = NeoPixel_gamma8 (NeoPixel_sine8(192+delta)); // 192 means from Min to Max (same as sine8_0 ())

                  findLED(now.second ())->b =     secondBrightness1;
                  findLED(now.second () + 1)->b = secondBrightness2;    
        }
    }
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue outlineClock (long currentCallNumber) {
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = 35;
        findLED(i)->g = 30;
        findLED(i)->b = 30;
    }
  
    // Hour (3 lines of code)
            uint8_t hourPos = _hourPos (now.hour(), now.minute());
            findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
            findLED(hourPos)->r  = 190;
    
    // Minute  
            findLED(now.minute())->r =   0;
            findLED(now.minute())->g = 255;
            findLED(now.minute())->b =   0;
      
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
    const int pendulumPeriod = 2;
    static unsigned long millisAtStart;
  
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = 35;
        findLED(i)->g = 30;
        findLED(i)->b = 30;
    }

    if (currentCallNumber == 0) {
        millisAtStart = millis ();
    } else {

        // Pendulum lights are set first, so hour/min/sec lights override and don't flicker as millisec passes

                uint8_t deltaS = (((millis () - millisAtStart)/pendulumPeriod)%1000)/4;  // = 0..255
                uint8_t pendulumPos = 30 + halfAmplitude - (2*halfAmplitude * static_cast <int> (sine8_0 (deltaS)))/256.0; // = 38..22

                findLED(pendulumPos)->r = 0;
                findLED(pendulumPos)->g = 100;
                findLED(pendulumPos)->b = 100;

        // The colours are set last, so if on same LED mixed colours are created
        // Hour 
                uint8_t hourPos = _hourPos (now.hour(), now.minute());
                findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                findLED(hourPos)->r  = 190;
                findLED(hourPos-1)->g = findLED(hourPos+1)->g = findLED(hourPos)->g = 0;
                findLED(hourPos-1)->b = findLED(hourPos+1)->b = findLED(hourPos)->b = 0;

        // Minute  
                findLED(now.minute())->g = 255;
                findLED(now.minute())->r = findLED(now.minute())->b = 0;

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
    
    uint8_t breathBrightness = (sine8_0 (((millis()-millisAtStart)/20)%256) / 4 + 1); // (0..64) + 1
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = breathBrightness;
        findLED(i)->g = breathBrightness;
        findLED(i)->b = breathBrightness;
    }
  
  // Hour (3 lines of code)
          uint8_t hourPos = _hourPos (now.hour(), now.minute());
          findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
          findLED(hourPos-1)->g = findLED(hourPos+1)->g = findLED(hourPos)->g = 0;
          findLED(hourPos-1)->b = findLED(hourPos+1)->b = findLED(hourPos)->b = 0;
  
  // Minute  
          findLED(now.minute())->g = 255;
          findLED(now.minute())->r = findLED(now.minute())->b = 0;
    
  // Second  
          findLED(now.second())->b = 255;

    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Time adjustment routines
returnValue (*adjustTimeArray[])(long) = {adjustTime};
const int len_adjustTimeArray = 1;
ControlStruct adjustTimeControlStruct {adjustTimeArray, len_adjustTimeArray, nullptr, 
                                       LoopMode::INFINITE, &clockFacesControlStruct, &clockFacesControlStruct, 0};
//                                                         ^
//                                                         short press is handled inside adjustTime ()

//                                       LoopMode::INFINITE, nullptr, &clockFacesControlStruct, 0};

returnValue adjustTime (long currentCallNumber) {
    static int adjustmentStep; // 60*60 for hours, 60 for minutes, 1 for seconds
    static long deltaSeconds;
    static Timer timer;

    if (!currentCallNumber) { // First-time entry
        timer.setInterval ("adjustTime", 10000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();

        deltaSeconds = 0;
        adjustmentStep = 3600; // HOURS = 60*60
        Serial.println("\n adjustmentStep = 3600; // HOURS = 60*60");
    } else {
        if (rotaryTurnLeft ()) {
            deltaSeconds -= adjustmentStep;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
        }    
        if (rotaryTurnRight ()) {
            deltaSeconds += adjustmentStep; // HOURS -> MINUTES -> SECONDS
            timer.switchOn ();
        }    
        int dh, dm, ds;
        if (button.shortPress()) {
            timer.switchOn ();
            if (adjustmentStep == 1) {  // work done
                splitHMS (deltaSeconds, dh, dm, ds);
                Serial.print("\n adjustTime () detected shortPress() !!! \n dh, dm, ds: ");
                Serial.print(dh); Serial.print(" "); Serial.print(dm); Serial.print(" "); Serial.println(ds);
                
                RTC.adjust (DateTime (now.year (), now.month (), now.day (), dh, dm, ds));
                return returnValue::SHORTPRESS; 
            }
            adjustmentStep /= 60;
        }
        if (timer.needToTrigger()) { // Adjustment will be aborted 
            Serial.print("\n adjustTime () detected needToTrigger() !!! \n dh, dm, ds: ");
            timer.switchOff ();
            return returnValue::LONGPRESS; // no adjustments saved
        }
        drawAdjustmentClock (deltaSeconds, adjustmentStep);
    }
    return returnValue::CONTINUE; // continue next loop
}

// It must blink with H, M or S depending on adjustmentStep
void drawAdjustmentClock (long deltaSeconds, int adjustmentStep) {

    static int memory = -1;
    static unsigned long millisAtStart;
    if (adjustmentStep != memory) {
        millisAtStart = millis ();
        memory = adjustmentStep;
        //erase60leds ();
    }

    // 5-minute marks
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r =  9;
        findLED(i)->g =  9;
        findLED(i)->b =  15;
    }

    unsigned long deltaT = millis () - millisAtStart;
    float blinkingPeriodSec = 2.0; 
    if (adjustmentStep == 1) blinkingPeriodSec = 0.5;

    float brt = squareSine8 ((static_cast<uint8_t>(deltaT/blinkingPeriodSec))%256)/256.0; // 0..1 range
    
    int h, m, s;
    splitHMS (deltaSeconds, h, m, s);

    // Hour (3 lines of code)
            uint8_t hourPos = _hourPos (h, m);
            uint8_t hourBrightness = 190;
            if (adjustmentStep == 3600) hourBrightness *= brt;
    
            findLED (hourPos)->r  = NeoPixel_gamma8(hourBrightness);
            if (h >= 12) 
                findLED (hourPos-1)->r = findLED(hourPos+1)->r = NeoPixel_gamma8(hourBrightness/6);

    // Minute  
            uint8_t minuteBrightness = 255;
            if (adjustmentStep == 60) minuteBrightness *= brt;
            findLED (m)->g = NeoPixel_gamma8(minuteBrightness);

    // Second  
            uint8_t secondsBrightness = 255;
            if (adjustmentStep == 1) secondsBrightness *= brt;
            findLED (s)->b = NeoPixel_gamma8(secondsBrightness);
}

void splitHMS (int deltaSeconds, int &dh, int &dm, int &ds) {
    dh = deltaSeconds / 3600;
    dm = (deltaSeconds - dh * 3600) / 60;
    ds = (deltaSeconds - dh * 3600 - dm * 60);
    
    ds += now.second ();
    while (ds >= 60) {
        ds -= 60;
        dm += 1;
    }
    while (ds < 0) {
        ds += 60;
        dm -= 1;
    }
    
    dm += now.minute ();
    while (dm >= 60) {
        dm -= 60;
        dh += 1;
    }
    while (dm < 0) {
        dm += 60;
        dh -= 1;
    }
    
    dh += now.hour ();
    while (dh >= 24) dh -= 24;
    while (dh < 0) dh += 24;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
returnValue spacerShortDemo (long currentCallNumber) {
    static unsigned long millisAtStart;
    const long playTimeMs = 1000;
    const float dimmingTimeMs = 500.0;
    static int direction = -1;

    if (currentCallNumber == 0) {
        millisAtStart = millis ();
        erase60leds ();
        direction = -direction;
    }

    unsigned long deltaT = millis () - millisAtStart;
    unsigned long timeStep = 1;
    float wavelen = 10.0;

    if (deltaT > playTimeMs) { LEDS.clear (); return returnValue::NEXT; };

    static float ledBrightness;
    byte r, g, b;
    Wheel ((millis()/15)%384, r, g, b);
    for (int led = 0; led < numLEDs; led++) {
        uint8_t firstBrightness = sin_1_2 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*2.1))
                                                                -direction*led*wavelen*(1+deltaT/(playTimeMs+4000.0)))%256);
        
        // dimming at the beginning of demo and in the end:
        if (deltaT <= dimmingTimeMs) ledBrightness = deltaT/dimmingTimeMs;
        if (deltaT >= playTimeMs-dimmingTimeMs) ledBrightness = (playTimeMs-deltaT)/dimmingTimeMs;
        
        float brt;
        const float shade = 5;
        if (led < shade) {      // gimming LEDs near 12 o'clock
            brt = NeoPixel_gamma8(firstBrightness*(led/shade)*ledBrightness)/128.0;
            findLED(led)->r = brt*r;
            findLED(led)->g = brt*g;
            findLED(led)->b = brt*b;
        } else if (led >= numLEDs - shade) {      
            brt = NeoPixel_gamma8(firstBrightness*((numLEDs-led)/shade)*ledBrightness)/128.0;
            findLED(led)->r = brt*r;
            findLED(led)->g = brt*g;
            findLED(led)->b = brt*b;
        } else {
            brt = NeoPixel_gamma8(firstBrightness*ledBrightness)/128.0;
            findLED(led)->r = brt*r;
            findLED(led)->g = brt*g;
            findLED(led)->b = brt*b;
        }
    }
    return returnValue::CONTINUE;
}

void erase60leds (void) {
    for (int led = 0; led < numLEDs; led++) { // Don't erase starting LEDs (digits 3-6-9-12)
        findLED(led)->r = 0;
        findLED(led)->g = 0;
        findLED(led)->b = 0;
    }
}    

void backlightLEDsEndingFunc (long dummy) {
    static Timer timer;
    for (int i = 0; i < startingLEDs; i++) {
        _leds[i].g = 5;
        _leds[i].r = 5;
        _leds[i].b = 255;
    }
    
    if (eeprominfo.modeNumber <> modeChanger->getCurrModeNumber ()) {
        eeprominfo.modeNumber = modeChanger->getCurrModeNumber ();
        timer.setInterval ("backlightLEDsEndingFunc, Timer for save mode in EEPROM", 5000);
        timer.switchOn ();
    }

    if (timer.needToTrigger ())
        timer.switchOff ();
        Serial.println ("Saving mode in EEPROM!");
        writeEeprom ();
    }
    #ifdef MOSFET_LED 
      analogWrite(MOSFET_Pin, 255);
    #endif
}
