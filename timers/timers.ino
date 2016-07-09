/*
   TimeNTP_ESP8266WiFi.ino
   Example showing time sync to NTP time source

   This sketch uses the ESP8266WiFi library
*/

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
const char ssid[] = "Esoptron";  //  your network SSID (name)
const char pass[] = "melenepopi6543211986m3fisto";       // your network password
char auth[] = "67938244ee844528a89e0c03ba378098";
// NTP Servers:
static const char ntpServerName[] = "time.nist.gov";
const int timeZone = 3;     // Greek European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);
int timer1 = 10;
int timer2 = 5;
int timer3 = 1;
int start = 8;
int stop1 = 17;
int interval = 2;

BLYNK_WRITE(V0)
{
  timer1 = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V1)
{
  timer2 = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V2)
{
  timer3 = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V3)
{
  start = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V4)
{
  stop1 = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V5)
{
  interval = param.asInt(); // Get value as integer
}






void setup()
{
  bool result = Blynk.connect();
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  delay(250);
  Serial.print("Connecting to ");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  while ( timeStatus() != timeSet) {
    delay(1000);
    setSyncProvider(getNtpTime);
    Serial.print("-");
  }
  setSyncInterval(3600);
  Blynk.config(auth);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
}


time_t prevDisplay = 0; // when the digital clock was displayed
int current_minutes, next_minutes;
bool firstTime = true;
void sequential_timers(int t1, int t2, int t3)
{
  digitalWrite(D1, HIGH);
  delay(t1 * 1000);
  digitalWrite(D2, HIGH);
  delay(t2 * 1000);
  digitalWrite(D3, HIGH);
  delay(t3 * 1000);
  digitalWrite(D3, LOW);
  delay(1000);
  digitalWrite(D2, LOW);
  delay(1000);
  digitalWrite(D1, LOW);
}
void loop()
{
  Blynk.run();
  if (now() != prevDisplay) { //update the display only if time has changed
    prevDisplay = now();
    Serial.print(hour());
    Serial.print(":");
    Serial.println(minute());
    Serial.print("timer1=");
    Serial.println(timer1);
    Serial.print("timer2=");
    Serial.println(timer2);
    Serial.print("timer3=");
    Serial.println(timer3);
     Serial.print("start=");
    Serial.println(start);
     Serial.print("stop1=");
    Serial.println(stop1);
     Serial.print("interval=");
    Serial.println(interval);
    if ( hour() >= start && hour() <= stop1)
    {
      if ( minute() == next_minutes || firstTime == true)
      {
        sequential_timers(timer1, timer2, timer3);
        next_minutes = minute() + interval;
        if (next_minutes > 59)
          next_minutes = next_minutes - 60;
        Serial.print(next_minutes);
        Serial.print("ACTIVATE");
        Serial.print(hour());
        Serial.print(":");
        Serial.println(minute());
        firstTime = false;
      }
    }

  }
































  /* if (timeStatus() != timeNotSet) {
     if (now() != prevDisplay) { //update the display only if time has changed
       prevDisplay = now();
       digitalClockDisplay();
     }
    }*/
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
