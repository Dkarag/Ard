#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <utility/w5100.h>
#include <RF433.h>
#include <Xively.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define DHTPIN 6     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
#define RFPIN  21 //pin used  for rf receiver
#define ONE_WIRE_BUS 5 //pin used  for DS1820 receiver

//ETHERNET INITIALIZATION******************
// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 166);
//ETHERNET INITIALISATION******************
EthernetClient client;


// Your Xively key to let you upload data
char xivelyKey[] = "xFAXuPlmZUwYXnvqsFwnEqTvM5SUvAxvNG0jUqNwBbRrWG5c";
unsigned long feedId = 2017146907; // enter your feed ID, under Activated
char Temp_IN[] = "Temp_IN";
char Temp_OUT[]= "Temp_OUT";
char Humidity_IN[]= "Humidity_IN";
char SEQ[] = "SEQ";

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
XivelyClient xivelyclient(client);


RF433 RF_SENSOR = RF433(RFPIN);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
void setup() {
  Serial.begin(9600);
  // Start up the library
  sensors.begin();
  dht.begin();
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);
  RF_SENSOR.enableRF();
  //>**********ETHERNET-MODULE**********<//

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  Serial.println("Started....");

  //>**start the Ethernet connection*<//
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac,ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println(Ethernet.localIP());

}

void loop() {
  float TEMP_OUT, TEMP_IN;
  bool UPLOAD = false;
  int ret;
  float h;
  if (RF_SENSOR.received == true)
  {
    sensors.requestTemperatures(); // Send the command to get temperatures
    TEMP_IN = sensors.getTempCByIndex(0);
    TEMP_OUT = RF_SENSOR.getTemperature();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print(" :=====> Outside Temperature is:");
    Serial.println(TEMP_OUT);
    Serial.print(" :=====> Inside Temperature is:");
    Serial.println(TEMP_IN);
    if(TEMP_OUT==0.01)
    {
     Serial.println("Corrupted signal received.");
    }
    else
      UPLOAD = true;
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
  }
}
