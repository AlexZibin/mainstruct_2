#include "modeChanger.h"
#include <FastLED.h>  
#include "DualFunctionButton.h"
#include <Wire.h>  // I2C: On Arduino Uno & Nano & ProMini use pins A4 for SDA (yellow/orange) and A5 for SCL (green). For other boards ee http://arduino.cc/en/Reference/Wire
#include <RTClib.h>           // Include the RTClib library to enable communication with the real time clock.
#include <Encoder.h>          // Include the Encoder library to read the outputs of the rotary encoders
#include <EEPROMex.h>         // Extended EEPROM library / Copyright (c) 2012 Thijs Elenbaas.  All right reserved.

// Pin definitions:
//    #define ARDUINO_NANO

    #ifdef ARDUINO_NANO
          //#define reedSwitchPin 3 // Switches off the display to reduce power consumption (before re-flashing)
          #define rotaryLeft 3
          #define rotaryRight 4
          #define LEDStripPin 9 // Data pin
          #define menuPin 7 // 
          #define MOSFET_Pin 9 
        // Arduino Nano i2c: SDA = A4, SCL = A5.    
    #endif
    
    #ifndef ARDUINO_NANO
          #define menuPin 2 // Arduino Pro Mini supports external interrupts only on pins 2 and 3
//          #define reedSwitchPin 3 // Switches off the display to reduce power consumption (before re-flashing)
          #define LEDStripPin 3 // Data pin
          #define rotaryLeft 4
          #define rotaryRight 5
          #define MOSFET_Pin 9 
        // Arduino Pro Mini i2c: SDA = A4, SCL = A5.    
    #endif

// No leading 4 leds, only mosfet thru pin9
//#define MOSFET_LED 
#ifdef MOSFET_LED 
  #define startingLEDs 0 // Number of backlight LEDs BEFORE the strip
#endif
#ifndef MOSFET_LED 
  #define startingLEDs 4 // Number of backlight LEDs BEFORE the strip
#endif

#define numLEDs 60 // Number of LEDs in strip
#define LEDOffset  30 // First LED in strip corresponds to 30-th second

struct CRGB _leds[startingLEDs+numLEDs];

Encoder rotary1 (rotaryLeft, rotaryRight); // Setting up the Rotary Encoder
#define ROTARY_TICKS 4

RTC_DS1307 RTC; // Establishes the chipset of the Real Time Clock

DualFunctionButton button (menuPin, 1000, INPUT_PULLUP);

extern uint8_t sin_1_2 (int x);
extern uint8_t NeoPixel_sine8 (int x);
extern uint8_t sine8_0 (int x); // starts from 0, not from 128 as NeoPixel_sine8
extern uint8_t NeoPixel_gamma8 (int x);
extern uint8_t squareSine8 (int x);

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

extern ControlStruct clockFacesControlStruct;
extern ControlStruct energySaverControlStruct;
/////// shortIntroControlStruct ///////
returnValue (*introFuncArray[])(long) = {fColorDemo1, fColorDemo2};
const int len_introFuncArray = sizeof(introFuncArray)/sizeof(introFuncArray[0]);
ControlStruct shortIntroControlStruct {introFuncArray, len_introFuncArray, nullptr, 
                                       LoopMode::ONCE, &clockFacesControlStruct, &energySaverControlStruct, 0};

/////// energySaverControlStruct ///////
returnValue (*energySaverFuncArray[])(long) = {energySaver};
const int len_energySaverFuncArray = sizeof(energySaverFuncArray)/sizeof(energySaverFuncArray[0]);
ControlStruct energySaverControlStruct {energySaverFuncArray, len_energySaverFuncArray, nullptr, 
                                       LoopMode::ONCE, &shortIntroControlStruct, &shortIntroControlStruct, 0};

returnValue energySaver (long currentCallNumber) {
    LEDS.clear ();
    findLED(0)->b = NeoPixel_gamma8 (sin_1_2 ((millis()/10)%256)/3);
    //findLED(led)->r = findLED(led)->g = findLED(led)->b = NeoPixel_gamma8 (sine8_0 ((millis()/5)%256)/2);
    
    #ifdef MOSFET_LED 
      analogWrite(MOSFET_Pin, 0);
    #endif

    return returnValue::CONTINUE;
}

ModeChanger *modeChanger;

void setup () {
    initDevices ();
    readEEPROM ();
    
    Serial.println ("\n\nStarting...");
    modeChanger = new ModeChanger (&shortIntroControlStruct);
}

int num = 0;

DateTime now;

void loop () {
    now = RTC.now();
    LEDS.clear ();
    handleUnlockCode ();
    modeChanger->loopThruModeFunc ();
    LEDS.show ();
    adjustSeconds ();
}

