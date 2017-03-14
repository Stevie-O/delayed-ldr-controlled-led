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

Currently, this is a prototype for the LED driver logic.  To try it out, compile+upload and open the Serial Monitor.
Each letter A-F/a-f will run a script:

A/a - turn on LED
B/b - turn off LED
C/c - blink LED twice, then turn it on and leave it on for 100ms
D/d - turn off LED, wait for a random period of time from 0-15s, then turn on LED
E/e - turn on LED and leave it on for 1s
F/f - turn off LED and leave it off for 1s

A capital letter will always replace the current script.
A lowercase letter will replace the current script if it was run by a lowercase letter.
A lowercase letter that follows a capital letter will wait until the capital letter script finishes first.

To see this in action, you can try this:

* `Ed` - (E) turn on LED for one second, then (d) turn off LED for 0-15s, then turn on LED
* `Fc` - (F) turn off LED for one second, then (c) blink LED twice, then turn on LED
* `Cb` - (C) Turn off LED for 100ms, blink LED on once, turn on LED for 100ms, then (b) turn off LED (effectively, blinking twice)

