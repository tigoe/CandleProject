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

//#include <ESP8266WiFi.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"


WiFiUDP Udp;
IPAddress ip(192, 168, 1, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const int port = 8888;
const int numClients = 30;
IPAddress dns(8, 8, 8, 8);
IPAddress clients[numClients];
int clientCount = 0;
long lastNetworkMsg = 0;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  while (!Serial);
  Udp.setTimeout(10);
  Serial.print("Setting up access point");
  WiFi.config(ip, dns, gateway, subnet);
  int  status = WiFi.beginAP(SECRET_SSID, SECRET_PASS);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }
  //  WiFi.softAP(SECRET_SSID, SECRET_PASS);
  //
  //  WiFi.softAPConfig(ip, gateway, subnet);
  Udp.begin(port);
  printWiFiStatus();
  Serial.print("Connect to Wifi SSID: ");
  Serial.println(SECRET_SSID);
  Serial.print("then telnet to IP address: ");
  Serial.println(ip);
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
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
      Serial.print(": ");
      Serial.println(line);
      // if remoteIP is not in the client list,
      // and clientCount < numClients, add it to the client list:
      if (!checkAddress(Udp.remoteIP()) && clientCount < numClients) {
        clients[clientCount] = Udp.remoteIP();
        // send an initial greeting (clients expect !!!):
        Serial.print("Greeting new client: ");
        Serial.println(Udp.remoteIP());
        Udp.beginPacket(Udp.remoteIP(), 8888);
        Udp.println("Hello!!!");
        Udp.endPacket();
        // increment the count of current clients:
        clientCount++;
      } else {
        Udp.beginPacket(Udp.remoteIP(), 8888);
        Udp.println("Thanks!!!");
        Udp.endPacket();
      }
      // if there are clients in the list,
      // send them the message
      if (clientCount > 0 ) {
        broadcast(line);
      }
    }
    // timestamp the last message:
    lastNetworkMsg = millis();
  }
  // since this candle is the host, no need for this now:
  // Read all the lines of the serial port and send them to the
  // connected clients:
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // send the data:
    broadcast(line);
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
    Serial.println(clients[c]);
    Udp.beginPacket(clients[c], 8888);
    Udp.println(message);
    Udp.endPacket();
  }
}
