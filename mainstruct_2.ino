#include "modeChanger.h"
#include <FastLED.h>  
#include "DualFunctionButton.h"
#include <Wire.h>  // I2C: On Arduino Uno & Nano & ProMini use pins A4 for SDA (yellow/orange) and A5 for SCL (green). For other boards ee http://arduino.cc/en/Reference/Wire
#include <RTClib.h>           // Include the RTClib library to enable communication with the real time clock.
#include <Encoder.h>          // Include the Encoder library to read the outputs of the rotary encoders

// Pin definitions:
//    #define ARDUINO_NANO
    #ifdef ARDUINO_NANO
          #define rotaryLeft 3
          #define rotaryRight 4
          #define LEDStripPin 9 // Data pin
          #define menuPin 7 // 
          //#define reedSwitchPin 3 // Switches off the display to reduce power consumption (before re-flashing)
        // Arduino Nano i2c: SDA = A4, SCL = A5.    
    #endif
    
    #ifndef ARDUINO_NANO
          #define rotaryLeft 4
          #define rotaryRight 5
          #define LEDStripPin 3 // Data pin
          #define menuPin 2 // Arduino Pro Mini supports external interrupts only on pins 2 and 3
          #define reedSwitchPin 3 // Switches off the display to reduce power consumption (before re-flashing)
        // Arduino Pro Mini i2c: SDA = A4, SCL = A5.    
    #endif

#define startingLEDs 4 // Number of backlight LEDs BEFORE the strip
#define numLEDs 60 // Number of LEDs in strip
#define LEDOffset  30 // First LED in strip corresponds to 30-th second

struct CRGB _leds[startingLEDs+numLEDs];

Encoder rotary1 (rotaryLeft, rotaryRight); // Setting up the Rotary Encoder
#define ROTARY_TICKS 4

RTC_DS1307 RTC; // Establishes the chipset of the Real Time Clock

DualFunctionButton button(menuPin, 1000, INPUT_PULLUP);

// CONVERSIONS
        // Which LED (in [0..59] range) corresponds to given hh:mm time 
        uint8_t _hourPos (int hour, int minute) { 
          while (hour < 0) { // Foolproof check :)
              hour += 12;
          }
          while (minute < 0) {  // Foolproof check :)
              minute += numLEDs;
          }
          uint8_t _hp = (hour%12)*5 + (minute+6)/12;
          return (_hp == 60) ? 0 : _hp;
        }

        // Which offset (in [4..63] range) corresponds to given [0..59] mm time 
        // First [0..3] offsets are reserved for backlight LEDs (==startingLEDs)
        struct CRGB* findLED (int minute) {
          while (minute < 0) {  // Foolproof check :)
              minute += numLEDs;
          }
          return &_leds[(LEDOffset+minute)%numLEDs+startingLEDs];
        }
// END CONVERSIONS

returnValue (*modeFuncArray[])(long) = {fColorDemo1, fColorDemo2};
const int len_modeFuncArray = sizeof(modeFuncArray)/sizeof(modeFuncArray[0]);
//ModeChanger *intro = new ModeChanger (modeFuncArray, len_modeFuncArray);

void setup () {
  initDevices ();
  readEEPROM ();

  Serial.println ("\n\nStarting...");
}

int numPresses = 0;

DateTime now;

void loop () {
    now = RTC.now();
    
    ModeChanger modeChanger (modeFuncArray, len_modeFuncArray);
    modeChanger.loopThruModeFunc (30);
    
    /*
    clockFaces.applyCurrFunc();
    if (rotaryTurnLeft) clockFaces.prevMode();
    if (rotaryTurnLeft) clockFaces.nextMode();
    if (button.longPress() clockFaces.changeCtlArray (_adjustTime);
        To-DO:  1) Add function pointer to afterLoopCtlFunction to ModeChanger class.
                This function will handle button presses and encoder specifically.
                2) Add ModeChanger::changeCtlArray (): _fptr = newFunc.
    */

    if (button.shortPress()) {
        Serial.println (++numPresses);
        Serial.println ("intro->nextMode();");
        intro->nextMode();
    }
    if (button.longPress()) {
        Serial.println ("intro->applyMode (fColorDemo1);");
        intro->applyMode (fColorDemo1);
    }
    //backlightLEDs ();
    LEDS.show ();
}

void initDevices (void) {
    // Start LEDs
    LEDS.addLeds<WS2812B, LEDStripPin, GRB>(_leds, startingLEDs+numLEDs); 
    LEDS.clear(true);
    
    Serial.begin (9600);

    // Start RTC
    Wire.begin(); // Arduino Pro Mini i2c: SDA = A4, SCL = A5.
    if (!RTC.begin()) { // Starts communications to the RTC
          Serial.println("Couldn't find RTC");
    }
    if (!RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now = RTC.now();
    Serial.print("Hour time is... ");
    Serial.println(now.hour());
    Serial.print("Min time is... ");
    Serial.println(now.minute());
    Serial.print("Sec time is... ");
    Serial.println(now.second());
    
    Serial.print("Year is... ");
    Serial.println(now.year());
    Serial.print("Month is... ");
    Serial.println(now.month());
    Serial.print("Day is... ");
    Serial.println(now.day());
}

void readEEPROM (void) {}

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

    if (deltaT > playTimeMs) { LEDS.clear (); return returnValue::NEXT; };

    static float ledBrightness;
    byte r, g, b;
    Wheel ((deltaT/15)%384, r, g, b);
    for (int led = 0; led < numLEDs; led++) {
        uint8_t firstBrightness = sine8_0 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*2.1))-direction*led*wavelen*(1+deltaT/(playTimeMs+4000.0)))%256);
        
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
        uint8_t firstBrightness = sine8_0 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*3.1))-direction*led*wavelen*(1+deltaT/(playTimeMs*1.3)))%256);
        //uint8_t firstBrightnessB = sine8_0 (static_cast<uint8_t>(deltaT/timeStep/(1-deltaT/(playTimeMs*4.1))+direction*led*wavelen*(1-deltaT/76000.0))%256);

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
    if (deltaT <= 16000.0) ledBrightness2 = deltaT/16000.0;
    Wheel ((deltaT/15)%384, r, g, b);
    for (int i = 0; i < startingLEDs; i++) {
          _leds[i].r = r*2*ledBrightness2;
          _leds[i].g = g*2*ledBrightness2;
          _leds[i].b = b*2*ledBrightness2;
    }

    return returnValue::CONTINUE;
}

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

void backlightLEDs (void) {
  for (int i = 0; i < startingLEDs; i++) {
    _leds[i].g = 5;
    _leds[i].r = 5;
    _leds[i].b = 255;
  }
}

bool rotaryTurnLeft (void) {
  static long lastRotary = 0;
  int rotary1Pos = rotary1.read(); // Checks the rotary position

  if (rotary1Pos <= -ROTARY_TICKS && (millis() - lastRotary) >= 1000) {
      rotary1.write(0);
      lastRotary = millis();
  } 
}

bool rotaryTurnRight (void) {
  static long lastRotary = 0;
  int rotary1Pos = rotary1.read(); // Checks the rotary position

  if (rotary1Pos >= ROTARY_TICKS && (millis() - lastRotary) >= 1000) {
      rotary1.write(0);
      lastRotary = millis();
  } 
}

