LDR-controlled LED with a twist

My friend Phoxtane had an idea for an LDR-controlled LED, but where the LED turns on after a random delay, so when the lights are turned off, different LEDs will come on at different times, like a city at night.

As it stands, this code works with the Robotale Scratch shield for the Arduino Nano, which can be had from Microcenter (with a Nano included!) for a paltry $9.99.

Quick installation steps for FreeRTOS
-------------------------------------

This sketch requires FreeRTOS for Arduino.

More info available here: https://create.arduino.cc/projecthub/feilipu/using-freertos-multi-tasking-in-arduino-ebc3cc

1. In Arduino IDE, choose the menu: Sketch -> Include Library -> Manage Libraries...
2. Type FreeRTOS in the search box
3. Click on the FreeRTOS by Richard Berry, then click the Install button
4. Click Close to get out of the Library Manager.
5. Choose the menu: Sketch -> Include Library, choose FreeRTOS (it'll be near the bottom, in a list "Contributed libraries").

Board Setup
===========

You need:
* An LDR/photoresistor in a voltage divider configuration with one end of the LDR connected to ground and the other end connected to one of the ADC pins on the Arduino, such that in maximum darkness the voltage read is Vcc/2 (so, 2.5V normally.)
* A 3-lead trimpot with one end at GND, one end at Vcc, and the center tap connected to another of the ADC pins on the Arduino.
* An LED connected to one of the digital pins on the Arduino, preferably a PWM pin.

Sketch Setup
============

Edit config.h.
* If you've got an active-low LED output, change `OUTPUT_DRIVER_ACTIVE_LEVEL` to `LOW`.
* Set `OUTPUT_DRIVER_PIN` to the LED output pin.
* Set `PIN_LIGHT_SENSOR` to the pin connected to the LDR.
* Set `PIN_THRESHOLD` to the pin connected to the center tap of the trimpot.

The sketch
==========

This version actually works!