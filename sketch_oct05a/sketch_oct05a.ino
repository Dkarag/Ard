#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <Xively.h>

//ETHERNET INITIALISATION******************

// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

//ETHERNET INITIALISATION******************


// Your Xively key to let you upload data
char xivelyKey[] = "xFAXuPlmZUwYXnvqsFwnEqTvM5SUvAxvNG0jUqNwBbRrWG5c";
unsigned long feedId = 2017146907; // enter your feed ID, under Activated
char TEMPERATURE[] = "TEMPERATURE";
char ON_OFF[] = "ON_OFF";
//DATASTREAM TO WRITE
XivelyDatastream datastream_TEMPERATURE[] = {
  XivelyDatastream(TEMPERATURE, strlen(TEMPERATURE), DATASTREAM_FLOAT)
};
//DATASTREAM TO READ FROM
XivelyDatastream datastreams_ON_OFF[] = {
 XivelyDatastream(ON_OFF, strlen(ON_OFF), DATASTREAM_INT),
};

// Finally, wrap the datastreams into  feeds
XivelyFeed feed_TEMPERATURE(feedId, datastream_TEMPERATURE, 1 /* number of datastreams */);
XivelyFeed feed_ON_OFF(feedId, datastreams_ON_OFF, 1 /* number of datastreams */);

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
  datastream_TEMPERATURE[0].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastream_TEMPERATURE[0].getFloat());


  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed_TEMPERATURE, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  
  int ret1 = xivelyclient.get(feed_ON_OFF, xivelyKey);
  Serial.print("xivelyclient.get returned ");
  Serial.println(ret);
  
  if (ret1 > 0)
  {
    Serial.println("Datastream is...");
    //Serial.println(datastreams_ON_OFF[0]);

    Serial.print("On_Off is: ");
    Serial.println(datastreams_ON_OFF[0].getFloat());
  }

  Serial.println();
  
  
  
  
  delay(5000);
}








