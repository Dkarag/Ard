#define length1 250
#define SYNC 1400
#define BIT1_LENGTH  600
#define BIT0_LENGTH  300
#define RXPIN  2
#define DEVIATION 100
int timings[length1];
int i, counter;
int pinstate;
char Str[32];
bool captured = false;

void setup() {
  Serial.begin(9600);
  pinMode(RXPIN, INPUT);
}

void loop() {
  int i = 0;
  int pinstate = 1;
  while (pinstate == 0)
  {
    pinstate = digitalRead(RXPIN);
  }
  while (i < length1) {
    while (pinstate == 0)
    {
      counter++;
      pinstate = digitalRead(RXPIN);
    }
    if (counter > 300) {
      timings[i] = counter;
      i++;
    }
    counter = 0;
    while (pinstate == 1)
    {
      pinstate = digitalRead(RXPIN);
    }
  }

  for (int i = 0; i < length1; i++)
  {
    Serial.println(timings[i]);
  }
}



