#include <ESP8266WiFi.h>
#include <Esp.h>
#include <ThingSpeak.h>
extern "C" {
#include "user_interface.h"
}

//User Configuration
//*********************************************************
const char* ssid     = "Esoptron";
const char* password = "melenepopi6543211986m3fisto";
unsigned long myChannelNumber = 109165;
const char * myWriteAPIKey = "5GLMONFA69EYET4S";
//const char * myReadAPIKey = "T4XWUMI3LLVEUJ6T";a
float cor_factor = 0.01678;//y = 0.01678x − 0.1114
//Define the frequency of measurements in seconds
//i.e  if you want to measure every half an hour then 30min*60sec=1800
int refresh_rate = 1;
//*********************************************************
int sensorPin = A0;
int counter = 0;


WiFiClient client;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  digitalWrite(LED_BUILTIN, HIGH);
}


void loop() {
  delay(500);//1second
  counter++;
  if (counter > refresh_rate)
  {
    WIFI_CONNECT();// connect to wifi in not already connected
    // read the value from the sensor:
    float sensorValue = analogRead(sensorPin);
    float volts = sensorValue * cor_factor - 0.1114;
    //round to 1 decimal point precission
    int v = volts * 10;
    volts = (float)v / 10;
   // ThingSpeak.writeField(myChannelNumber, 3, sensorValue, myWriteAPIKey);
    //ThingSpeak.writeField(myChannelNumber, 3, volts, myWriteAPIKey);
    counter = 0;
    //Serial.print("Batery Voltage is=");
   // Serial.print(volts);
   // Serial.println("V");
    Serial.println(sensorValue);
    Serial.println();
    //OPTINAL CODE TO LIGHT BUILTIN LED WHEN VOLTAGE IS BELOW THRESHOLD
    if (volts < 11)
      digitalWrite(LED_BUILTIN, LOW);
    else
      digitalWrite(LED_BUILTIN, HIGH);
    //delay(500);
  }
}
void WIFI_CONNECT()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_STA);
    wifi_station_connect();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("-");
    }
  }
}


