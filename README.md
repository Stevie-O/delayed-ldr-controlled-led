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


The sketch
==========

Currently, this is a test for monitoring the LDR.
It continously monitors the LDR input and writes to the serial port every time the value changes.
