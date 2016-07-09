#include <ESP8266WiFi.h>
#include <Esp.h>
//>OTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//<OTA

#include <ThingSpeak.h>
#include <RF433.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "DHT.h"
extern "C" {
#include "user_interface.h"
}

//****************************************
#define DEBUG
//#define POWER_SAVE_ENABLE
#define PIR_ENABLE


//*********************************************
#define RFPIN  D2 //pin used  for rf receiver
#define ONE_WIRE_BUS D1 //pin used  for DS1820 receiver
#define PIRPIN  D5 //pin used for PIR SENSOR

const char* ssid     = "Esoptron";
const char* password = "melenepopi6543211986m3fisto";
WiFiClient client;
unsigned long myChannelNumber = 109165;
const char * myWriteAPIKey = "5GLMONFA69EYET4S";
const char * myReadAPIKey = "T4XWUMI3LLVEUJ6T";


RF433 RF_SENSOR = RF433(RFPIN);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(100);
  sensors.begin();
  pinMode(PIRPIN, INPUT);
  // dht.begin();
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  /*
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

  */

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("RF_SENSOR.enableRF");
  RF_SENSOR.enableRF();
  ThingSpeak.begin(client);
#ifdef PIR_ENABLE
  attachInterrupt(digitalPinToInterrupt(PIRPIN), pirhandler, RISING);
#endif

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
float TEMP_IN;
float TEMP_OUT;
int ret;
char* ALERT_EVENT = "NOT_SET";
unsigned long TIME1, TIME2;

void loop() {
  ArduinoOTA.handle();
#ifdef POWER_SAVE_ENABLE
  // SLEEP_WIFI();
#endif
  bool UPLOAD = false;
  float h = 0.1;
  digitalWrite(LED_BUILTIN, HIGH);
  if (RF_SENSOR.received == true)
  {
    digitalWrite(LED_BUILTIN, LOW);
    RF_SENSOR.disableRF();
    Serial.println("check1:RF_SENSOR.received == true");
    sensors.requestTemperatures(); // Send the command to get temperatures
    TEMP_IN = sensors.getTempCByIndex(0);
    TEMP_OUT = RF_SENSOR.getTemperature();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // h = dht.readHumidity();
    //Serial.print("Humidity: ");
    //Serial.println(h);
    Serial.print(" :=====> Outside Temperature is:");
    Serial.println(TEMP_OUT);
    Serial.print(" :=====> Inside Temperature is:");
    Serial.println(TEMP_IN);
    if (TEMP_OUT > 100.0)
    {
      Serial.println("Corrupted signal received");
      UPLOAD = false;
      Serial.println("check2:RF_SENSOR.enableRF");
      RF_SENSOR.enableRF();
    }
    else
    {
      Serial.println("Correct signal received");
      UPLOAD = true;
    }
  }
  if (UPLOAD == true)
  {
#ifdef POWER_SAVE_ENABLE
    WAKEUP_WIFI();
#endif
    delay(500);
    ThingSpeak.setField(1, TEMP_IN);
    ThingSpeak.setField(2, TEMP_OUT);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    Serial.println("check2:RF_SENSOR.enableRF");
#ifdef POWER_SAVE_ENABLE
    SLEEP_WIFI();
#endif
    RF_SENSOR.enableRF();
  }
#ifdef PIR_ENABLE
  if (ALERT_EVENT == "SET")
  {
    detachInterrupt(digitalPinToInterrupt(PIRPIN));
#ifdef POWER_SAVE_ENABLE
    WAKEUP_WIFI();
#endif
    Serial.println();
    ALERT_EVENT = "RESET";
    ThingSpeak.writeField(myChannelNumber, 3, 100, myWriteAPIKey);
    Serial.println("ALERT_EVENT_SET");
    TIME1 = millis();
  }

  if (ALERT_EVENT == "RESET")
  {
    TIME2 = millis();
    if (TIME2 - TIME1 > 10000)
    {
      ALERT_EVENT = "NOT_SET";
      ThingSpeak.writeField(myChannelNumber, 3, 0, myWriteAPIKey);
      Serial.println("ALERT_EVENT_CLEARED");
#ifdef POWER_SAVE_ENABLE
      SLEEP_WIFI();
#endif
      attachInterrupt(digitalPinToInterrupt(PIRPIN), pirhandler, RISING);
      RF_SENSOR.enableRF();
    }
  }
#endif
}

void pirhandler()
{
  RF_SENSOR.disableRF();
  ALERT_EVENT = "SET";
}

void SLEEP_WIFI()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(3000);
    Serial.println(WiFi.status());
  }
}

void WAKEUP_WIFI()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.forceSleepWake();
    delay(1);
    WiFi.mode(WIFI_STA);
    wifi_station_connect();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("-");
    }
  }
}

