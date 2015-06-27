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
  modified 27 Jun 2015
  by Tom Igoe

 */

#include <ESP8266WiFi.h>
#include "settings.h"

WiFiClient client;
const int port = 8080;

void setup() {
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);     // hold the ATTiny in reset until you connect
  Serial.begin(9600);
  Serial.setTimeout(10);
  client.setTimeout(10);
  Serial.print("Connecting to ");   // connect to access point
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
  digitalWrite(15, HIGH);       // activate the ATtiny
  login();                      // log into the server
}


void loop() {
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.print(line);
  }

  // Read all the lines of the reply from server and print them to Serial
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    client.print(line);
  }

  if (!client.connected()) {
    Serial.println("0");
    delay(1000);
    login();
  }
}

boolean login() {
  client.connect(host, port);

  while (!client.connected()) {
    Serial.println("connection failed, trying again");
    delay(2000);
    client.connect(host, port);
  }
  // This will send the IP address to the server
  client.println(WiFi.localIP());
  Serial.println("1");
}

