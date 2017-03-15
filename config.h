#ifndef __CONFIG_H__
#define __CONFIG_H__

/** config.h
 *  Configuration parameters for the Delayed LDR-Controlled LED sketch
 */


// output pin for driving LEDs
#define   OUTPUT_DRIVER_PIN   9
// if you have an active-low output pin, change this to LOW
#define OUTPUT_DRIVER_ACTIVE_LEVEL  HIGH

#define OUTPUT_DRIVER_ENABLE_PWM    1

// analog pin for the light sensor
#define PIN_LIGHT_SENSOR  A5
// analog pin for the calibration pot
#define PIN_THRESHOLD     A0

// threshold hysteresis: must move at least 3 units for it to register
#define THRESHOLD_HYSTERESIS  4

// it is assumed that LDR has one end connected to ground, in a 'maximum brightness = 0V' configuration

// to register 'house lights are now on', LDR voltage needs to drop below (threshold - HYSTERESIS_ON)
#define HYSTERESIS_ON   0
// to register 'house lights are now off', LDR voltage needs to rise above (threshold + HYSTERESIS_OFF)
#define HYSTERESIS_OFF  5

// threshold divisor
//  If we're using a separate 
#define THRESHOLD_DIVISOR 2

// calibration mode idle timeout (in milliseconds)
// after no changes have been made to the configured threshold for that long,
// we automatically exit calibration mode.
#define CALIBRATION_MODE_TIMEOUT  5000





#endif // __CONFIG_H__

