/*
  This sketch makes a softwareSerial connection on pins 3 and 4
  It listens for a string in the serial port in the following form:
  Crrr,ggg,bbb\n

  It sets three NeoPixels with the color values rrr, ggg, bbb based
  on the input.

  created 26 Jun 2015
  by Tom Igoe

 */


#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

const int neoPixelPin = 2;
const int pixelCount = 3;

SoftwareSerial mySerial(3, 4); // RX, TX

Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, neoPixelPin, NEO_RGB + NEO_KHZ800);

int red;
int green;
int blue;

void setup()
{
  // set the data rate for the SoftwaremySerial port
  mySerial.begin(9600);
  mySerial.setTimeout(10);  // set serial timeout
  strip.begin();          // initialize pixel strip
  strip.show();           // Initialize all pixels to 'off'
  for (int pixel = 0; pixel < pixelCount; pixel++) {
    strip.setPixelColor(pixel, 0, 0, 0);// set the color for this pixel
    strip.show();                                // refresh the strip
  }
}

void loop() {
  // listen for mySerial:
  if (mySerial.available() > 0) {
    if (mySerial.read() == 'C') {    // string should start with C
      red = mySerial.parseInt();     // then an ASCII number for red
      green = mySerial.parseInt();   // then an ASCII number for green
      blue = mySerial.parseInt();    // then an ASCII number for blue

      for (int pixel = 0; pixel < pixelCount; pixel++) {
        strip.setPixelColor(pixel, red, green, blue);// set the color for this pixel
        strip.show();                                // refresh the strip
      }
    }
  }
}
