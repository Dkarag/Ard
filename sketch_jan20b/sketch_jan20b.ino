/* 
  Transmit sketch - RF Calibration
     Written by ScottC 17 July 2014
     Arduino IDE version 1.0.5
     Website: http://arduinobasics.blogspot.com
     Transmitter: FS1000A/XY-FST
     Description: A simple sketch used to calibrate RF transmission.          
 ------------------------------------------------------------- */

 #define rfTransmitPin 4  //RF Transmitter pin = digital pin 4
 #define ledPin 13        //Onboard LED = digital pin 13
 
 const int codeSize = 33;      //The size of the code to transmit
 int codeToTransmit[codeSize]; //The array used to hold the RF code
 int lightON[]={1,2,2,2,2,2,2,1,2,1,1,1,1,1,1,1,2,1,2,1,1,2,1,2,2,2,2,1,1,1,1,1,3};
 //int lightON[]={2}; //The RF code that will turn the light ON
 // int lightOFF[]={1,2,2,2,2,2,2,1,2,1,1,1,1,1,1,1,2,1,2,1,1,2,1,2,2,2,2,1,1,1,1,1,3};//16.5
    int lightOFF[]={1,2,2,2,2,2,2,1,2,1,1,1,1,1,1,1,1,2,2,2,1,1,2,2,1,2,2,2,2,2,2,1,3};//11.5
 int codeToggler = 0;  //Used to switch between turning the light ON and OFF
 int timeDelay=100;      // The variable used to calibrate the RF signal lengths.

 
 
 void setup(){
   Serial.begin(9600);        // Turn the Serial Protocol ON
   pinMode(rfTransmitPin, OUTPUT);   //Transmit pin is an output  
   pinMode(ledPin, OUTPUT);          
  
 //LED initialisation sequence - gives us some time to get ready
  digitalWrite(ledPin, HIGH); 
  delay(1000);
  digitalWrite(ledPin, LOW); 
  delay(500);
 }
 
 
 
  void loop(){
    toggleCode();    // switch between light ON and light OFF
    transmitCode();  // transmit the code to RF receiver on the Fan/Light
    
   // timeDelay+=1;    //Increment the timeDelay by 10 microseconds with every transmission
   delay(500);      //Each transmission will be about 2 seconds apart.
    
  Serial.print(timeDelay);
  }
  
  
  
  
  /*---------------------------------------------------------------- 
     toggleCode(): This is used to toggle the code for turning 
                   the light ON and OFF 
  -----------------------------------------------------------------*/
  void toggleCode(){
    if(codeToggler){
       for(int i = 0; i<codeSize; i++){
         codeToTransmit[i]=lightON[i];
       } 
      
    } else{
      for(int i = 0; i<codeSize; i++){
         codeToTransmit[i]=lightOFF[i];
       } 
    }
    codeToggler=!codeToggler;
  }
   
   
   
   
  /*-----------------------------------------------------------------
    transmitCode(): Used to transmit the signal to the RF receiver on
                    the fan/light. There are 6 different HIGH-LOW signal combinations. 
                    
                    SH = short high   or  LH = long high   
                                     PLUS
         SL = short low    or    LL = long low    or    VLL = very long low
                    
  -------------------------------------------------------------------*/
   void transmitCode(){
    // The LED will be turned on to create a visual signal transmission indicator.
    digitalWrite(ledPin, HIGH);
   
   //initialise the variables 
    int highLength = 0;
    int lowLength = 0;
    
    //The signal is transmitted 6 times in succession - this may vary with your remote.       
    for(int j = 0; j<6; j++){
      for(int i = 0; i<codeSize; i++){ 
        switch(codeToTransmit[i]){
          case 1: // SH + SL
            highLength=16;
            lowLength=4;
          break;
          case 2: // SH + LL
            highLength=34;
            lowLength=4;
          break;
          case 3: // SH + VLL
            highLength=82;
            lowLength=4;
          break;
          case 4: // LH + SL
            highLength=7;
            lowLength=4;
          break;
          case 5: // LH + LL
            highLength=7;
            lowLength=4;
          break;
          case 6: // LH + VLL
            highLength=7;
            lowLength=4;
          break;
        }
           
         /* Transmit a HIGH signal - the duration of transmission will be determined 
            by the highLength and timeDelay variables */
         digitalWrite(rfTransmitPin, HIGH);     
         delayMicroseconds(highLength*timeDelay); 
         
         /* Transmit a LOW signal - the duration of transmission will be determined 
            by the lowLength and timeDelay variables */
         digitalWrite(rfTransmitPin,LOW);     
         delayMicroseconds(lowLength*timeDelay);
           
      }
    }
    //Turn the LED off after the code has been transmitted.
    digitalWrite(ledPin, LOW); 
 }
