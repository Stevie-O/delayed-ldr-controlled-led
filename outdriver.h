#ifndef __OUTDRIVER_H__
#define __OUTDRIVER_H__

/** outdriver.h
 *  Copyright (c) 2017 Stephen Oberholtzer, all rights reserved.
 *  Published under the MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
#define ODSC_OUTPUT_ON    0x0001
#define ODSC_OUTPUT_OFF   0x0002

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

