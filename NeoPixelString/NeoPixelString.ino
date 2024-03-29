
/*
  This sketch makes 150 NeoPixels fade in a candle-like behavior.

  created 15 Jun 2015
  by Tom Igoe

 */
#include <Adafruit_NeoPixel.h>
#include <Console.h>
#include <YunClient.h>
#include <Process.h>


YunClient client;             // connection to the server
const int port = 8080;        // port for server communication
char host[] = "192.168.1.3";
String macAddr = "00:00:00:00:00:00";

const int neoPixelPin = 3;    // pin that's controlling the neoPixels
const int numPixels = 100;    // count of neoPixels

// initialize neoPixel strip:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_RGB + NEO_KHZ800);

// changing range of keyframe colors for the pixels to flicker to:
//unsigned long keyColors[] = {0xCB500F, 0xB4410C, 0x95230C, 0x853E0B};
unsigned long keyColors[] = {0x3E0701, 0x3A0902, 0x2F0F02, 0x370E05};

unsigned long targetColor[numPixels];    // next target for each pixel
unsigned long pixelColor[numPixels];     // current color for each pixel

// count of keyframe colors:
byte numColors = sizeof(keyColors) / 4;
int flickerInterval = 30;                // in millis, the delay between flicker steps
long lastTwinkle = 0;                    // how long since the last twinkle
boolean online = false;                  // whether the client is online
boolean lightsOn = true;                 // whether the string's on or not

void setup()  {
  Bridge.begin();   // initialize Bridge library
  Console.begin();  // initialize Console library (for debugging)
  strip.begin();    // initialize neoPixel strip


  randomSeed(millis() + analogRead(A0));
  unsigned long testColor = 0xFF0000;   // initial color is red

  //  set the pixels to red, then green, then blue:
  while (testColor > 0) {
    setStringColor(testColor);
    strip.show();   // refresh the strip
    delay(1000);    // delay to see it:
    // shift the color value down one byte:
    testColor = testColor >> 8;
  }

  strip.show();
  twinkleChase();  // run a twinkle through the whole string
  resetString();
  getMacAddress();
  login();         // log into the server
}

void loop() {
  // if you get a socket message, respond:
  if (client.available()) {
    readClient();
  }

  // create the flicker effect:
  if (millis() % flickerInterval < 2 && lightsOn) {
    flickerPixels();
    randomSeed(millis() + analogRead(A0));   // reset the randomseed
  }

  if (!online) {
    strip.setPixelColor(0, 0, 0, 34);  // turn first pixel to low blue
  }

  // update the strip:
  strip.show();

  if (millis() % 4000 < 5) {
    if (!client.connected()) {
      getMacAddress();
      login();
    }
  }
}

/*
  this function reads client input and interprets it
 */

void readClient() {
  char input = client.read();
  switch (input) {
    case '*':    // do a twinkle
      twinkleChase();
      break;
    case '!':    // set status to online
      online = true;
      break;
    case '~':    // set status to offline
      client.stop();
      online = false;
      break;
    default:  // placeholder for other options here
      break;
  }
}

void getMacAddress() {
  Process getMac;
  getMac.runShellCommand("ifconfig wlan0 | grep HWaddr");
  while (getMac.running());
  while (getMac.available()) {
    String trash = getMac.readStringUntil('H');
    trash = getMac.readStringUntil(' ');
    macAddr = getMac.readStringUntil('\n');
  }

}

boolean login() {
  if (!client.connected()) {
    online = false;
    Console.println("connection failed, trying again");
    client.connect(host, port);
  }
  if (client.connected() && online == false) {
    // This will send the IP address to the server
    client.println(macAddr);
    online = true;
  }
}

void resetString() {
  //  set the pixels with colors from the keyColors array:
  for (int pixel = 0; pixel < numPixels; pixel++) {
    pixelColor[pixel] = 0;                           // set the pixel color
    strip.setPixelColor(pixel, pixelColor[pixel]);  
  }
  strip.show();
}

/*
  this function creates the twinkle effect:
*/
//void twinkle() {
//  int whichPixel = random(numPixels);
//  int thisPixel = whichPixel; // pick a random pixel from the visible list
//  pixelColor[thisPixel] = 0xFFDDDD;          // set its color to white
//  lastTwinkle = millis();
//}

/*
  this function shoots a twinkling LED down the whole string:
*/
void twinkleChase() {
  // chase through all the pixels:
  for (int pixel = 0; pixel < numPixels; pixel++) {
    if (pixel > 0) {
      strip.setPixelColor(pixel - 1, pixelColor[pixel]); // reset  lastpixel
    } else {
      strip.setPixelColor(numPixels - 1, pixelColor[pixel]); // reset end pixel
    }

    strip.setPixelColor(pixel, 255, 255, 255);  // turn pixel white
    strip.show();                         // refresh the strip
    delay(10);
  }
}



void setStringColor(unsigned long thisColor) {
  //  set the pixels with colors from the keyColors array:
  for (int pixel = 0; pixel < numPixels; pixel++) {
    pixelColor[pixel] = thisColor;                  // set the pixel color
    strip.setPixelColor(pixel, pixelColor[pixel]);  // set pixel using keyColor
  }
  strip.show();
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

