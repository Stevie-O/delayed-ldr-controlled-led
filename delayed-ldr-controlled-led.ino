/** delayed-ldr-controlled-led
 *  Main Arduino sketch file.
 *  
 *  Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
 *  Published under the MIT license (see LICENSE.md)
 */

#include <Arduino_FreeRTOS.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "config.h"
#include "outdriver.h"
#include "ldrmon.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("OK");

  pinMode(PIN_LIGHT_SENSOR, INPUT);
  pinMode(PIN_THRESHOLD, INPUT);

  output_driver_init();
  ldrmon_init();

  xTaskCreate(
       ldr_watch_task,
       (const portCHAR *)"LDR",
       128, // Stack size = 128 words (256 bytes)
       NULL, // ???
       2, // Priority (medium)
       NULL
     );
}

void loop() {
}

void ldr_watch_task(void *unused)
{
  Serial.println("ldr_watch_task");
  int prev_ldr = -1;
  int prev_threshold = -1;
  for (;;)
  {
    int threshold = analogRead(PIN_THRESHOLD);
    if (threshold != prev_threshold)
      ldrmon_set_threshold(threshold / THRESHOLD_DIVISOR);
    int ldr = analogRead(PIN_LIGHT_SENSOR);
    if (ldr != prev_ldr || threshold != prev_threshold)
      ldrmon_ldr_changed(ldr);

    prev_threshold = threshold;
    prev_ldr = ldr;
  }
}

void vApplicationIdleHook(void) {
  sleep_enable();

  sleep_mode();

  sleep_disable();
}

