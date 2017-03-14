/** ldrmon.cpp
    LDR monitoring subsystem

    Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
    Published under the MIT license (see LICENSE.md)
*/

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include "ldrmon.h"
#include "outdriver.h"
#include "config.h"

// must use this for conversion of durations larger than about 1000ms, due to an integer overflow bug
#define pdMS_TO_TICKS_LONG( xTimeInMs ) ( (TickType_t) ( ( (uint32_t) ( xTimeInMs ) * (uint32_t) configTICK_RATE_HZ) / (uint32_t) 1000 ) )

typedef enum {
  /** External lights are ON */
  HOUSE_LIGHTS_ON,
  /** External lights are OFF */
  HOUSE_LIGHTS_OFF,
  /** Calibration mode; external lights are ON */
  CALIBRATION_LIGHTS_ON,
  /** Calibration mode; external lights are OFF */
  CALIBRATION_LIGHTS_OFF,
} ldrmon_state_t;

static const char* ldrmon_state_names[] = { "HOUSE_LIGHTS_ON", "HOUSE_LIGHTS_OFF", "CALIBRATION_LIGHTS_ON", "CALIBRATION_LIGHTS_OFF", };

/************************ OUTPUT DRIVER SCRIPTS ************************/

/** Script run when house lights go off in calibration mode. */
static const output_driver_script cm_hl_turned_off[] = { ODSC_OUTPUT_ON, ODSC_END };
/** Script run when house lights come on in calibration mode. */
static const output_driver_script cm_hl_turned_on[] = { ODSC_OUTPUT_OFF, ODSC_END };
/** Script run when calibration mode starts with house lights on */
// With HL *on*, the output driver should currently be OFF.
static const output_driver_script cm_start_with_hl_on[] = {
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // blink
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  //
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // blink again
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  //
  ODSC_END
};
/** Script run when calibration mode starts with house lights off */
// With HL *off*, the output driver may be on, or it may be off.
static const output_driver_script cm_start_with_hl_off[] = {
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // ensure ON for at least 100ms
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // blink off for 100ms
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // back on
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // blink off for 100ms
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // back on
  ODSC_END
};

/** Script run when house lights go on outside calibration mode */
#define hl_turned_on cm_hl_turned_off
/** Script to run when exiting calibration mode with house lights ON */
#define cm_end_with_hl_on cm_start_with_hl_on
/** Script to run when exiting calibration mode with house lights OFF */
// With HL *off*, the ouptut driver will be ON
static const output_driver_script cm_end_with_hl_off[] = {
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // blink off for 100ms
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // back on
  ODSC_OUTPUT_OFF, ODSC_DELAY(pdMS_TO_TICKS(100)),  // blink off for 100ms
  ODSC_OUTPUT_ON, ODSC_DELAY(pdMS_TO_TICKS(100)),   // back on
  ODSC_OUTPUT_OFF,                                  // aaand off again
  ODSC_END
};
/** Script run when house lights go off outside calibration mode */
static const output_driver_script hl_turned_off[] = {
  ODSC_OUTPUT_OFF,
  ODSC_DELAY_RANDOM(pdMS_TO_TICKS_LONG(15000)),
  ODSC_OUTPUT_ON,
  ODSC_END
};

/***********************************************************************/

static ldrmon_state_t ldrmon_state;
static int16_t threshold;
static TimerHandle_t calib_end_timer;

static void calib_end_callback(TimerHandle_t unused);

void ldrmon_init(void) {
  ldrmon_state = HOUSE_LIGHTS_ON;
  threshold = UINT16_MAX;
  calib_end_timer = xTimerCreate(
                      "CALIB",
                      pdMS_TO_TICKS_LONG(CALIBRATION_MODE_TIMEOUT),
                      pdFALSE, // one-shot
                      0, // timer ID (not used)
                      &calib_end_callback);
  xTimerStop(calib_end_timer, UINT16_MAX);
}

static void calib_end_callback(TimerHandle_t unused) {
  ldrmon_exit_calibration();
}

static void ldrmon_set_state(ldrmon_state_t newState, const output_driver_script *script)
{
  Serial.print("State changed to "); Serial.println(ldrmon_state_names[newState]);
  ldrmon_state = newState;
  output_driver_run_script(script, ODS_PRIO_LOW);
}

void ldrmon_ldr_changed(int16_t ldr) {
  switch (ldrmon_state) {
    case HOUSE_LIGHTS_OFF:
      if (ldr < threshold - HYSTERESIS_ON)
        ldrmon_set_state(HOUSE_LIGHTS_ON, hl_turned_on);
      break;
    case HOUSE_LIGHTS_ON:
      if (ldr >= threshold + HYSTERESIS_OFF)
        ldrmon_set_state(HOUSE_LIGHTS_OFF, hl_turned_off);
      break;
    case CALIBRATION_LIGHTS_OFF:
      if (ldr < threshold)
        ldrmon_set_state(CALIBRATION_LIGHTS_ON, cm_hl_turned_on);
      break;
    case CALIBRATION_LIGHTS_ON:
      if (ldr >= threshold)
        ldrmon_set_state(CALIBRATION_LIGHTS_OFF, cm_hl_turned_off);
      break;
  }
}

/** Sets the threshold for detecting house lights on/off
*/
void ldrmon_set_threshold(int16_t new_threshold) {
  if (threshold == new_threshold) return;
  Serial.print("LDR threshold changed to "); Serial.println(new_threshold);
  threshold = new_threshold;
  // automatically enter calibration mode as soon as the threshold changes
  ldrmon_start_calibration();
}

/** Enters calibration mode.
   In calibration mode, all hysteresis and delays to the output driver are disabled.
*/
void ldrmon_start_calibration(void) {
  if (ldrmon_state < CALIBRATION_LIGHTS_ON) {
    // start timer
    xTimerStart(calib_end_timer, UINT16_MAX);
    if (ldrmon_state == HOUSE_LIGHTS_OFF)
      ldrmon_set_state(CALIBRATION_LIGHTS_OFF, cm_start_with_hl_off);
    else
      ldrmon_set_state(CALIBRATION_LIGHTS_ON, cm_start_with_hl_on);
  }
}

/** Exits calibration mode.
*/
void ldrmon_exit_calibration(void) {
  xTimerStop(calib_end_timer, UINT16_MAX);
  switch (ldrmon_state) {
    case CALIBRATION_LIGHTS_ON:
      output_driver_run_script(cm_end_with_hl_on, ODS_PRIO_HIGH);
      ldrmon_set_state(HOUSE_LIGHTS_ON, hl_turned_on);
      break;
    case CALIBRATION_LIGHTS_OFF:
      output_driver_run_script(cm_end_with_hl_off, ODS_PRIO_HIGH);
      ldrmon_set_state(HOUSE_LIGHTS_OFF, hl_turned_off);
      break;
  }
}

