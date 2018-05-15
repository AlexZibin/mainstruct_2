////////////////////////////////////////////////////////////////////////////////////////////
//   CLOCK DISPLAY MODES
// Add any new display mode functions here. Then add to the "void timeDisplay(DateTime now)" function.
// Add each of the new display mode functions as a new "case", leaving default last.
////////////////////////////////////////////////////////////////////////////////////////////

extern DateTime now;
{minimalClock, basicClock, smoothSecond, 

//
void minimalClock (long currentCallNumber) {
  uint8_t hourPos = _hourPos (now.hour(), now.minute());
  
  findLED(hourPos)->r = 255;
  findLED(now.minute())->g = 255;
  findLED(now.second())->b = 255;
}

//
void basicClock(long currentCallNumber) {
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
}

//////////////////////////////////////////////////////////////////////////////////////////
void smoothSecond (long currentCallNumber) {
    static unsigned long millisAtStart;
    static DateTime oldTime;
    static bool catchSeconds;
    unsigned long deltaT;
    uint8_t secondBrightness1, secondBrightness2;
  
    if (currentCallNumber == 0) {
        //
        oldTime = RTC.now();
        catchSeconds = false;
    }
    
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
        deltaT = millis () - millisAtStart;
    }

    if (now.second()!=old.second()) {
      old = now;
      cyclesPerSec = millis() - newSecTime;
      cyclesPerSecFloat = (float) cyclesPerSec;
      newSecTime = millis();      
    } 
    // set hour, min & sec LEDs
    fracOfSec = (millis() - newSecTime)/cyclesPerSecFloat;  // This divides by 733, but should be 1000 and not sure why???
    if (subSeconds < cyclesPerSec) {secondBrightness = 50.0*(1.0+sin((3.14*fracOfSec)-1.57));}
    if (subSeconds < cyclesPerSec) {secondBrightness2 = 50.0*(1.0+sin((3.14*fracOfSec)+1.57));}

    uint8_t hourPos = _hourPos (now.hour(), now.minute());
    // The colours are set last, so if on same LED mixed colours are created
    // Hour (2 lines of code)
          findLED(hourPos-1)->r = findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r = 170;

    // Minute  
          findLED(now.minute())->g = 255;

    // Second  
          findLED(now.second ())->b = secondBrightness1;
          findLED(now.second () + 1)->b = secondBrightness2;
}

//
void outlineClock(DateTime now)
{
  for (int i = 0; i < numLEDs; i+= numLEDs/12) {
      findLED(i)->r = 100;
      findLED(i)->g = 100;
      findLED(i)->b = 100;
  }
  uint8_t hourPos = _hourPos (now.hour(), now.minute());

  // Hour (3 lines of code)
          findLED(hourPos-1)->r = 30;
          findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
    
  // Second  
          findLED(now.second())->b = 255;
}

//
void minimalMilliSec(DateTime now)
{
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
}

// Pendulum will be at the bottom and left for one second and right for one second
void simplePendulum(DateTime now)
{
  if (now.second()!=old.second()) {
      old = now;
      cyclesPerSec = millis() - newSecTime;
      cyclesPerSecFloat = (float) cyclesPerSec;
      newSecTime = millis();
      if (swingBack == true) {
        swingBack = false;
      } else {
        swingBack = true;
      }
  } 
    
  // set hour, min & sec LEDs
  fracOfSec = (millis() - newSecTime)/cyclesPerSecFloat;  // This divides by 733, but should be 1000 and not sure why???
  if (subSeconds < cyclesPerSec && swingBack == true) {pendulumPos = 27.0 + 3.4*(1.0+sin((3.14*fracOfSec)-1.57));}
  if (subSeconds < cyclesPerSec && swingBack == false) {pendulumPos = 27.0 + 3.4*(1.0+sin((3.14*fracOfSec)+1.57));}

  uint8_t hourPos = _hourPos (now.hour(), now.minute());
    
  // Pendulum lights are set first, so hour/min/sec lights override and don't flicker as millisec passes
  findLED(pendulumPos)->r = 100;
  findLED(pendulumPos)->g = 100;
  findLED(pendulumPos)->b = 100;
    
  // The colours are set last, so if on same LED mixed colours are created
  // Hour (3 lines of code)
          findLED(hourPos-1)->r = 30;
          findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
    
  // Second  
          findLED(now.second())->b = 255;
}

void breathingClock(DateTime now) {
  if (alarmTrig == false) {
      breathBrightness = 15.0*(1.0+sin((3.14*(millis()%86400000)/2000.0)-1.57));
      for (int i = 0; i < numLEDs; i++) {
          fiveMins = i%5;
          if (fiveMins == 0) {
              findLED(i)->r = breathBrightness + 5;
              findLED(i)->g = breathBrightness + 5;
              findLED(i)->b = breathBrightness + 5;
          } else {
              findLED(i)->r = 0;
              findLED(i)->g = 0;
              findLED(i)->b = 0;
          }
       }
  }
  
  uint8_t hourPos = _hourPos (now.hour(), now.minute());

  // Hour (3 lines of code)
          findLED(hourPos-1)->r = 30;
          findLED(hourPos+1)->r = 30;
          findLED(hourPos)->r  = 190;
  
  // Minute  
          findLED(now.minute())->g = 255;
    
  // Second  
          findLED(now.second())->b = 255;
}
