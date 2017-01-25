#include "DHT.h"
#include <RF433.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <Ethernet.h>
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define RFPIN  21 //pin used  for rf receiver
DHT dht(DHTPIN, DHTTYPE);
//ETHERNET INITIALIZATION******************
// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 167);
//ETHERNET INITIALISATION******************
EthernetClient client;
unsigned long myChannelNumber = 217050;
const char * myWriteAPIKey = "PSYYN0838K9JM9QH";
const char * myReadAPIKey = "JBE4YX4Q8Q2VFAH1";
RF433 RF_SENSOR = RF433(RFPIN);
void setup() {
 // Serial.begin(9600);
  Serial.println("DHTxx test!");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  dht.begin();
  //>**start the Ethernet connection*<//
  if (Ethernet.begin(mac) == 0) {
  //  Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
 // Serial.println(Ethernet.localIP());
  ThingSpeak.begin(client);
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);
 // Serial.println("RF_SENSOR.enableRF");
  RF_SENSOR.enableRF();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
 //   Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
 /*
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(" *C ");
  */
  if (RF_SENSOR.received() == true)
  {
    // digitalWrite(LED_BUILTIN, LOW);
    RF_SENSOR.disableRF();
    //Serial.println("check1:RF_SENSOR.received() == true");
    float TEMP_OUT = RF_SENSOR.getTemperature();
   // Serial.print(" :=====> Outside Temperature is:");
   // Serial.println(TEMP_OUT);
   // Serial.print("RF_SENSOR.data_OK=");
  //  Serial.println(RF_SENSOR.data_OK);
    if (RF_SENSOR.data_OK == true)
    {
  //    Serial.println("COrrect signal received");
      //ThingSpeak.writeField(myChannelNumber, 3,TEMP_OUT, myWriteAPIKey);
    ThingSpeak.setField(3,TEMP_OUT);
    }
    else
    {
   //   Serial.println("Incorrect signal received");
    }

    delay(2500);
  }
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
   ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  RF_SENSOR.enableRF();

}
