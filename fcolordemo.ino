////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
returnValue fColorDemo2 (long currentCallNumber) {
    static unsigned long millisAtStart;
    const long playTimeMs = 18000;

    if (currentCallNumber == 0) {
      millisAtStart = millis ();
      for (int led = 0; led < numLEDs; led++) { // Don't erase starting LEDs (digits 3-6-9-12)
          findLED(led)->r = 0;
          findLED(led)->g = 0;
          findLED(led)->b = 0;
      }
    }

    unsigned long deltaT = millis () - millisAtStart;
    unsigned long timeStep = 5;
    int direction = -1;
    float wavelen = 10.0;

    #ifdef MOSFET_LED 
      uint8_t mosBrt = sine8_0 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*1.9)))%256);
      //Serial.print ("mosBrt = "); Serial.println (mosBrt); // Bug catch for MOSFET_LED
      analogWrite(MOSFET_Pin, mosBrt);
    #endif

    if (deltaT > playTimeMs) { LEDS.clear (); return returnValue::NEXT; };

    static float ledBrightness;
    byte r, g, b;
    Wheel ((deltaT/15)%384, r, g, b);
    for (int led = 0; led < numLEDs; led++) {
        uint8_t firstBrightness = sine8_0 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*2.1))
                                                                -direction*led*wavelen*(1+deltaT/(playTimeMs+4000.0)))%256);
        
        // dimming at the beginning of demo and in the end:
        const float dimmingTimeMs = 3000.0;
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

    //byte r, g, b;
    static float ledBrightness2;
    if (deltaT <= 2000.0) ledBrightness2 = deltaT/2000.0;
    Wheel ((deltaT/25)%384, r, g, b);
    for (int i = 0; i < startingLEDs; i++) {
          _leds[i].r = r*2*ledBrightness2;
          _leds[i].g = g*2*ledBrightness2;
          _leds[i].b = b*2*ledBrightness2;
    }

    return returnValue::CONTINUE;
}

returnValue fColorDemo1 (long currentCallNumber) {
    static unsigned long millisAtStart;
    const long playTimeMs = 18000;

    //Serial.println ("Mode: fColorDemo1");
    //Serial.println (currentCallNumber);

    if (currentCallNumber == 0) {
        millisAtStart = millis ();
        LEDS.clear(true);
    }

    unsigned long deltaT = millis () - millisAtStart;
    unsigned long timeStep = 5;
    int direction = 1;
    float wavelen = 10.0;

    /*if (deltaT >  3000) { wavelen = 20; timeStep =  4; };
    if (deltaT >  6000) { wavelen = 20; timeStep =  3; };
    if (deltaT >  9000) { wavelen = 20; timeStep =  2; };
    if (deltaT > 12000) { wavelen = 20; timeStep =  1; };
    if (deltaT > 15000) { wavelen = 20; direction = -1; };*/
    if (deltaT > playTimeMs) { /*LEDS.clear (); */return returnValue::NEXT; };

    //Serial.print ("deltaT: ");   Serial.println (deltaT);

    static float ledBrightness;
    for (int led = 0; led < numLEDs; led++) {
        uint8_t firstBrightness = sin_1_2 (static_cast<int>(deltaT/timeStep/(1-deltaT/(playTimeMs*3.1))-direction*led*wavelen*(1+deltaT/(playTimeMs*1.3)))%256);

        // dimming at the beginning of demo and in the end:
        const float dimmingTimeMs = 3000.0;
        if (deltaT <= dimmingTimeMs) ledBrightness = deltaT/dimmingTimeMs;
        if (deltaT >= playTimeMs-dimmingTimeMs) ledBrightness = (playTimeMs-deltaT)/dimmingTimeMs;

        const float shade = 5;
        if (led < shade) {      
            findLED(led)->b = NeoPixel_gamma8(firstBrightness*(led/shade)*ledBrightness);
            //findLED(led)->b = NeoPixel_gamma8(firstBrightnessB*(led/shade)/4);
        } else if (led >= numLEDs - shade) {      
            findLED(led)->b = NeoPixel_gamma8(firstBrightness*((numLEDs-led)/shade)*ledBrightness);
            //findLED(led)->b = NeoPixel_gamma8(firstBrightnessB*((numLEDs-led)/shade)/4);
        } else
            findLED(led)->b = NeoPixel_gamma8(firstBrightness*ledBrightness);
            //findLED(led)->b = NeoPixel_gamma8(firstBrightnessB/4);
            //Serial.print (led); Serial.print (": "); Serial.println (firstBrightness); 
    }

    byte r, g, b;
    static float ledBrightness2;
    if (deltaT <= 6000.0) ledBrightness2 = deltaT/6000.0;
    Wheel ((deltaT/15)%384, r, g, b);
    for (int i = 0; i < startingLEDs; i++) {
          _leds[i].r = r*2*ledBrightness2;
          _leds[i].g = g*2*ledBrightness2;
          _leds[i].b = b*2*ledBrightness2;
    }

    return returnValue::CONTINUE;
}

