/*
  This sketch makes 100 NeoPixels fade in a candle-like behavior.

  created 15 Jun 2015
  modified 16 Jul 2015
  by Tom Igoe

 */
#include <Adafruit_NeoPixel.h>
#include <Console.h>
#include <Bridge.h>
#include <Process.h>

String macAddr = "00:00:00:00:00:00";
char bridgeData[10];
long tenMinutes = 600000;
long loginInterval = 10000;
long lastNetworkMsg = 0;
long lastLogin = 0;
Process udpBridge;

const int neoPixelPin = 3;    // pin that's controlling the neoPixels
const int numPixels = 100;    // count of neoPixels

// initialize neoPixel strip:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_RGB + NEO_KHZ800);

// changing range of keyframe colors for the pixels to flicker to:
unsigned long keyColors[] = {0x3E0701, 0x3A0902, 0x2F0F02, 0x370E05};
unsigned long burstColor = 0xFF6633;

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
  Console.println("Starting");
  strip.begin();    // initialize neoPixel strip
  // might be able to do this on Yun startup:
  udpBridge.runShellCommandAsynchronously("python /usr/lib/python2.7/bridge/udpBridge.py");

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
  long currentTime = millis();
  // read for new data:
  readClient();

  // create the flicker effect:
  if (millis() % flickerInterval < 3) {
    flickerPixels();
    randomSeed(currentTime + analogRead(A0));   // reset the randomseed
  }

  if (online == false) {
    strip.setPixelColor(0, 0, 0, 34);  // turn first pixel to low blue
    // attempt to log in:
    if (currentTime - lastLogin > loginInterval) {
      login();
    }
  } else {
    // if you're online and  haven't gotten a server message in ten minutes,
    // assume the server's offline and let the candle controller know:
    if (currentTime - lastNetworkMsg >= tenMinutes) {
      Console.println("~~~");
      online = false;
    }
  }

  // update the strip:
  strip.show();
}

/*
  this function reads client input and interprets it
 */

void readClient() {
  Bridge.get("in", bridgeData, 10);
  String result = String(bridgeData);
  if (result.length() > 0) {
    for (int c = 0; c < result.length(); c++) { // read all bytes of bridgeData
      char input = result[c];
      Console.print(input);
      switch (input) {
        case '*':    // do a twinkle
          twinkleChase();
          Console.println("twinkle");
          lastNetworkMsg = millis();
          break;
        case '!':    // set status to online
          online = true;
          lastNetworkMsg = millis();
          break;
        case '~':    // set status to offline
          lastNetworkMsg = millis();
          online = false;
          break;
        default:  // placeholder for other options here
          break;
      }
    }
    Bridge.put("in", "");
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

void sendPacket(String message) {
  Process udp;
  String  cmd = "python /usr/lib/python2.7/bridge/udpClient.py 192.168.1.2 \"";
  cmd += message;
  cmd += "\"";
  udp.runShellCommand(cmd);
}

boolean login() {
  Console.println("logging in...");
  getMacAddress();
  // send the MAC address to the server by way of greeting:
  sendPacket(macAddr);
  lastLogin = millis();
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

    strip.setPixelColor(pixel, burstColor);  // turn pixel white
    for (int n = 0; n < 9; n++) {            // light nine pixels at a time
      if (pixel + n < numPixels) {
        strip.setPixelColor(pixel + n, burstColor); // turn pixel white
      }
    }
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

