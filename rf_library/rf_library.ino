#include <RF433.h>
int RFPIN=2;
RF433 sensor=RF433(RFPIN) ;
int i=0;
void setup() {
  Serial.begin(9600);
  pinMode(RFPIN, INPUT);
  sensor.enableRF();
}

void loop() {
 // float temp;
 
  if(sensor.received==true)
  {
 // temp = sensor.getTemperature();
  Serial.print(i);
  Serial.print(" :=====> outside Temperature is:");
  Serial.println( sensor.getTemperature());

  i++;
   }
  sensor.received=false;
}
