/*
  This sketch makes a telnet client on port 8080 to a server
  It echoes any serial bytes to the server, and any server bytes
  to the serial port

  note: the header file "arduino_secrets.h" is included in this repository.
  Include the following variables:
  #define SECRET_SSID "ssid";       // network name
  #define SECRET_PASS "password";   // network password
  #define SECRET_HOST "x.x.x.x";    // server IP address

  created 26 Jun 2015
  modified 7 Oct 2019
  by Tom Igoe

*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"

WiFiClient client;
WiFiUDP Udp;

const int port = 8888;
byte mac[6];
long tenMinutes = 600000;
int loginInterval = 10000;
long lastNetworkMsg = 0;
long lastLogin = 0;
boolean online = false;

void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(10);
  Udp.setTimeout(10);
  connectToRouter();
}


void loop() {
  long currentTime = millis();

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // if there's a UDP packet, read it:
    while (Udp.available()) {
      String line = Udp.readStringUntil('\n');
      Serial.print(line);
    }
    lastNetworkMsg = millis();
    online =  true;
  }

  // Read all the lines of the reply from server and print them to Serial
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // send the MAC address to the server by way of greeting:
    Udp.beginPacket(SECRET_HOST, 8888);
    Udp.println(line);
    Udp.endPacket();
  }

  // if you're not connected to the router, attempt to reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToRouter();
    online = false;
  }

  if (online == false) {
    // attempt to log in:
    if (currentTime - lastLogin > loginInterval) {
      login();
    }
  } else {
    // if you're online and  haven't gotten a server message in ten minutes,
    // assume the server's offline and let the candle controller know:
    if (currentTime - lastNetworkMsg >= tenMinutes) {
      Serial.println("~~~");
      online = false;
    }
  }
}

void connectToRouter() {
  // let the ATTiny start:
  delay(100);
  online = false;
  //inform the ATTiny that you're offline
  Serial.println("~~~");
  Serial.print("Connecting to ");   // connect to access point
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  digitalWrite(5, LOW);     // hold the ATTiny in reset until you connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
   digitalWrite(5, HIGH);       // activate the ATtiny
  // when connected to access point, acknowledge it:
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);
  login();
}

boolean login() {
  Serial.println("logging in");
  Udp.begin(port);
  // This will send the MAC address to the server:
  WiFi.macAddress(mac);             //get your MAC address
  String macAddr;                   // a string to convert it for sending
  for (int i = 0; i < 5; i++) {     // iterate over the MAC address
    if (mac[i] < 0x10) {            // add a leading 0 to single-digit hex numbers
      macAddr += "0";
    }
    macAddr += String(mac[i], HEX); // get the byte, convert to hex string
    macAddr += ":";                 // add a :
  }
  if (mac[5] < 0x10) {             // add a leading 0 to single-digit hex numbers
    macAddr += "0";
  }
  macAddr += String(mac[5], HEX);               // add the final byte

  // send the MAC address to the server by way of greeting:
  Udp.beginPacket(SECRET_HOST, 8888);
  Udp.println(macAddr);
  Udp.endPacket();
  lastLogin = millis();
}
