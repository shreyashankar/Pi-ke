/* 
 * Functions for Raspberry Pi ARM Timer.
 * 
 * Author: Pat Hanrahan <hanrahan@cs.stanford.edu>
 *         Dawson Engler <engler@cs.stanford.edu>
 * 
 * Date: April 20, 2016
*/
#include "register.h"
#include "armtimer.h"

/*
 * See BCM p 196 (unfortunately the documentation is wrong in several places)
 *
 * This timer has two parts: a free running counter and a count-down timer.
 * The timer is always running in free running mode.
 *
 * In this library, we implement the count-down timer
 *
 * The timer and counter are based on the APB clock, which on the
 * Raspberry Pi is 250 Mhz
 * 
 * Some errors in the documentation:
 *  (1) the counters are either 32-bit (not 23-bit) or 16-bit
 *  (2) the effect of pre-scale bits (3:2) in the control register is unclear.
 */

typedef volatile struct {
    unsigned load;                  // writing immediately loads the counter
    unsigned value;                 // current counter value, read only
    struct {
        unsigned :1,
        timer_is_32_bit:1,          // 0 -> 16-bit, 1 -> 32 bit
        :3,
        enable_timer_interrupt:1,   // 1 = timer interrupts enabled
        :1,
        enable_timer:1,             // 1 - timer enabled
        run_in_debug:1,             // 1 = run timer in arm debug mode/halt
        free_counter_enable:1,      // 1 = free running counter enabled
        :6,
        free_counter_prescale:8;    // prescalar for the free running counter
                                    // clk/(prescale+1)
    } control;
    unsigned irq_clear;             // clear irq, write only
    unsigned overflow;              // irq pending & enabled, read only
    unsigned irq;                   // timer interrupt request, read only
    unsigned reload;                // this value is laoded when value == 0
    unsigned prescale;              // prescaler for the timer clk/(prescale+1)
    unsigned free_counter_value;    // free running counter value
} armtimer_t;

#define ARMTIMER_BASE 0x2000B400;
volatile armtimer_t *armtimer = (void*)ARMTIMER_BASE;

/* Cute hack: compile-time assertion . */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

/*
 * initialize armtimer
 *
 *  ticks is the number of usecs between overflows/interrupts
 *
 * timer is initialized, but not enabled. interrupts are disabled.
 * 
 */
void  armtimer_init(unsigned ticks) 
{ 
    // make sure bit-fields are within 1 word
    AssertNow(sizeof armtimer->control == 4);

    armtimer->control.enable_timer = 0;
    armtimer->control.enable_timer_interrupt = 0; 
    armtimer->control.timer_is_32_bit = 1;

    // setup up timer it counts once per microsecond
    armtimer->prescale = (250-1); // 250 000 000 / 250 = 1 000 000 = 1 usec

    // ticks are number of microseconds between overflows/interrupts
    armtimer->load = ticks-1;
    armtimer->reload = ticks-1;
}

/*
 * set timer prescalar (apbclock/divisor)
 *
 * the timer counts at a rate equal to the ABP clock divided by the divisor.
 * the APB clock on the Raspberry Pi runs at 250 Mhz.  thus, if the divisor 
 * equals 250, the timer will count once per usec.
 *
 * The prescale register is 10-bits
 *
 * The reset value of this register is 0x7D so gives a divide by 126.
 */
void armtimer_set_prescalar(unsigned divisor)
{
    armtimer->prescale = divisor-1;
}

/*
 * set load register. 
 *
 *  this value is immediately loaded into the counter.
 */
void armtimer_set_load(unsigned ticks)
{
    armtimer->load = ticks-1;
}

/*
 * set reload register. 
 *
 *  this value is loaded into the counter when it hits 0.
 */
void armtimer_set_reload(unsigned ticks)
{
    armtimer->reload = ticks-1;
}

unsigned armtimer_get_time(void)
{
    return armtimer->value;
}

void armtimer_enable(void)
{
    armtimer->control.enable_timer = 1;
}

void armtimer_disable(void)
{
    armtimer->control.enable_timer = 0;
}

void armtimer_enable_interrupt(void)
{
    armtimer->control.enable_timer_interrupt = 1;
}

void armtimer_disable_interrupt(void)
{
    armtimer->control.enable_timer_interrupt = 0;
}

unsigned armtimer_check_overflow(void)
{
    return armtimer->overflow;
}

unsigned armtimer_check_interrupt(void)
{
    return armtimer->irq;
}

void armtimer_clear_interrupt(void)
{
    armtimer->irq_clear = 1; 
}