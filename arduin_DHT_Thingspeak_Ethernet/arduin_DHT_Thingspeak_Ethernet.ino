#include "DHT.h"
#include <ThingSpeak.h>
#include <SPI.h>
#include <Ethernet.h>
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
//ETHERNET INITIALIZATION******************
// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 166);
//ETHERNET INITIALISATION******************
EthernetClient client;
unsigned long myChannelNumber = 109165;
const char * myWriteAPIKey = "5GLMONFA69EYET4S";
const char * myReadAPIKey = "T4XWUMI3LLVEUJ6T";

void setup() {
 // Serial.begin(9600);
 // Serial.println("DHTxx test!");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  dht.begin();
  //>**start the Ethernet connection*<//
  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
//  Serial.println(Ethernet.localIP());
  ThingSpeak.begin(client);
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
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");

*/
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
}
