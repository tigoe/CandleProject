/*
  Simple BlinkyStrip control

  This sketch allows Console control over a BlinkyStrip.
  To set any pixel in the strip, send the following Console string:

  Cr,g,b

  r, g, and b are ASCII numbers for the red, green, and blue brightness
  levels, from 0 to 255.

  This will probably work for any string of NeoPixels from Adafruit as well.

  Uses Adafruit's NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel

  created 4 Dec 2014
  by Tom Igoe

*/
#include <Adafruit_NeoPixel.h>
#include <Bridge.h>
#include <Console.h>

#define PIN 3
const int numPixels = 100;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, PIN, NEO_RGB + NEO_KHZ800);

char dataBuffer[4];      // buffer for receiving the data. You need an extra byte at the end, so 4 bytes to get 3
int red, green, blue;    // ints to set the LED brightnesses
String dataString;       // a string for the buffer-to-int conversion

int pixel = 0;            // pixel number that you're changing

void setup() {
  // initiate Bridge connection to linux processor:
  Bridge.begin();
  Console.begin();     // initialize Console communication
  strip.begin();          // initialize pixel strip
  strip.show();           // Initialize all pixels to 'off'
  Console.println("Hello");
  for (pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, 0, 0, 0);// set the color for this pixel
    strip.show();                                // refresh the strip
  }
}

void loop() {
  Bridge.get("red", dataBuffer, 3);  // get the red value
  dataString = String(dataBuffer);   // convert to a String
  red = dataString.toInt();          // convert String to int

  Bridge.get("green", dataBuffer, 3);// get the green value
  dataString = String(dataBuffer);   // convert to a String
  green = dataString.toInt();        // convert String to int

  Bridge.get("blue", dataBuffer, 3);  // get the blue value
  dataString = String(dataBuffer);    // convert to a String
  blue = dataString.toInt();          // convert String to int

  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, red, green, blue);// set the color for this pixel
  }
  Console.print(red, HEX);
  Console.print(" ");
  Console.print(green, HEX);
    Console.print(" ");
  Console.println(blue, HEX);
  strip.show();                                // refresh the strip
  delay(30);
}



