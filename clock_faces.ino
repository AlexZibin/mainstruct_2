extern ControlStruct adjustTimeControlStruct;
extern ControlStruct a12hourPartyControlStruct;
extern ControlStruct adjustBrightnessControlStruct;

extern ControlStruct a12hourPartyControlStruct;
extern ControlStruct clockFacesControlStruct;

////////////////////
returnValue (*adjColorArray[])(long) = {adjustColor};
const int len_adjColorArray = 1;
ControlStruct adjustColorControlStruct {adjColorArray, len_adjColorArray, nullptr, 
                                       LoopMode::INFINITE, &clockFacesControlStruct, &clockFacesControlStruct, 0, 0};

////////////////////
returnValue (*brightnessArray[])(long) = {adjustBrightness};
const int len_brightnessArray = 1;
ControlStruct adjustBrightnessControlStruct {brightnessArray, len_brightnessArray, nullptr, 
                                       LoopMode::INFINITE, &adjustColorControlStruct, &adjustColorControlStruct, 0, 0};

////////////////////
returnValue (*adjustTimeArray[])(long) = {adjustTime};
const int len_adjustTimeArray = 1;
ControlStruct adjustTimeControlStruct {adjustTimeArray, len_adjustTimeArray, nullptr, 
                                       LoopMode::INFINITE, &clockFacesControlStruct, &clockFacesControlStruct, 0, 0};
//                                                         ^
//                                                         short press is handled inside adjustTime ()
//                                     LoopMode::INFINITE, nullptr, &clockFacesControlStruct, 0};


extern ControlStruct clockFacesControlStruct;
extern ControlStruct energySaverControlStruct;
extern ControlStruct longDemoFacesControlStruct;

/////// shortIntroControlStruct ///////
returnValue (*introFuncArray[])(long) = {fColorDemo1, fColorDemo2};
const int len_introFuncArray = sizeof(introFuncArray)/sizeof(introFuncArray[0]);
ControlStruct shortIntroControlStruct {introFuncArray, len_introFuncArray, nullptr, 
                                       LoopMode::ONCE, &clockFacesControlStruct, &energySaverControlStruct, 0, 0};

/////// energySaverControlStruct ///////
returnValue (*energySaverFuncArray[])(long) = {energySaver};
const int len_energySaverFuncArray = sizeof(energySaverFuncArray)/sizeof(energySaverFuncArray[0]);
ControlStruct energySaverControlStruct {energySaverFuncArray, len_energySaverFuncArray, nullptr, 
                                       LoopMode::ONCE, &shortIntroControlStruct, &clockFacesControlStruct, 0, 0};

////////////////////
returnValue (*clockFacesArray[])(long) = {minimalClock, spacerShortDemo, basicClock, spacerShortDemo, 
                                          smoothSecond, spacerShortDemo, outlineClock, spacerShortDemo,
                                          simplePendulum, spacerShortDemo, 
                                          minimalMilliSec, spacerShortDemo, 
                                          breathingClock};
const int len_clockFacesArray = sizeof (clockFacesArray) / sizeof (clockFacesArray[0]);
ControlStruct clockFacesControlStruct {clockFacesArray, len_clockFacesArray, backlightLEDsEndingFunc, 
//                                       LoopMode::INFINITE, &longDemoControlStruct, &adjustTimeControlStruct};
//                                       LoopMode::INFINITE, &shortIntroControlStruct, &adjustTimeControlStruct, 0};
                                       LoopMode::INFINITE, &longDemoFacesControlStruct, &adjustTimeControlStruct, 0, 0};

////////////////////
returnValue (*longDemoFacesArray[])(long) = {fColorDemo2, minimalClock, _ArinaDemo, 
                                          basicClock, blinkHours, 
                                          smoothSecond, brightLoop, 
                                          outlineClock, _ArinaDemo,
                                          simplePendulum, blinkHours, 
                                          minimalMilliSec, brightLoop, 
                                          breathingClock};
const int len_longDemoFacesArray = sizeof (longDemoFacesArray) / sizeof (longDemoFacesArray[0]);
ControlStruct longDemoFacesControlStruct {longDemoFacesArray, len_longDemoFacesArray, nullptr, 
                                       LoopMode::INFINITE, &adjustBrightnessControlStruct, &energySaverControlStruct, 0, 15};
/*    fPtr *funcArray;
    int funcArrayLen;
    fPtr endingFunction;
    LoopMode loopMode;
    ControlStruct *nextPress;
    ControlStruct *nextLongPress;
    int startMode;
};*/


