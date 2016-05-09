#define PIRPIN  D5

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIRPIN, INPUT);
}
int sensorVal;
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  sensorVal = digitalRead(PIRPIN);
  Serial.println(sensorVal);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}
