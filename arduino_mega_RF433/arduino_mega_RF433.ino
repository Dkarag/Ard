//#include <ESP8266WiFi.h>
//#include <ThingSpeak.h>
//#include <Esp.h>
#include <RF433.h>
//#include "DHT.h"
//extern "C" {
//#include "user_interface.h"
//}
//****************************************
#define DEBUG
//#define POWER_SAVE_ENABLE
//#define PIR_ENABLE
//*********************************************
#define RFPIN  21 //pin used  for rf receiver
#define ONE_WIRE_BUS D1 //pin used  for DS1820 receiver
#define PIRPIN  D5 //pin used for PIR SENSOR
//#define RFPIN 21
/*
const char* ssid     = "Esoptron";
const char* password = "melenepopi6543211986m3fisto";
WiFiClient client;
unsigned long myChannelNumber = 217050;
const char * myWriteAPIKey = "5GLMONFA69EYET4S";
const char * myReadAPIKey = "T4XWUMI3LLVEUJ6T";
*/
RF433 RF_SENSOR = RF433(RFPIN);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
void setup() {

  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(200);
 /*
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
*/
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);
  Serial.println("RF_SENSOR.enableRF");
  RF_SENSOR.enableRF();
}
//float TEMP_OUT;

void loop() {
  //digitalWrite(LED_BUILTIN, HIGH);
  if (RF_SENSOR.received() == true)
  {
    // digitalWrite(LED_BUILTIN, LOW);
    RF_SENSOR.disableRF();
    //Serial.println("check1:RF_SENSOR.received() == true");
    float TEMP_OUT = RF_SENSOR.getTemperature();
    Serial.print(" :=====> Outside Temperature is:");
    Serial.println(TEMP_OUT);
    Serial.println("RF_SENSOR.data_OK=");
    Serial.print(RF_SENSOR.data_OK);
    if (RF_SENSOR.data_OK == true)
    {
      Serial.println("COrrect signal received");
      // Serial.println("check2:RF_SENSOR.enableRF");
      //ThingSpeak.setField(3, TEMP_OUT);
      ThingSpeak.writeField(myChannelNumber, 3, (float)TEMP_OUT, myWriteAPIKey);
    }
    else
    {
      Serial.println("Incorrect signal received");
    }
    RF_SENSOR.enableRF();
    //delay(2500);
  }
}
