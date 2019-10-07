# Candle Project	

This is my candle project. We shall see how it develops.

Tom Igoe <br>
26 June 2015

## Theory of Operation

ESP8266 modules handle the network traffic, and communicate serially to the ATTiny85, which controls the WS2812s. When the candle turns on, it turns two pixels to warm fading, and a thirs to blue. Then the ESP8266 attempts to contact the server. When it succeeds, the third pixel is turned to warm fading.

The metal can of the candle forms a capacitive touch sensor. When a touch is detected, the candle flares the pixels to white, and sends a signal to the server. The server then sends the same message to all the candles, causing them all to flare white. They gradually fade back to the warm glow.

## Communication Protocol

All communications are over UDP. When a new client connects, the server sends "Hello!!!" to the client. When any candle sends a message to the server, the server echoes it to all servers that it's seen since launch.

When a candle receives a * message, it flares white. Similarly, when it receives a touch, it sends a * message.

* Programming

The ATTinys must be removed and programmed with a hardware programmer. The ESP modules can be programmed over the serial connection. The ground of the serial connection is the pin closest to the outside edge of the board. You need to press the IO0 button then the reset on the ESP8266 to put it in programming mode. The  indicator light will grow a dim red when you are in programming mode.

