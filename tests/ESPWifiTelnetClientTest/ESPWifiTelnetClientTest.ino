/*
  This sketch makes a telnet client on port 8080 to a server
  It echoes any serial bytes to the server, and any server bytes 
  to the serial port

  created 26 Jun 2015
  by Tom Igoe

 */

#include <ESP8266WiFi.h>

char ssid[]     = "ssid";
char password[] = "password";
char host[] = "192.168.0.2";

WiFiClient client;
const int port = 8080;

void setup() {
  Serial.begin(9600);
  delay(10);

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
  delay(100);
  // Use WiFiClient class to create TCP connections
  Serial.print("connecting to ");
  Serial.println(server);

  client.connect(host, port);

  while (!client.connected()) {
    Serial.println("connection failed, trying again");
    delay(2000);
    client.connect(host, port);
  }
  // This will send the request to the server
  client.println("Hello");
}


void loop() {
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    char line = client.read();
    Serial.write(line);
  }

  // Read all the lines of the reply from server and print them to Serial
  while (Serial.available()) {
    char line = Serial.read();
    client.write(line);
  }
}