/*
returnValue fColorDemo4 (long currentCallNumber) {
  static unsigned long millisAtStart;
  
  //Serial.println ("Mode: fColorDemo2");
  //Serial.println (currentCallNumber);
  
  if (currentCallNumber == 0) {
      millisAtStart = millis ();
  }
  
  unsigned long deltaT = millis () - millisAtStart;
  int timeStep = 80;
  int ledBrightness = 20;
  int direction = -1;
  int wavelen = 12;

  if (deltaT >  3000) wavelen = 10;
  if (deltaT >  6000) wavelen = 8;
  if (deltaT >  9000) {wavelen = 6; timeStep = 40; ledBrightness += 80;};
  if (deltaT > 12000) wavelen = 4;
  if (deltaT > 15000) {wavelen = 4; ledBrightness = 255;};
  if (deltaT > 18000) { LEDS.clear (); return returnValue::NEXT; };

//  Serial.print ("deltaT: ");   Serial.println (deltaT);

  for (int led = 0; led < numLEDs; led++) {
      // =ОСТАТ(время/timeStep1-направление*диод; waveLen)<1
   //Serial.print ("deltaT/timeStep-direction*led)%wavelen: ");   Serial.println ((deltaT/timeStep-direction*led)%wavelen);

     if ((deltaT/timeStep-direction*led)%wavelen < 1) {
          // switch this led on
          // Хорошо бы и предыдущий led несильно зажигать, чтобы выглядело как затухающий след 
          // И встречную волну пустить другого цвета
          //findLED(led-direction)->b = 25;
          findLED(led)->g = ledBrightness;
      } else {
          // switch this led off
          findLED(led)->r = 0;
          findLED(led)->g = 0;
          findLED(led)->b = 0;
          //findLED(led-1)->b = 000;
      }
  }
  return returnValue::CONTINUE;
}

returnValue fColorDemo3 (long currentCallNumber) {
  static unsigned long millisAtStart;
  
  //Serial.println ("Mode: fColorDemo3");
  //Serial.println (currentCallNumber);
  
  if (currentCallNumber == 0) {
      millisAtStart = millis ();
  }
  
  unsigned long deltaT = millis () - millisAtStart;
  int timeStep = 80;
  int ledBrightness = 10;
  int direction = 1;
  int wavelen = 12;

  if (deltaT >  3000) wavelen = 10;
  if (deltaT >  6000) { wavelen = 8; timeStep = 40; ledBrightness = 80; };
  if (deltaT >  9000) { wavelen = 6; timeStep = 40; ledBrightness = 120; };
  if (deltaT > 12000) wavelen = 4;
  if (deltaT > 15000) { wavelen = 4; ledBrightness = 255; };
  if (deltaT > 18000) { LEDS.clear (); return returnValue::NEXT; };

//  Serial.print ("deltaT: ");   Serial.println (deltaT);

  for (int led = 0; led < numLEDs; led++) {
      // =ОСТАТ(время/timeStep1-направление*диод; waveLen)<1
   //Serial.print ("deltaT/timeStep-direction*led)%wavelen: ");   Serial.println ((deltaT/timeStep-direction*led)%wavelen);

     if ((deltaT/timeStep-direction*led)%wavelen < 1) {
          // switch this led on
          // Хорошо бы и предыдущий led несильно зажигать, чтобы выглядело как затухающий след 
          // И встречную волну пустить другого цвета
          //findLED(led-direction)->b = 25;
          findLED(led)->b = ledBrightness;
      } else {
          // switch this led off
          findLED(led)->r = 0;
          findLED(led)->g = 0;
          findLED(led)->b = 0;
          //findLED(led-1)->b = 000;
      }
  }
  return returnValue::CONTINUE;
}
*/

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r
void Wheel (uint16_t WheelPos, byte &r, byte &g, byte &b) {
  switch(WheelPos / 128) {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
}

