/*

 */

#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <Xively.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };



// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

// Your Xively key to let you upload data
char xivelyKey[] = "xFAXuPlmZUwYXnvqsFwnEqTvM5SUvAxvNG0jUqNwBbRrWG5c";
unsigned long feedId = 2017146907; // enter your feed ID, under Activated
char sensorId[] = "sensor_reading";
char sensorread[] = "sensorread";
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT)
};



XivelyDatastream datastreams1[] = {
 XivelyDatastream(sensorread, strlen(sensorread), DATASTREAM_FLOAT),
};



// Finally, wrap the datastreams into a feed
XivelyFeed feed(feedId, datastreams, 1 /* number of datastreams */);



// Set the static IP address to use if the DHCP fails to assign
//IPAddress ip(192,168,1,177);   if static IP is needed

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
XivelyClient xivelyclient(client);
void setup() {
 // Open serial communications and wait for port to open:
 pinMode(10, OUTPUT); 
 digitalWrite(10,HIGH);
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac);
  }
  // give the Ethernet shield a second to initialize:
  delay(2000);
  Serial.println("connecting...");

}

void loop(){
  int sensorValue = analogRead(10);
  datastreams[0].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());


  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  
  
  /*
  int randomvalue(random(100)); 
  datastreams[1].setFloat(randomvalue);
  Serial.print("Setting string value to:\n    ");
  Serial.println(datastreams1[1].getFloat());

  
  Serial.println("Uploading it to Xively");
  int ret1 = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret1);

  Serial.println();
  
  */  
  
  delay(15000);
}







