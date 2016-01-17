/*
  MyLIB.h - My first Library 
*/
#ifndef MyLIB_h
#define MyLIB_h

#include "Arduino.h"
#include <OneWire.h>


class Sensor
{
  public:
    void SensorPin(int pin);
    float getTemp();
  private:
    float temperature;
    int _pin;
	byte i;
	byte present = 0;
	byte data[12];
    byte addr[8];
    int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;	
};

#endif