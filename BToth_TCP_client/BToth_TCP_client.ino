
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial BTport(D5, D6);
const char* ssid     = "Esoptron";
const char* password = "melenepopi6543211986m3fisto";
const int Port = 8888;
const char* host = "192.168.1.150";
char buf[28];


void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // Start each software serial port
  BTport.begin(9600);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}
WiFiClient client;
void loop() {
  if (BTport.available() > 0)
  {
    String Str1;
    // while there is data coming in, read it
    // and send to the hardware serial port:
    while (BTport.available() > 0) {
      //Serial.println("Data from port one:");
      char inByte = BTport.read();
      //Serial.print(inByte);
      delay(1);
      Str1 = Str1 + inByte;
    }

    if (!client.connect(host, Port)) {
      Serial.println("connection failed");
      return;
    }
    client.print(String(Str1));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      line.toCharArray(buf, sizeof(line));
      BTport.write(buf);//send back to BT serial
    }
    Serial.println();
    Serial.println(Str1);
    Serial.println(Str1.length());
  }
}
