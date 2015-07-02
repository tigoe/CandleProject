
#include <Adafruit_NeoPixel.h>

const int neoPixelPin = 0;
const int numPixels = 3;

int red, green, blue;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

void setup()  {
  strip.begin();                          // initialize pixel strip
  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, 255, 0, 0);  // turn off pixel
    strip.show();                         // refresh the strip
  }
  delay(1000);    // delay to see the LEDs reset before main loop
  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, 0, 255, 0);  // turn off pixel
    strip.show();                         // refresh the strip
  }
  delay(1000);
  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, 0, 0, 255);  // turn off pixel
    strip.show();                         // refresh the strip
  }
  delay(1000);
red = random(255);
green = random(255);
blue = random(255);
}

void loop() {

if (red >= 255) {
  red = 0;
} else {
  red++;
}

if (green >= 255) {
  green = 0;
} else {
  green++;
}

if (blue >= 255) {
  blue = 0;
} else {
  blue++;
}
  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, red, green, blue);  // turn off pixel
    strip.show();                         // refresh the strip
    delay(1);
  }

  // update the strip:
  strip.show();
}

