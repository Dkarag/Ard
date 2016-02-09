#include <HT12E.h>
#undef int
#undef abs
#undef double
#undef float
#undef round

HT12E remote(8, B0); // Arduino pin #7, HT12E's address 0111 1111b

void setup()
{
 Serial.begin(9600);
}

void loop()
{
 unsigned int valor;
 valor = remote.read();
 if(valor > 0xFFF0) Serial.println(valor, HEX); // error catching
 else
 {
   Serial.print("DATA ");
   Serial.println(valor, BIN); // data received from the remote control
 }
 delay(1000);
}
