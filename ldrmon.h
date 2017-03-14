#ifndef __LDRMON_H__
#define __LDRMON_H__

/** ldrmon.h
 *  Header file for the LDR monitoring subsystem.
 *  
 *  Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
 *  Published under the MIT license (see LICENSE.md)
 */

/** Call to initialize the LDR monitoring subsystem. */
void ldrmon_init(void);


/** Call to initialize the LDR monitoring code. */
void ldrmon_set_threshold(void);


/** Call to report that the observed LDR value (or the threshold) has changed
 *
 * @param ldr       The reading from the analog pin connected to the LDR.
 * @param threshold The reading from the analog pin connected to the calibration pot.
 */

void ldrmon_ldr_changed(int16_t ldr);

/** Sets the threshold for detecting house lights on/off.  Will not take effect
 *  until the next call to ldrmon_ldr_changed().
 */
void ldrmon_set_threshold(int16_t new_threshold);

/** Enters calibration mode.
 * In calibration mode, all hysteresis and delays to the output driver are disabled.
 */
void ldrmon_start_calibration(void);

/** Exits calibration mode.
 */
void ldrmon_exit_calibration(void);

#endif // __LDRMON_H__
