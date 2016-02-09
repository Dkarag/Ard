int State = 0;
int lastState = 0;
  long duration = 0;
long lastTime =0;
void setup() {
 Serial.begin(9600);
  Serial.println("Started.");
  pinMode(6, INPUT);
  int i=0;
}

void loop() {
  int i=0;

  do 
  {
   State = digitalRead(6);
   if (State != lastState) {

    long time = millis();
     duration = time - lastTime;
     lastTime = time;
     lastState = State;
   }
  }while(duration>80);
  // if (duration>8)
   
   Serial.println(duration);
   
}
