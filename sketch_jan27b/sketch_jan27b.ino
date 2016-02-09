 const int dataSize = 1300;  //Arduino memory is limited (max=1700)
 byte storedData[dataSize];  //Create an array to store the data
 #define ledPin 13           //Onboard LED = digital pin 13
 #define rfReceivePin A0     //RF Receiver data pin = Analog pin 0
 const unsigned int upperThreshold = 100;  //upper threshold value
 const unsigned int lowerThreshold = 80;  //lower threshold value
 int maxSignalLength = 255;   //Set the maximum length of the signal
 int dataCounter = 0;    //Variable to measure the length of the signal
 unsigned long startTime=0;  //Variable to record the start time 
 unsigned long endTime=0;    //Variable to record the end time 
 unsigned long signalDuration=0; //Variable to record signal reading time
 
void setup() {
    Serial.begin(9600);
  pinMode(ledPin, OUTPUT); 

}

void loop() {

for (int i=0;i<dataSize;i=i+2)
{
   while(analogRead(rfReceivePin)<1){
      //Wait here until a LOW signal is received
      startTime=micros();  //Update start time with every cycle.  
  }

    //Identify the length of the LOW signal---------------LOW
    dataCounter=0; //reset the counter
    while(analogRead(rfReceivePin)>upperThreshold && dataCounter<maxSignalLength){
      dataCounter++;
    }
    storedData[i]=dataCounter;
    
    //Identify the length of the HIGH signal---------------HIGH
    dataCounter=0;//reset the counter
    while(analogRead(rfReceivePin)<lowerThreshold && dataCounter<maxSignalLength){
      dataCounter++;
    }
    storedData[i+1]=dataCounter;
    
}
    
    
for (int i=0;i<dataSize;i++)
{
   for(int i=0; i<dataSize; i=i+2){
   
    Serial.print(storedData[i]);
    Serial.print(",");
     if(storedData[i+1]>12){
    Serial.println(storedData[i+1]);
    }
    else Serial.println(0);
}
}
}


