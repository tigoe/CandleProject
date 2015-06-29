/*
  This sketch prints the MAC address on an ESP8266

  created 29 Jun 2015
  by Tom Igoe

 */

#include <ESP8266WiFi.h>

byte mac[6];

void setup() {
  Serial.begin(9600);

  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  Serial.println(mac[5], HEX);
}


void loop() {

}
