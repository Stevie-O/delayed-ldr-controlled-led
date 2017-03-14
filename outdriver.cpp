/** outdriver.cpp
 *  Output driver subsystem.
 *  
 *  Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
 *  Published under the MIT license (see LICENSE.md)
 */

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "outdriver.h"
#include "config.h"

#define DEBUG_PRINT   while(0)Serial.print
#define DEBUG_PRINTLN while(0)Serial.println

#define ODS_PRIO_NONE 0

static void output_driver_task(void *unused);

static volatile const output_driver_script *cur_script = NULL;
static volatile uint8_t cur_script_prio = ODS_PRIO_NONE;

static volatile const output_driver_script *next_script = NULL;
static volatile uint8_t next_script_prio = ODS_PRIO_NONE;

static TaskHandle_t h_output_driver_task = NULL;

void output_driver_init(void)
{
  pinMode(OUTPUT_DRIVER_PIN, OUTPUT);
  digitalWrite(OUTPUT_DRIVER_PIN, LOW);

  xTaskCreate(
                           output_driver_task,
                           (const portCHAR *)"ODS",
                           128, // Stack size = 128 words (256 bytes)
                           NULL, // ???
                           3, // Priority (high)
                           &h_output_driver_task
                         );
}

/* if we were doing more work, or we needed higher levels of concurrency,
   I'd use a proper semaphore/mutex here.  But we don't, so there's no point
   wasting the RAM or code space.
*/
#define cur_script_lock   noInterrupts
#define cur_script_unlock interrupts

void output_driver_run_script(const output_driver_script *script, uint8_t priority)
{
  /* we must not queue an empty script, it will break get_next_script_command() */
  if (script == NULL || *script == ODSC_END)
    return;

  bool notify_parent = false;
  cur_script_lock();
  // if the priority is greater than or equal to the currently running script,
  // we replace the running script right now
  if (priority >= cur_script_prio)
  {
    cur_script = script;
    cur_script_prio = priority;
    notify_parent = true;
    goto out;
  }
  // ok, well, if the priority is greater than or equal to the queued script,
  // we replace the queued script
  if (priority >= next_script_prio)
  {
    next_script = script;
    next_script_prio = priority;
  }
out:
  cur_script_unlock();

  if (notify_parent)
    xTaskNotifyGive(h_output_driver_task);
    }

  /** Gets the next script command to be executed. */
  static output_driver_script get_next_script_command(void)
{
  output_driver_script ret;

  cur_script_lock();
  if (cur_script == NULL)
  {
    cur_script_unlock();
    return ODSC_END;
  }
  ret = *cur_script++;
  if (ret == ODSC_END) {
    // end of the current script
    if (next_script_prio > ODS_PRIO_NONE) {
      // but there's one queued -- run it now
      cur_script = next_script;
      cur_script_prio = next_script_prio;
      next_script_prio = ODS_PRIO_NONE;
      // note that ret will never be ODSC_END here
      // we blocked that at the beginning of output_driver_run_script()
      ret = *cur_script++;
    } else {
      // there is no longer a current script queued
      cur_script_prio = ODS_PRIO_NONE;
    }
  }
  cur_script_unlock();
  return ret;
}

static void output_driver_task(void *unused)
{
  for (;;)
  {
    output_driver_script cmd = get_next_script_command();
    DEBUG_PRINTLN(cmd, HEX);
    if (ODSC_IS_DELAY(cmd)) {
      // process a 'delay' command
      uint16_t delay_value = ODSC_GET_DELAY(cmd);
      if (ODSC_IS_DELAY_RANDOM(cmd)) {
        DEBUG_PRINTLN("ODSC_DELAY_RANDOM");
        delay_value = random() % delay_value;
      } else {
        DEBUG_PRINTLN("ODSC_DELAY");
      }
      DEBUG_PRINT("Delaying "); DEBUG_PRINTLN(delay_value);
      ulTaskNotifyTake(pdTRUE, delay_value);
    } else {
      // process a non-delay command
      switch (cmd & 0xFF) {
        case ODSC_END:
          DEBUG_PRINTLN("ODSC_END");
          // wait indefinitely
          ulTaskNotifyTake(pdTRUE, (TickType_t) - 1);
          break;
        case ODSC_OUTPUT_ON:
        case ODSC_OUTPUT_OFF:
          DEBUG_PRINTLN( (cmd & 1) ? "ODSC_OUTPUT_ON" : "ODSC_OUTPUT_OFF");
          digitalWrite(OUTPUT_DRIVER_PIN, (cmd & 1));
          break;
      }
    }
  } // infinite for(;;) loop
}