void initDevices (void) {
    // Start LEDs
    LEDS.addLeds<WS2812B, LEDStripPin, GRB>(_leds, startingLEDs+numLEDs); 
    LEDS.clear(true);
    
    Serial.begin (9600);
    EEPROM.setMaxAllowedWrites (200);

    // Start RTC
    Wire.begin (); // Arduino Pro Mini i2c: SDA = A4, SCL = A5.
    if (!RTC.begin ()) { // Starts communications to the RTC
        Serial.println(F("Couldn't find RTC"));
    }
    if (!RTC.isrunning ()) {
        Serial.println(F("RTC is NOT running!"));
        RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    //RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    DateTime now = RTC.now();
    Serial.print(F("Hour time is... "));
    Serial.println(now.hour());
    Serial.print(F("Min time is... "));
    Serial.println(now.minute());
    Serial.print(F("Sec time is... "));
    Serial.println(now.second());
    
    Serial.print(F("Year is... "));
    Serial.println(now.year());
    Serial.print(F("Month is... "));
    Serial.println(now.month());
    Serial.print(F("Day is... "));
    Serial.println(now.day());
}


/////////////////////////////////////// EEPROM
const uint16_t correctMagicValue = 0xEFD3;
const int correctUnlockCode[] = {2, 2}; 
const int unlockCodeLen = sizeof(correctUnlockCode)/sizeof(correctUnlockCode[0]);
const int correctTotalUnlockCode[unlockCodeLen] = {0, 3}; // First "0" stands for totally unlocked clock

struct EEPROMdata {
    uint16_t magicValue;
    int currentClockFace;
    
    DateTime lastClockManualCorrectionTime;
    long clockCorrectionSecInterval;
    int clockCorrectionDirection;
    
    int dayBrightness;
    int nightBrightness;
    int unlockCode[unlockCodeLen];
    int totalUnlockCode[unlockCodeLen];
    int remainingUnlockEfforts;
};

EEPROMdata eepromData;
void readEEPROM (void) {
    EEPROM.readBlock <EEPROMdata> (0, eepromData);
    
    if (eepromData.magicValue != correctMagicValue) { // EEPROM is blank
        Serial.println (F("EEPROM is blank!"));
        eepromData.magicValue = correctMagicValue;
        eepromData.currentClockFace = 0;
        
        //eepromData.lastClockCorrectionTime = now ();
        eepromData.clockCorrectionSecInterval = 0L; // 0 stands for "no correction ever made"
        for (int i=0; i < unlockCodeLen; i++) {
            eepromData.unlockCode[i] = correctUnlockCode[i];
            eepromData.totalUnlockCode[i] = correctTotalUnlockCode[i];
        }
        eepromData.remainingUnlockEfforts = 5;
    }
    Serial.print (F("eepromData.remainingUnlockEfforts = ")); Serial.println (eepromData.remainingUnlockEfforts);
    Serial.print (F("clockCorrectionSecInterval = ")); Serial.println (eepromData.clockCorrectionSecInterval);
    clockFacesControlStruct.startMode = eepromData.currentClockFace;
    Serial.print (F("clockFacesControlStruct.startMode = ")); Serial.println (clockFacesControlStruct.startMode);
}

void writeEeprom (void) {
    EEPROM.writeBlock <EEPROMdata> (0, eepromData);
}

bool rotaryTurnLeft (void) {
  static long lastRotary = 0;
  int rotary1Pos = rotary1.read(); // Checks the rotary position

  if (rotary1Pos <= -ROTARY_TICKS && (millis() - lastRotary) >= 200) {
      rotary1.write(0);
      lastRotary = millis();
      return true;
  } 
  return false;
}

bool rotaryTurnRight (void) {
  static long lastRotary = 0;
  int rotary1Pos = rotary1.read(); // Checks the rotary position

  if (rotary1Pos >= ROTARY_TICKS && (millis() - lastRotary) >= 200) {
      rotary1.write(0);
      lastRotary = millis();
      return true;
  } 
  return false;
}

// If, for example, DS1307 loses 24 seconds per day, 
// then we manually add 1 second 24 times a day, evenly distributed
void adjustSeconds (void) {
    if (eepromData.clockCorrectionSecInterval != 0) {
        static DateTime adjustAtThisSecond (2000, 1, 1);

        if (adjustAtThisSecond.year () == 2000) { 
            adjustAtThisSecond = DateTime (now.year (), now.month (), now.day (), 0, 0, 0);
            while (adjustAtThisSecond < RTC.now ())
                  adjustAtThisSecond = adjustAtThisSecond + TimeSpan (eepromData.clockCorrectionSecInterval);
            showDate("adjustAtThisSecond: ", adjustAtThisSecond); //take code from DateTime timespan example
        }
        int s = now.second ();
        if ((adjustAtThisSecond <= RTC.now ()) && (s > 0) && (s < 59)) {
            adjustAtThisSecond = adjustAtThisSecond + TimeSpan (eepromData.clockCorrectionSecInterval);
            showDate("NEW adjustAtThisSecond: ", adjustAtThisSecond); 
            
            RTC.adjust (DateTime (now.year (), now.month (), now.day (), 
                                  now.hour (), now.minute (), eepromData.clockCorrectionDirection + s));
        }
    }
}

void showDate(const char* txt, const DateTime& dt) {
    Serial.print(txt);
    Serial.print(' ');
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);
    
    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.print("s / ");
    Serial.print(dt.unixtime() / 86400L);
    Serial.print(F("d since 1970"));
    
    Serial.println();
}

void handleUnlockCode (void) {
    if (correctTotalUnlockCode[0] != 0) { // First "0" stands for totally unlocked clock
    }
}

