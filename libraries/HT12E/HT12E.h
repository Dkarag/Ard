
/*---

 HT12E.h
 HT12E Support for Arduino
 Author: Marcelo Shiniti Uchimura
 Date  : May '08

 Note  : make sure HT12E is operating at 3~4kHz clock range

---*/

#ifndef HT12E_h
#define HT12E_h
#include "Arduino.h"

class HT12E 
{
 public:
                HT12E(int pin, unsigned int addrMask); // this is the constructor
   int          read();                                // this is the main method
 private:
   byte         _pin;      // this is Arduino input pin
   unsigned int _data;     // this is data
   unsigned int _mask;     // this is the address mask
   byte         _tries;    // this is how many times Arduino could find
                           // valid HT12E words
   unsigned long _dur;     // pulse duration
};

#endif