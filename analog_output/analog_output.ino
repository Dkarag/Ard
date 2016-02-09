#define DATAPIN  A0

bool readinput(int rfReceivePin)
{
  int val=analogRead(rfReceivePin);
  if(val<80)
  {
    return true;
  }
 return false;  
}

int i=0;

void setup() {
   Serial.begin(9600);
  Serial.println("Started.");

}

void loop() {
//int  val=analogRead(DATAPIN);
Serial.println(i);
 i=0;
while(readinput(DATAPIN))
{
  i++;
  
}

    
}
