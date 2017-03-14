#include <Arduino_FreeRTOS.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "config.h"
#include "outdriver.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Hello, world!");
  Serial.print("sizeof(int) = "); Serial.print(sizeof(int)); Serial.println("");
  Serial.print("sizeof(void*) = "); Serial.print(sizeof(void*)); Serial.println("");

  pinMode(PD3, INPUT);
  pinMode(A5, INPUT);
  pinMode(A0, INPUT);

  output_driver_init();

  TaskHandle_t h_srt = xTaskCreate(
       serial_read_task,
       (const portCHAR *)"SR",
       128, // Stack size = 128 words (256 bytes)
       NULL, // ???
       2, // Priority (medium)
       NULL
     );

  Serial.print("serial_read_task handle: "); Serial.println((uint32_t)h_srt);
}

void loop() {
}

static const output_driver_script script_a[] = { ODSC_OUTPUT_ON, ODSC_END };
static const output_driver_script script_b[] = { ODSC_OUTPUT_OFF, ODSC_END };
static const output_driver_script script_c[] = { ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)), ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)), ODSC_END };
static const output_driver_script script_d[] = { ODSC_OUTPUT_OFF, ODSC_DELAY_RANDOM(pdMS_TO_TICKS(1000)), ODSC_OUTPUT_ON, ODSC_END };

void serial_read_task(void *unused)
{
  Serial.println("serial_read_task");
  for (;;)
  {
    int cmd = Serial.read();
    if (cmd > 0) 
      //Serial.println(cmd);
    switch (cmd) {
      case 'a': output_driver_run_script(script_a, ODS_PRIO_HIGH); break;
      case 'b': output_driver_run_script(script_b, ODS_PRIO_HIGH); break;
      case 'c': output_driver_run_script(script_c, ODS_PRIO_HIGH); break;
      case 'd': output_driver_run_script(script_d, ODS_PRIO_HIGH); break;
    }
    
  }
}

void vApplicationIdleHook(void) {
  sleep_enable();

  sleep_mode();

  sleep_disable();
}

