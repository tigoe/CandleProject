
/*
  This sketch makes three NeoPixels fade in a candle-like behavior.

  created 26 Jun 2015
  modified 24 Jul 2015
  by Tom Igoe

 */
#include <CapacitiveSensor.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

const int neoPixelPin = 0;
const int numPixels = 3;

SoftwareSerial mySerial(3, 4); // RX, TX

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

CapacitiveSensor  mySensor = CapacitiveSensor(1, 2);       // 10M resistor between pins 1 & 2, pin 2 is sensor pin

// changing range of keyframe colors for the pixels to flicker to:
unsigned long keyColors[] = {0xCB500F, 0xB4410C, 0x95230C, 0x853E0B};

unsigned long targetColor[numPixels];    // next target for each pixel
unsigned long pixelColor[numPixels];     // current color for each pixel

// count of keyframe colors:
int numColors = sizeof(keyColors) / 4;
boolean online = false;       // whether the client is online
int flickerInterval = 30;     // in millis, the delay between flicker steps
int threshold = 500;          // difference threshold for sensor
int sensingInterval = 250;    // time between local candle twinkles
int networkInterval = 1500;   // time between twinkle messages to the network
long lastTwinkle = 0;         // how long since the last local twinkle
long lastNetworkTwinkle = 0;  // how long since the last network twinkle   
      
void setup()  {
  mySerial.begin(9600);                  // initialize serial
  mySensor.set_CS_Timeout_Millis(50);
  mySensor.set_CS_Timeout_Millis(50);
  strip.begin();                          // initialize pixel strip
  for (int pixel = 0; pixel < numPixels; pixel++) {
    strip.setPixelColor(pixel, 0, 0, 0);  // turn off pixel
    strip.show();                         // refresh the strip
  }

  delay(1000);    // delay to see the LEDs reset before main loop
}

void loop() {
  // if you get a serial x, make with the twinkling:
  if (mySerial.available()) {
    readSerial();
  }

  // create the flicker effect:
  if (millis() % flickerInterval < 2) {
    flickerPixels();
  }

  if (!online) {
    strip.setPixelColor(0, 0, 0, 34);  // turn first pixel to low blue
  }

  // read sensor every half second:
  if (millis() - lastTwinkle > sensingInterval) {       // disable the sensor after a sensing event happens
    long touch =  mySensor.capacitiveSensor(15);
    if (touch > threshold) {
      // if the minimum network interval has passed:
      if (millis() - lastNetworkTwinkle > networkInterval) {
        mySerial.print("*");
        lastNetworkTwinkle = millis();
      }
      twinkle();
    }
  }
  // update the strip:
  strip.show();
}

/*
  this function reads serial input and interprets it
 */

void readSerial() {
  char input = mySerial.read();
  switch (input) {
    case '*':    // do a twinkle
      twinkle();
      break;
    case '!':
      online = true;
      break;
    case '~':
      online = false;
      break;
    default:  // placeholder for other options here
      break;
  }
}

/*
  this function creates the twinkle effect:
*/
void twinkle() {
  int whichPixel = random(numPixels);
  int thisPixel = whichPixel; // pick a random pixel from the visible list
  pixelColor[thisPixel] = 0xFFDDDD;          // set its color to white
  lastTwinkle = millis();
}

/*
  this function creates the flicker effect:
*/
void flickerPixels() {
  // iterate over all pixels:
  for (int thisPixel = 0; thisPixel < numPixels; thisPixel++) {
    // if the target color matches the current color for this pixel,
    // then pick a new target color randomly:
    if (targetColor[thisPixel] == pixelColor[thisPixel]) {
      int nextColor = random(numColors);
      targetColor[thisPixel] = keyColors[nextColor];
    }
    // fade the pixel one step from the current color to the target color:
    pixelColor[thisPixel] = compare(pixelColor[thisPixel], targetColor[thisPixel]);
    // set the pixel color in the strip:
    strip.setPixelColor(thisPixel, pixelColor[thisPixel]);// set the color for this pixel
  }
}

/*
  takes two colors and returns a color that's a point on each axis (r, g, b)
  closer to the target color
*/
unsigned long compare(unsigned long thisColor, unsigned long thatColor) {
  // separate the first color:
  byte r = thisColor >> 16;
  byte g = thisColor >>  8;
  byte b = thisColor;

  // separate the second color:
  byte targetR = thatColor >> 16;
  byte targetG = thatColor >>  8;
  byte targetB = thatColor;

  // fade the first color toward the second color:
  if (r > targetR) r--;
  if (g > targetG) g--;
  if (b > targetB) b--;

  if (r < targetR) r++;
  if (g < targetG) g++;
  if (b < targetB) b++;

  // combine the values to get the new color:
  unsigned long result = ((unsigned long)r << 16) | ((unsigned long)g << 8) | b;
  return result;
}

