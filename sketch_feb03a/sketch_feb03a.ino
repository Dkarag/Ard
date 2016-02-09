#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <Xively.h>
OneWire  ds(3);  // on pin 3 (a 4.7K resistor is necessary)

//ETHERNET INITIALIZATION******************
// Enter a MAC address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 166);
//IPAddress myDns(8, 8, 8, 8);
EthernetClient client;
//ETHERNET INITIALIZATION******************
// Your Xively key to let you upload data
char xivelyKey[] = "xFAXuPlmZUwYXnvqsFwnEqTvM5SUvAxvNG0jUqNwBbRrWG5c";
unsigned long feedId = 2017146907; // enter your feed ID, under Activated
char Temp_IN[] = "Temp_IN";
char Temp_OUT[] = "Temp_OUT";
char SEQ[] = "SEQ";
//DATASTREAM TO WRITE
XivelyDatastream datastream_Temp_IN[] = {
  XivelyDatastream(Temp_IN, strlen(Temp_IN), DATASTREAM_FLOAT)
};
XivelyDatastream datastream_Temp_OUT[] = {
  XivelyDatastream(Temp_OUT, strlen(Temp_OUT), DATASTREAM_FLOAT)
};



//DATASTREAM TO READ FROM
XivelyDatastream datastreams_SEQ[] = {
  XivelyDatastream(SEQ, strlen(SEQ), DATASTREAM_INT),
};

// Finally, wrap the datastreams into  feeds
XivelyFeed feed_Temp_IN(feedId, datastream_Temp_IN, 1 /* number of datastreams */);
XivelyFeed feed_Temp_OUT(feedId, datastream_Temp_OUT, 1 /* number of datastreams */);
XivelyFeed feed_SEQ(feedId, datastreams_SEQ, 1 /* number of datastreams */);
XivelyClient xivelyclient(client);



//variables used for RF LINK
#define buffer_size 150
#define CORRECTOR 1
#define SYNC 1750*CORRECTOR
#define BIT1_LENGTH  745*CORRECTOR
#define BIT0_LENGTH  380*CORRECTOR
#define RXPIN  7
#define DEVIATION 200
int timings[buffer_size];
int i, counter;
int pinstate;
char captured_data[33];
char bitreceived;
bool captured_state = false;
String temp_out, captured_data_str, temp;
String sensor_id = "0111111010000000";





float bintodec (String input)
{
  float  value = 0;
  for (int i = 0; i < 8; i++)
  {
    if (input.charAt(i) == '1')
    {
      value = value + pow(2, 7 - i);
    }
  }
  return value / 10;
}

void setup() {
  //int DELAY_CONSTATNT = 5000;
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  Serial.begin(9600);
  pinMode(RXPIN, INPUT);
  pinstate = 1;
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
  //variables used for DS1820
  //byte bt;
//  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  float Temp_out;
  i = 0;
  counter = 0;
  while (pinstate == 0)
  {
    pinstate = digitalRead(RXPIN);//wait until pinstate=0
  }
  // Serial.println("DEBUG2");
  while (i < buffer_size) {
    while (pinstate == 0)
    {
      counter++;
      pinstate = digitalRead(RXPIN);
    }
    if (counter > BIT0_LENGTH - DEVIATION / 2 && counter < SYNC + DEVIATION / 2)
    {
      timings[i] = counter;
      i++;
    }

    counter = 0;
    while (pinstate == 1)
    {
      pinstate = digitalRead(RXPIN);
    }

  }
  for (int i = 0; i < buffer_size; i++)
  {
       Serial.println(timings[i]);
  }

  int i = 0;
  while ( i < buffer_size && captured_state == false )
  { //Serial.println("DEBUG1");
    if (timings[i] > SYNC - DEVIATION && timings[i] < SYNC + DEVIATION && i + 33 < buffer_size)
    {
      // Serial.println("DEBUG2");
      for (int k = 1; k < 34; k++)
      {
        if (timings[i + k] > (BIT1_LENGTH - DEVIATION) && timings[i + k] < (BIT1_LENGTH + DEVIATION)) {
          //Serial.print("1");
          bitreceived = '1';
        } else if (timings[i + k] > (BIT0_LENGTH - DEVIATION) && timings[i + k] < (BIT0_LENGTH + DEVIATION)) {
          //Serial.print("0");
          bitreceived = '0';
        } else if (timings[i + k] > (SYNC - DEVIATION) && timings[i + k] < (SYNC + DEVIATION)) {
          // Serial.println("SYNC");

        }
        else {
          //   Serial.print("?");  // undefined timing
          bitreceived = '?';
        }
        captured_data[k - 1] = bitreceived;
      }
      //checks of captured data
      String captured_data_str(captured_data);

      if (captured_data_str.indexOf('?') == -1)// check the data
      {
        temp = captured_data_str.substring(0, 16);
        Serial.println(temp);
        if (temp.equals(sensor_id)) //check the sensor id
        {
        //  Serial.println("found!!!!!!!!!!");
          captured_state = true;
          temp_out = captured_data_str.substring(16, 24);
        }
      }

    }
    i++;
  }
  if (captured_state == true)
  {
    Serial.print("Temp_out=");
    Temp_out = bintodec(temp_out);
    Serial.println(Temp_out);
    pinstate = 1;
    captured_state = false;
    datastream_Temp_OUT[0].setFloat(Temp_out);
//    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed_Temp_OUT, xivelyKey);
    Serial.print("xivelyclient.put returned ");
//    Serial.println(ret);
   // delay(600);
//    Serial.println();
  }

  if ( !ds.search(addr)) {
    //  Serial.println("No more addresses.");
    //  Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  /*
    Serial.print("ROM =");
    for( i = 0; i < 8; i++) {
     Serial.write(' ');
     Serial.print(addr[i], HEX);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
       Serial.println("CRC is not valid!");
       return;
    }
    Serial.println();
  */
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      // Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //  Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      // Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //   Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(800);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad
  /*
    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
  */
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //  Serial.print(data[i], HEX);
    //  Serial.print(" ");
  }
  /*
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
  */
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  Serial.print("Temperature = ");
  Serial.println(celsius);
  datastream_Temp_IN[0].setFloat(celsius);
//  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed_Temp_IN, xivelyKey);
  Serial.print("xivelyclient.put returned ");
//  Serial.println(ret);
  //delay(1000);
 // Serial.println();

}
