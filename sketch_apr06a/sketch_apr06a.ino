/*
    Simple HTTP get webclient test
*/

#include <ESP8266WiFi.h>
//#include <Esp.h>
#include <Xively.h>
#include <RF433.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

//#define DHTPIN D1     // what digital pin we're connected to
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//DHT dht(DHTPIN, DHTTYPE);
#define RFPIN  D2 //pin used  for rf receiver
#define ONE_WIRE_BUS D1 //pin used  for DS1820 receiver

const char* ssid     = "Esoptron";
const char* password = "melenepopi6543211986m3fisto";

// Your Xively key to let you upload data
char xivelyKey[] = "xFAXuPlmZUwYXnvqsFwnEqTvM5SUvAxvNG0jUqNwBbRrWG5c";
unsigned long feedId = 2017146907; // enter your feed ID, under Activated
char Temp_IN[] = "Temp_IN";
char Temp_OUT[] = "Temp_OUT";
char Humidity_IN[] = "Humidity_IN";
char SEQ[] = "SEQ";
WiFiClient client;
XivelyClient xivelyclient(client);
//DATASTREAM TO WRITE
XivelyDatastream datastream_Temp_IN[] = {
  XivelyDatastream(Temp_IN, strlen(Temp_IN), DATASTREAM_FLOAT)
};
XivelyDatastream datastream_Temp_OUT[] = {
  XivelyDatastream(Temp_OUT, strlen(Temp_OUT), DATASTREAM_FLOAT)
};
XivelyDatastream datastream_Humidity_IN[] = {
  XivelyDatastream(Humidity_IN, strlen(Humidity_IN), DATASTREAM_FLOAT)
};
//DATASTREAM TO READ FROM
XivelyDatastream datastreams_SEQ[] = {
  XivelyDatastream(SEQ, strlen(SEQ), DATASTREAM_INT),
};

// Finally, wrap the datastreams into  feeds
XivelyFeed feed_Temp_IN(feedId, datastream_Temp_IN, 1 /* number of datastreams */);
XivelyFeed feed_Temp_OUT(feedId, datastream_Temp_OUT, 1 /* number of datastreams */);
XivelyFeed feed_Humidity_IN(feedId, datastream_Humidity_IN, 1 /* number of datastreams */);
XivelyFeed feed_SEQ(feedId, datastreams_SEQ, 1 /* number of datastreams */);

RF433 RF_SENSOR = RF433(RFPIN);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(100);
  sensors.begin();
  // dht.begin();
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);

  // We start by connecting to a WiFi network

  Serial.println();
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
  Serial.println("RF_SENSOR.enableRF");
  RF_SENSOR.enableRF();

}

float TEMP_IN;
float TEMP_OUT;
int ret;
void loop() {
  bool UPLOAD = false;
  float h = 0.1;
  digitalWrite(LED_BUILTIN, HIGH);
  if (RF_SENSOR.received == true)
  { digitalWrite(LED_BUILTIN, LOW);
    RF_SENSOR.disableRF();
    Serial.println("check1:RF_SENSOR.received == true");
    sensors.requestTemperatures(); // Send the command to get temperatures
    TEMP_IN = sensors.getTempCByIndex(0);
    TEMP_OUT = RF_SENSOR.getTemperature();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // h = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.println(h);
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

    datastream_Temp_OUT[0].setFloat(TEMP_OUT);
    Serial.println("Uploading it to Xively");
    ret = xivelyclient.put(feed_Temp_OUT, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
    delay(500);
    Serial.println();

    datastream_Temp_IN[0].setFloat(TEMP_IN);
    Serial.println("Uploading it to Xively");
    ret = xivelyclient.put(feed_Temp_IN, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
    delay(500);
    Serial.println();

    datastream_Humidity_IN[0].setFloat(h);
    Serial.println("Uploading it to Xively");
    ret = xivelyclient.put(feed_Humidity_IN, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
    delay(500);
    Serial.println();
    Serial.println("check2:RF_SENSOR.enableRF");
    RF_SENSOR.enableRF();
  }



}



