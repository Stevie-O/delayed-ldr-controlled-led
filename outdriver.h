#ifndef __OUTDRIVER_H__
#define __OUTDRIVER_H__

#include "config.h"

/** outdriver.h
 *  Header file for the output driver subsystem.
 *  
 *  Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
 *  Published under the MIT license (see LICENSE.md)
 */

/** Script command types:
 *  - Delay a specified amount of time  (0x8000 | x) where x is a 14-bit unsigned integer containing the number of ticks to delay
 *  - Delay a random amount of time, up to a specified limit (0xC000 | x) where x is a 14-bit unsigned integer containing the maximum number of ticks to delay
 *  - Output ON (0x0001)
 *  - Output OFF (0x0002)
 *  - End of script (0x0000)
 */

/* Output driver script commands */
#define ODSC_DELAY_MASK        0x8000
#define ODSC_DELAY_RANDOM_MASK 0x4000
#define ODSC_DELAY_VALUE_MASK 0x3FFF
#define ODSC_DELAY(x)         (ODSC_DELAY_MASK | (x))
#define ODSC_DELAY_RANDOM(x)  (ODSC_DELAY_RANDOM_MASK | ODSC_DELAY(x))

#define ODSC_IS_DELAY(x)  ((x) & ODSC_DELAY_MASK)
#define ODSC_IS_DELAY_RANDOM(x) ((x) & ODSC_DELAY_RANDOM_MASK)
#define ODSC_GET_DELAY(x) ((x) & ODSC_DELAY_VALUE_MASK)

#define ODSC_END          0x0000
#define ODSC_OUTPUT_ON    ((0x0002)|(OUTPUT_DRIVER_ACTIVE_LEVEL))
#define ODSC_OUTPUT_OFF   ((0x0002)|(OUTPUT_DRIVER_ACTIVE_LEVEL^HIGH))

#if OUTPUT_DRIVER_ENABLE_PWM
#define ODSC_OUTPUT_HALF  0x0005
#else
#define ODSC_OUTPUT_HALF  ODSC_OUTPUT_ON
#endif


#define ODS_PRIO_HIGH   2
#define ODS_PRIO_LOW    1

typedef uint16_t output_driver_script;

/** Call to initialize the output driver code. */
void output_driver_init(void);

/** Call to run a script against the output driver.
 *
 * @param script The script to be run.
 * @param priority The priority of the script (ODS_PRIO_HIGH or ODS_PRIO_LOW). 
 *                 If the new script is of greater or equal priority to the currently running script,
 *                 the currently running script will be replaced.  Otherwise, the new script will be
 *                 queued until the current script finishes running.
 */
void output_driver_run_script(const output_driver_script *script, uint8_t priority);

#endif // __OUTDRIVER_H__

