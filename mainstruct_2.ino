#include "modeChanger.h"
#include <FastLED.h>  
#include "DualFunctionButton.h"

#define startingLEDs 4 // Number of backlight LEDs BEFORE the strip
#define numLEDs 60 // Number of LEDs in strip
#define LEDOffset  30 // First LED in strip corresponds to 30-th second

struct CRGB _leds[startingLEDs+numLEDs];

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


int f1 (long dummy) {
  Serial.println ("\nMode: f1");
  return 0;
}

int f2 (long dummy) {
  Serial.println ("Mode: f2");
  return 0;
}

int f3 (long dummy) {
  Serial.println ("Mode: f3");
  return 0;
}

int (*modeFuncArray[])(long) = {fColorDemo1, fColorDemo2, fColorDemo3};
ModeChanger *mode = new ModeChanger (modeFuncArray, sizeof(modeFuncArray)/sizeof(modeFuncArray[0]));

void setup () {
  initDevices ();
  readEEPROM ();

  Serial.println ("\n\nStarting...");
}

int numPresses = 0;

void loop () {
  //static long _currentCallNumber = 0;
  //mode->applyMode (fColorDemo10sec);

  mode->loopThruModeFunc (100);

  if (button.shortPress()) {
      Serial.println (++numPresses);
      Serial.println ("mode->nextMode();");
      mode->nextMode();
  }
  if (button.longPress()) {
      Serial.println ("mode->applyMode (fColorDemo1);");
      mode->applyMode (fColorDemo1);
  }

  LEDS.show ();
}

void initDevices (void) {
    // Start LEDs
    LEDS.addLeds<WS2812B, LEDStripPin, GRB>(_leds, startingLEDs+numLEDs); 
    LEDS.clear(true);
    
    Serial.begin (9600);
}

void readEEPROM (void) {}

int fColorDemo1 (long currentCallNumber) {
  static unsigned long millisAtStart;
  
  //Serial.println ("Mode: fColorDemo1");
  //Serial.println (currentCallNumber);
  
  if (currentCallNumber == 0) {
      millisAtStart = millis ();
  }
  
  unsigned long deltaT = millis () - millisAtStart;
  unsigned long timeStep = 80;
  float ledBrightness = 0.8;
  int direction = 1;
  float wavelen = 1;

  if (deltaT >  3000) { wavelen = 1.1; };
  if (deltaT >  6000) { wavelen = 1.2; };
  if (deltaT >  9000) { wavelen = 1.4; timeStep = 40; };
  if (deltaT > 12000) { wavelen = 1.6; };
  if (deltaT > 15000) { wavelen = 0.8; direction = -1; };
  if (deltaT > 18000) { LEDS.clear (); return 1; };

//  Serial.print ("deltaT: ");   Serial.println (deltaT);

  for (int led = 0; led < numLEDs; led++) {
     uint8_t firstBrightness = gamma8(sine8(deltaT/timeStep-direction*led*wavelen)%256));
     findLED(led)->r = firstBrightness;
     //Serial.print (led); Serial.print (": "); Serial.println (firstBrightness); 
  }
  // delay (10000);
  return 0;
}

int fColorDemo2 (long currentCallNumber) {
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
  if (deltaT > 18000) { LEDS.clear (); return 1;};

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
  return 0;
}

int fColorDemo3 (long currentCallNumber) {
  static unsigned long millisAtStart;
  
  //Serial.println ("Mode: fColorDemo1");
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
  if (deltaT >  6000) {wavelen = 8; timeStep = 40; ledBrightness = 80;};
  if (deltaT >  9000) {wavelen = 6; timeStep = 40; ledBrightness = 120;};
  if (deltaT > 12000) wavelen = 4;
  if (deltaT > 15000) {wavelen = 4; ledBrightness = 255;};
  if (deltaT > 18000) { LEDS.clear (); return 1;};

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
  return 0;
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
