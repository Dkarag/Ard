#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <utility/w5100.h>
#include <RF433.h>

//ETHERNET INITIALISATION******************

// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 166);

//ETHERNET INITIALISATION******************

EthernetClient client;



int RFPIN = 2; //pin used  for rf receiver


RF433 sensor = RF433(RFPIN) ;
int i = 0;
void setup() {
  Serial.begin(9600);
  //>**********RF-MODULE**********<//
  pinMode(RFPIN, INPUT);
  sensor.enableRF();
  //>**********RF-MODULE**********<//

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  Serial.println("Started....");

  //>**start the Ethernet connection*<//
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);




}

void loop() {
  // float temp;

  if (sensor.received == true)
  {
    // temp = sensor.getTemperature();
    Serial.print(i);
    Serial.print(" :=====> outside Temperature is:");
    Serial.println( sensor.getTemperature());

    i++;
  }
  sensor.received = false;
}
