#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <Xively.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 9 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

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
char SEQ[] = "SEQ";
//DATASTREAM TO WRITE
XivelyDatastream datastream_TEMPERATURE[] = {
  XivelyDatastream(TEMPERATURE, strlen(TEMPERATURE), DATASTREAM_FLOAT)
};
//DATASTREAM TO READ FROM
XivelyDatastream datastreams_SEQ[] = {
 XivelyDatastream(SEQ, strlen(SEQ), DATASTREAM_INT),
};

// Finally, wrap the datastreams into  feeds
XivelyFeed feed_TEMPERATURE(feedId, datastream_TEMPERATURE, 1 /* number of datastreams */);
XivelyFeed feed_SEQ(feedId, datastreams_SEQ, 1 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);


void setup() {
  

  
  int DELAY_CONSTATNT=5000;
  
 // Open serial communications and wait for port to open:
 pinMode(10, OUTPUT); 
 digitalWrite(10,HIGH);
  Serial.begin(115200);
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
 
  // Start up the library
  sensors.begin();
}


void loop(){
  int DELAY_CONSTATNT;
  //ds1820
  float temperature;
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));  
  temperature =sensors.getTempCByIndex(0);
  
  
  
  //int sensorValue = analogRead(10);
  datastream_TEMPERATURE[0].setFloat(temperature);

  //Serial.print("Read sensor value ");
  //Serial.println(datastream_TEMPERATURE[0].getFloat());


  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed_TEMPERATURE, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  delay(500);
  Serial.println();
  
  int ret1 = xivelyclient.get(feed_SEQ, xivelyKey);
  Serial.print("xivelyclient.get returned ");
  Serial.println(ret);
  
  if (ret1 > 0)
  {
    //Serial.println("Datastream is...");
    //Serial.println(datastreams_SEQ[0]);

   // Serial.print("SEQ is: ");
   // Serial.println(datastreams_SEQ[0].getInt());
    DELAY_CONSTATNT=1000*datastreams_SEQ[0].getInt();
    
  }

 // Serial.println();
  
  Serial.print("DELAY_CONSTATNT is= ");
  Serial.println(DELAY_CONSTATNT);
  
  delay(DELAY_CONSTATNT);
}








