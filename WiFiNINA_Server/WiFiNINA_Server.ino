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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "arduino_secrets.h"
#include "font.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP Udp;

const int port = 8888;
const int numClients = 30;
IPAddress clients[numClients];
int clientCount = 0;
long lastNetworkMsg = 0;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);

  // initialize the display library:
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println("SSD1306 setup failed");
    while (true);
  }

  displayWrite("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(3000);
  }
  printWiFiStatus();

  Udp.begin(port);
  Udp.setTimeout(10);
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  displayWrite("connected");
  Serial.print("IP Address: ");
  Serial.println(ip);
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();

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
      displayMsg(line);
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

void displayMsg(String msg) {
   display.setFont(&Open_Sans_Light_14);

  display.clearDisplay();
//  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 14);
  display.println(Udp.remoteIP());
  display.println(msg);
  display.display();
}

void displayWrite(String message) {
   display.setFont(&Open_Sans_Light_14);

  display.clearDisplay();
//  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 12);
  display.println(message);
  display.display();
}
