#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <Xively.h>
#include <OneWire.h>

#define length1 70
#define SYNC 9200
#define BIT1_LENGTH  4000
#define BIT0_LENGTH  2000
#define RXPIN  3
#define DEVIATION 1000
int timings[length1];
int i;
int pinstate;
int oldstate = 0;
int lastTime = 0;
int duration = 0;
bool captured = false;
char Str[32];
//ETHERNET INITIALISATION******************

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

XivelyClient xivelyclient(client);


float bintodec (String input)
{
  //int strlength = input.length();
  float  value = 0;
  for (int i = 0; i < 8; i++)
  {
    if (input.charAt(i) == '1')
    {
      value = value + pow(2, 7 - i);
    }
  }
  return value / 10.0;
}



void setup() {
  pinMode(10, OUTPUT);
  //

  //start serial connection
  Serial.begin(9600);
  pinMode(RXPIN, INPUT);
  pinMode(13, OUTPUT);
  //   start the Ethernet connection:
/*
  digitalWrite(10, HIGH);
  if (Ethernet.begin(mac) == 0) {
       Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
   Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
*/
//noInterrupts();
}

void loop() {
  int DELAY_CONSTATNT;
  captured = false;
  float temperature;
  i = 0;
  while (i < length1)
  {
    pinstate = digitalRead(RXPIN);
    if ( pinstate != oldstate)
    {
      // calculating timing since last change
      long time = micros();
      duration = time - lastTime;
      lastTime = time;
      if (duration > BIT0_LENGTH - DEVIATION && duration < SYNC + DEVIATION )
      {
        timings[i] = duration;
        i++;
      }
    }

  }

  char bitreceived;
  int i = 0;
  while ( i < length1&&captured == false)
  {
    if (timings[i] > SYNC - DEVIATION && timings[i] < SYNC + DEVIATION && i + 33 < length1)
    {
      Serial.println("");
      for (int k = 1; k < 34; k++)
      {
        if (timings[i + k] > (BIT1_LENGTH - DEVIATION) && timings[i + k] < (BIT1_LENGTH + DEVIATION)) {
          Serial.print("1");
          bitreceived = '1';
        } else if (timings[i + k] > (BIT0_LENGTH - DEVIATION) && timings[i + k] < (BIT0_LENGTH + DEVIATION)) {
          Serial.print("0");
          bitreceived = '0';
        } else if (timings[i + k] > (SYNC - DEVIATION) && timings[i + k] < (SYNC + DEVIATION)) {
          Serial.println("SYNC");
        }
        else {
          Serial.print("?");  // undefined timing
          bitreceived = '?';
        }
        Str[k - 1] = bitreceived;

      }
      String stringOne(Str);
      String temp_out = stringOne.substring(16, 24);
      if ( bintodec(temp_out) > 14 && bintodec(temp_out) < 17)
      {
        captured == true;
        Serial.print("Temp_out=");
        Serial.println(bintodec(temp_out));
        temperature = bintodec(temp_out);
      }
    }
    i++;
  }
  if (captured == true)
  {

    delay(1000);
    captured == false;
   /* datastream_TEMPERATURE[0].setFloat(temperature);
    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed_TEMPERATURE, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
    Serial.println();
    int ret1 = xivelyclient.get(feed_SEQ, xivelyKey);
    Serial.print("xivelyclient.get returned ");
    Serial.println(ret);

    if (ret1 > 0)
    {
      DELAY_CONSTATNT = 1000 * datastreams_SEQ[0].getInt();
    }

    // Serial.println();

    Serial.print("DELAY_CONSTATNT is= ");
    Serial.println(DELAY_CONSTATNT);

    // delay(DELAY_CONSTATNT);
*/
  }

  lastTime = 0;


  // Serial.println(timings[i]);
}

