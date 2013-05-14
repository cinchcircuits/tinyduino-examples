
tinyduino-examples
==================

Code from tinyduino examples from maker faire 2013 projects

OK First check in.  Code is sloppy and pins used is not well documented.  Will be cleaned up over time. Including crediting the source for these examples.  I pulled code and modified it. 

BMP085, hih_6130_read and some of the extrac methods in BMP_TMP_HUM will be rewritten to be 1 or more libraries.

As with all well documented sheilds and breakout boards, good exmaple code is available.  Since I am a HUGE believer in bnot reinventing the wheel, I pull from these sources.  I also make sure people get credit for their work as you see in the examples below.

A\_PulseSensor\_03
------------------

This reads the pulse sensor from http://pulsesensor.myshopify.com/ and writes the results to a SD card.

Pulse sensor example:

+ https://code.google.com/p/pulse-sensor/downloads/detail?name=A_PulseSensor_03.pde

tinyduino_leg_pulse
-------------------
This hooks the pulse sensor from http://pulsesensor.myshopify.com/ to a tiny-circuits.com TinyShield 16 Edge LEDs.

Pulse sensor example:

+ https://code.google.com/p/pulse-sensor/downloads/detail?name=A_PulseSensor_03.pde

LED Example code

+ https://bitbucket.org/tamberg/tinycircuits/raw/6fe520a019761098efaa746f7c4690018d81b603/TinyDuinoDarknessGauge/TinyDuinoDarknessGauge.ino
BMP085
------
Example for talking to the BMP085 on the Cinch Circuits Weather Sensor

BMP085 Extended Example Code
by: Jim Lindblom
SparkFun Electronics
date: 1/18/11
license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/

Get pressure and temperature from the BMP085 and calculate altitude.
Serial.print it out at 9600 baud to serial monitor.

hih\_6130\_read
-------------
Example for displaying data for both sensors on the Cinch Circuits Weather Sensor

copyright, Peter H Anderson, Baltimore, MD, Nov, '11
Illustrates how to measure relative humidity and temperature.
You may use it, but please give credit.

BMP\_TMP\_HUM
-----------
Example for displaying data for both sensors on the Cinch Circuits Weather Sensor
Combined the code from BMP085 and hih_6130_read exmaples.

lumodisplay
-----------
Is basically the http://learn.adafruit.com/tsl2561/ displaying on a Adafruit OLED 128x32 SPI display.

xbee_af
-------
Is just a basic xbee example.

All xbee exmaples used code written by D. Thiebaut and Based on a sketch created back in the mists of time by Tom Igoe itself based on Mikal Hart's example.

xbee\_af\_ir
----------
Transmits data read from DHT22, code grabbed from http://learn.adafruit.com/dht

xbee\_af\_tilt
------------
This one is pretty cool.  It uses a tilt sensor, https://www.sparkfun.com/products/10621 , with the code from http://bildr.org/2012/02/rpi-1031_arduino/


xbee\_shield
----------
This is the receiver. It displayes the packets it receives. It uses a arduino wireless shield with the xbee plugged into it.  For a display is uses a Adafruit LCD Shield Kit. http://www.adafruit.com/products/772 And used code from the example there.  http://learn.adafruit.com/rgb-lcd-shield


