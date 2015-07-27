/*
  This sketch makes a telnet client on port 8080 to a server
  It echoes any serial bytes to the server, and any server bytes
  to the serial port

  note: the header file "settings.h" is included in this repository.
  Include the following variables:
  char ssid[]     = "ssid";     // your network SSID
  char password[] = "password"; // your network password
  char host[] = "192.168.0.2";  // the IP address of the device running the server

  created 26 Jun 2015
  modified 20 Jul 2015
  by Tom Igoe

 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "settings.h"

WiFiClient client;
WiFiUDP Udp;

const int port = 8888;
byte mac[6];
long tenMinutes = 1000 * 60 * 10;
int loginInterval = 10 * 1000;
long lastNetworkMsg = 0;
long lastLogin = 0;
boolean online = false;

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);     // hold the ATTiny in reset until you connect
  Serial.begin(9600);
  Serial.setTimeout(10);
  Udp.setTimeout(10);
  Serial.print("Connecting to ");   // connect to access point
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.macAddress(mac);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // when connected to access point, acknowledge it:
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);
  digitalWrite(5, HIGH);       // activate the ATtiny
  login();
}


void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // if there's a UDP packet, read it:
    while (Udp.available()) {
      String line = Udp.readStringUntil('\n');
      Serial.print(line);
    }
    lastNetworkMsg = millis();
    online = true;
  }

  // Read all the lines of the reply from server and print them to Serial
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // send the MAC address to the server by way of greeting:
    Udp.beginPacket(host, 8888);
    Udp.println(line);
    Udp.endPacket();
  }
  // if you're not online, continue to try to login every ten seconds:
  if (!online && (millis() - lastLogin > loginInterval)) {
    login();
  }

  // if you're online and  haven't gotten a server message in ten minutes, 
  // assume the server's offline and let the candle controller know:
  if (online && millis() - lastNetworkMsg > tenMinutes) {
    Serial.println("~~~");
    login();
    online = false;
  }
}


boolean login() {
  Serial.println("logging in...");
  Udp.begin(port);
  // This will send the MAC address to the server:
  WiFi.macAddress(mac);             //get your MAC address
  String macAddr;                   // a string to convert it for sending
  for (int i = 0; i < 5; i++) {     // iterate over the MAC address
    macAddr += String(mac[i], HEX); // get the byte, convert to hex string
    macAddr += ":";                 // add a :
  }
  macAddr += String(mac[5], HEX);               // add the final byte

  // send the MAC address to the server by way of greeting:
  Udp.beginPacket(host, 8888);
  Udp.println(macAddr);
  Udp.endPacket();
  lastLogin = millis();
}