////////////////////////////////////////////////////////////////////////////////////////////
//   CLOCK DISPLAY MODES
////////////////////////////////////////////////////////////////////////////////////////////
returnValue minimalClock (long currentCallNumber) {
    if (currentCallNumber > 10) { // a dark screen at first few calls
        uint8_t hourPos = _hourPos (now.hour(), now.minute());
        findLED(hourPos)->r = 255;
        findLED(now.minute())->g = 160;
        findLED(now.second())->b = 160;
    }
    backlightLEDs ();
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

    backlightLEDs ();
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
    backlightLEDs ();
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue outlineClock (long currentCallNumber) {
    for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
        findLED(i)->r = 20;
        findLED(i)->g = 25;
        findLED(i)->b = 20;
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
  
    backlightLEDs ();
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue minimalMilliSec (long currentCallNumber) {
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

            // Millis
                  float delta = 59.99*(millis () - millisAtStart)/1000; // = 0..59.99
                  int deltaInt = delta;  // = 0..59
                    
                  uint8_t brightness1 = NeoPixel_gamma8 (NeoPixel_sine8( 
                       64+static_cast <uint8_t> ((delta-deltaInt)*256.0))); //  64 means from Max to Min
                  uint8_t brightness2 = NeoPixel_gamma8 (NeoPixel_sine8(
                      192+static_cast <uint8_t> ((delta-deltaInt)*256.0))); // 192 means from Min to Max (same as sine8_0 ())
                  findLED(delta)->b   = brightness1;
                  findLED(delta+1)->b = brightness2;
            
            // Hour
                  uint8_t hourPos = _hourPos (now.hour(), now.minute());
                  findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                  findLED(hourPos)->r = 170;
                  findLED(hourPos-1)->g = findLED(hourPos+1)->g = findLED(hourPos)->g = 0;
                  findLED(hourPos-1)->b = findLED(hourPos+1)->b = findLED(hourPos)->b = 0;

            // Minute  
                  findLED(now.minute())->g = 255;

            // Second 
                  findLED(now.second ())->b = 255;
        }
    }
    backlightLEDs ();
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
returnValue simplePendulum (long currentCallNumber) {
    const int halfAmplitude = 6;
    const int pendulumPeriod = 6;
    static unsigned long millisAtStart;
  
    if (currentCallNumber == 0) {
        millisAtStart = millis ();
    } else {

        // Pendulum lights are set first, so hour/min/sec lights override and don't flicker as millisec passes

                uint8_t deltaS = (((millis () - millisAtStart)/pendulumPeriod)%1000)/4;  // = 0..255
                float pendulumPos = 30.0 - halfAmplitude + (2*halfAmplitude * sine8_0 (deltaS))/256.0; // = 22.0 .. 38.0
                int basePendulumPos = static_cast<int>(pendulumPos);
                float deltaPendulumPos = pendulumPos - basePendulumPos; // = 0..1
                uint8_t brt1 = NeoPixel_gamma8 (NeoPixel_sine8 ( 64 + static_cast<int>(deltaPendulumPos*128.0))/2); // = 255..0 // = 127..0
                uint8_t brt2 = NeoPixel_gamma8 (NeoPixel_sine8 (192 + static_cast<int>(deltaPendulumPos*128.0))/2); // = 0..255

                //findLED(basePendulumPos)->r = 0;
                findLED(basePendulumPos)->g = brt1;
                findLED(basePendulumPos)->b = brt1;

                //findLED(basePendulumPos+1)->r = 0;
                findLED(basePendulumPos+1)->g = brt2;
                findLED(basePendulumPos+1)->b = brt2;
        
                for (int i = 0; i < numLEDs; i += numLEDs/12) { // 60/12 = 5
                    //findLED(i)->r = 20;
                    //findLED(i)->g = 20;
                    findLED(i)->b = 20;
                }
            
        // The colours are set last, so if on same LED mixed colours are created
        // Hour 
                uint8_t hourPos = _hourPos (now.hour(), now.minute());
                findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
                findLED(hourPos)->r  = 190;
                //findLED(hourPos-1)->g = findLED(hourPos+1)->g = findLED(hourPos)->g = 0;
                //findLED(hourPos-1)->b = findLED(hourPos+1)->b = findLED(hourPos)->b = 0;

        // Minute  
                findLED(now.minute())->g = 190;
                //findLED(now.minute())->r = findLED(now.minute())->b = 0;

        // Second  
                findLED(now.second())->b = 190;
    }
    backlightLEDs ();
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

    backlightLEDs ();
    return returnValue::CONTINUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Time adjustment routines

returnValue adjustTime (long currentCallNumber) {
    static int adjustmentStep; // 60*60 for hours, 60 for minutes, 1 for seconds
    static long deltaSeconds;
    static Timer timer;

    if (!currentCallNumber) { // First-time entry
        timer.setInterval ("aT", 10000); // Adjustment will be aborted after 10 seconds without user's activity
        timer.switchOn ();

        deltaSeconds = 0;
        adjustmentStep = 3600; // HOURS = 60*60
        //Serial.println("\n adjustmentStep = 3600; // HOURS = 60*60");
    } else {
        if (rotaryTurnLeft ()) {
            deltaSeconds -= adjustmentStep;
            timer.switchOn (); // user's activity - no termination is necessary, restore 10 seconds timeout
            if (adjustmentStep == 1) deltaSeconds -= 4; // Small UX trick - x5 TurnLeft step for seconds only
            //Serial.print("\n adjustmentStep = "); Serial.println(adjustmentStep);
        }    
        if (rotaryTurnRight ()) {
            deltaSeconds += adjustmentStep; // HOURS -> MINUTES -> SECONDS
            timer.switchOn ();
            //Serial.print("\n adjustmentStep = "); Serial.println(adjustmentStep);
        }    
        int dh, dm, ds;
        if (button.shortPress()) {
            timer.switchOn ();
            if (adjustmentStep == 1) {  // work done
                splitHMS (deltaSeconds, dh, dm, ds);
                //Serial.print("\n adjustTime () detected shortPress() !!! \n dh, dm, ds: ");
                //Serial.print(dh); Serial.print(" "); Serial.print(dm); Serial.print(" "); Serial.println(ds);
                
                RTC.adjust (DateTime (now.year (), now.month (), now.day (), dh, dm, ds));

                
          #ifdef CLOCK_CORRECTION
                showDate ("Adjusting Time! ", now);
                
                if (((now.secondstime() - eepromData.lastClockManualCorrectionTime.secondstime())/3600 > 12)
                        && (deltaSeconds < 180)) {
                    Serial.println("\n Hey! Correcting clock accuracy!");
                    long baseSeconds = now.secondstime() - eepromData.lastClockManualCorrectionTime.secondstime();
                    float timeCoeff = static_cast<float> (deltaSeconds) / baseSeconds; // -0.0001 .. +0.0001
                    float oldTimeCoeff = eepromData.clockCorrectionSecInterval / (3600.0*24) * eepromData.clockCorrectionDirection;
                    timeCoeff += oldTimeCoeff;

                    eepromData.clockCorrectionSecInterval = 1 / abs (timeCoeff);
                    eepromData.clockCorrectionDirection = (timeCoeff > 0) ? 1 : -1;
                    eepromData.lastClockManualCorrectionTime = now;
                    writeEeprom ();
                    
                    Serial.print("\n clockCorrectionSecInterval = "); Serial.println(eepromData.clockCorrectionSecInterval);
                    Serial.print("clockCorrectionDirection = "); Serial.println(eepromData.clockCorrectionDirection);

                } else {
                    Serial.println("\n NOT correcting clock accuracy!");
                }
            #endif

                return returnValue::SHORTPRESS; 
            }
            adjustmentStep /= 60;
        }
        if (timer.needToTrigger()) { // Adjustment will be aborted 
            //Serial.print("\n adjustTime () detected needToTrigger() !!! \n dh, dm, ds: ");
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
                findLED (hourPos-1)->r = findLED(hourPos+1)->r = NeoPixel_gamma8(hourBrightness);

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
    const long playTimeMs = 200;
    const float dimmingTimeMs = playTimeMs / 2.0;
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
        uint8_t firstBrightness = sin_1_2 (static_cast<int>(deltaT/timeStep/(1-deltaT/(playTimeMs*2.1))
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

void backlightLEDs () {
    byte r, g, b;
    Wheel (eepromData.digitsColor, r, g, b);
    
    for (int i = 0; i < startingLEDs; i++) {
        _leds[i].r = r;
        _leds[i].g = g;
        _leds[i].b = b;
    }
}
    
void backlightLEDsEndingFunc (long dummy) {
    static Timer timer;

    if (modeChanger->err ()) {
        Serial.print (F("Error modechanger ")); Serial.println (modeChanger->err ());
    }
    else if (eepromData.currentClockFace != modeChanger->getCurrModeNumber ()) {
        clockFacesControlStruct.startMode = eepromData.currentClockFace = modeChanger->getCurrModeNumber ();
        //timer.setInterval (F("backlightLEDsEndingFunc, Timer for save mode in EEPROM"), 5000);
        timer.setInterval ("ms", 5000);
        timer.switchOn ();
    }

    if (timer.needToTrigger ()) {
        timer.switchOff ();
        Serial.println (F("Saving mode in EEPROM!"));
        writeEeprom ();
    }
    #ifdef MOSFET_LED 
      analogWrite(MOSFET_Pin, 255);
    #endif

    if ((now.minute () == 0) && (now.second () == 0)) modeChanger->changeCtlArray (&a12hourPartyControlStruct);
}


