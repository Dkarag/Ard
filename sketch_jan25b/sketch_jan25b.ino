

#define DATAPIN  2  // D3 is interrupt 1

void setup() {
 Serial.begin(9600);
}

void loop() {
  int i=0;
  static unsigned long duration = 0;
  static unsigned long lastTime = 0;
if (digitalRead(DATAPIN) == LOW)
{
  long time1 = micros();
  while(digitalRead(DATAPIN) == LOW )
  {
  //  i++; 
  }
  lastTime =micros();
  duration=(lastTime-time1)/1000;
 Serial.println(duration);
}


}
