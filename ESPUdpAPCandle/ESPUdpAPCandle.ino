/*
 ESP UDP AP candle

 This is a variant on the ESP8266 firmware for the candles.
 This variant makes the ESP8266 into an access point. 
 arduino_secrets.h shouls contain the SSID name and password
 as SECRET_SSID and SECRET_PASSWORD

 currently does not compile. Committing this to work on other machine.
 -tigoe 10/5/2019
 modified 5 Oct 2019
 by Tom Igoe
 */
 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"


WiFiUDP Udp;
IPAddress ip(192, 168, 0, 2);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress clients[30];


const int port = 8888;
const int numClients = 30;
int clientCount = 0;
long lastNetworkMsg = 0;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  Udp.setTimeout(10);
  Serial.print("Setting up access point");
  WiFi.softAP(SECRET_SSID, SECRET_PASS);

  WiFi.softAPConfig(ip, gateway, subnet);
  Udp.begin(port);
  Serial.print("Connect to Wifi SSID: ");
  Serial.println(SECRET_SSID);
  Serial.print("then telnet to IP address: ");
  Serial.println(ip);
}


void loop() {
  long currentTime = millis();

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    String line;
    // if there's a UDP packet, read it:
    while (Udp.available()) {
      line = Udp.readStringUntil('\n');
      Serial.print("Packet from: ");
      Serial.print(Udp.remoteIP());

      // if remoteIP is not in the client list,
      // and clientCount < numClients, add it to the client list:
      if (!checkAddress(Udp.remoteIP()) && clientCount < numClients) {
        clients[clientCount] = Udp.remoteIP();
        // send an initial greeting (clients expect !!!):
        Udp.beginPacket(Udp.remoteIP(), 8888);
        Udp.println("Hello!!!");
        Udp.endPacket();
        // increment the count of current clients:
        clientCount++;
      }
      // if there are clients in the list,
      // send them the message
      if (clientCount > 0 ) {
        broadcast(line);
      }

      Serial.print(": ");
      Serial.println(line);
    }
    // timestamp the last message:
    lastNetworkMsg = millis();
  }

  // Read all the lines of the reply from server and print them to Serial
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // send the data:
    Udp.beginPacket(HOST, 8888);
    Udp.println(line);
    Udp.endPacket();
  }
}

boolean checkAddress(IPAddress ip) {
  // iterate over the client list, see if this IP is in it:
  for (int c = 0; c < numClients; c++) {
    if (clients[c] == ip ) {
      return true;
    }
  }
  return false;
}

void broadcast(String message) {
  // iterate over the client list
  // and send messages to all valid clients:
  for (int c = 0; c <= clientCount; c++) {
      Udp.beginPacket("192.168.0.255",8888);
//    Udp.beginPacket(clients[c], 8888);
    Udp.println(message);
    Udp.endPacket();
  }
}
