#include <Arduino_FreeRTOS.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "config.h"
#include "outdriver.h"

// must use this for conversion of durations larger than about 1000ms, due to an integer overflow bug
#define pdMS_TO_TICKS_LONG( xTimeInMs ) ( (TickType_t) ( ( (uint32_t) ( xTimeInMs ) * (uint32_t) configTICK_RATE_HZ) / (uint32_t) 1000 ) )

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("OK");

  pinMode(PD3, INPUT);
  pinMode(A5, INPUT);
  pinMode(A0, INPUT);

  output_driver_init();

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

static const output_driver_script script_a[] = { ODSC_OUTPUT_ON, ODSC_END };
static const output_driver_script script_b[] = { ODSC_OUTPUT_OFF, ODSC_END };
static const output_driver_script script_c[] = { ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // ensure off for at least 100ms
                                                 ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // blink on for 100ms
                                                 ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // blink off for 100ms
                                                 ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // turn on and ensure it remains that way for 100ms
                                                 ODSC_END };
static const output_driver_script script_d[] = { ODSC_OUTPUT_OFF, ODSC_DELAY_RANDOM(pdMS_TO_TICKS_LONG(15000)), ODSC_OUTPUT_ON, ODSC_END };
static const output_driver_script script_e[] = { ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(1000)), ODSC_END };
static const output_driver_script script_f[] = { ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(1000)), ODSC_END };

void ldr_watch_task(void *unused)
{
  Serial.println("ldr_watch_task");
  int prev_ldr = -1;
  for (;;)
  {
    int ldr = analogRead(PIN_LIGHT_SENSOR);
    if (ldr != prev_ldr) {
      Serial.print("LDR now "); Serial.println(ldr);
      prev_ldr = ldr;      
    }
  }
}

void vApplicationIdleHook(void) {
  sleep_enable();

  sleep_mode();

  sleep_disable();
}

