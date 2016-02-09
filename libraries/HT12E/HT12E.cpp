/*---
 HT12E.cpp
 HT12E Support for Arduino
 Author: Marcelo Shiniti Uchimura
 Date  : May '08
---*/

#include "Arduino.h"
#include "HT12E.h"

HT12E::HT12E(int pin, unsigned int addrMask)
{
 _pin = pin;
 pinMode(_pin, INPUT);
 _data = 0;
 _mask = addrMask << 4;  // the HT12E basic word is a stream with an 8-bit address
                         // followed by 4-bit data. I left shift the
                         // address mask 4 bits so I can match it to the entire word
}

int HT12E::read()
{
 byte ctr;            // for general error handling
 _tries = 0;
 do
 {
   /* look for HT12E basic word's pilot stream */
   for(ctr = 0; ctr < 13; ++ctr)
   {
     while(digitalRead(_pin) == LOW);                // wait for the signal to go HIGH
     _dur = pulseIn(_pin, LOW);

     if(_dur > 9000 && _dur < 12000) break;          // 36x(clock tick interval)
   }

   /* if error, skip everything */
   if(ctr == 13)
     {
       _tries = 4;
       break;
     }

   /* now wait until sync bit is gone */
   for(ctr = 0; ctr < 6; ++ctr)
     {
       if(digitalRead(_pin) == LOW) break;
       delayMicroseconds(80);
   }

     /* if error, skip everything */
     if(ctr == 6)
     {
       _tries = 5;
       break;
     }

   /* let's get the address+data bits now */
   for(_data = 0, ctr = 0; ctr < 12; ++ctr)
   {
     _dur = pulseIn(_pin, HIGH);
     if(_dur > 250 && _dur < 333)        // if pulse width is between 1/4000 and 1/3000 secs
     {
       _data = (_data << 1) + 1;         // attach a *1* to the rightmost end of the buffer
     }
     else if(_dur > 500 && _dur < 666)   // if pulse width is between 2/4000 and 2/3000 secs
     {
       _data = (_data << 1);             // attach a *0* to the rightmost end of the buffer
     }
     else
     {
       /* force loop termination */
       _data = 0;
           break;
     }
   }

   // check if buffered data matches the address mask
   if((_data & _mask) < _mask)
   {
     /* data error */
     _tries = 6;
   }
   else ++_tries;

 } while(_tries < 3);

 if(_tries > 3) // error handling
 {
   switch(_tries)
     {
       case 4: return 0xffff;
       case 5: return 0xfffe;
       case 6: return 0xfffd;
   }
 }
 return (_data ^ _mask);
}