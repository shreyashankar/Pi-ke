#include "armtimer.h"

typedef volatile struct {
    unsigned IRQ_basic_pending;
    unsigned IRQ_pending_1;
    unsigned IRQ_pending_2;
    unsigned FIQ_control;
    unsigned Enable_IRQs_1;
    unsigned Enable_IRQs_2;
    unsigned Enable_Basic_IRQs;
    unsigned Disable_IRQs_1;
    unsigned Disable_IRQs_2;
    unsigned Disable_Basic_IRQs;
} rpi_irq_controller_t;

//COPIED FROM CS107E GITHUB, ADDED A FUNCTION TO DISABLE INTERRUPTS FOR EXTENSION

// p 196
typedef volatile struct {
    unsigned load;
    unsigned value;                 // read only
    struct {
        unsigned : 1,
                 counter_is_23_bit: 1,  // 1 = 23 bit, 0 = 16
                 prescale: 2,           // 00 = clock / 1
                 // 01 = clock/16
                 // 10 = clock / 256
                 // 11 = clock / 1 [undef?]
                 : 1,
                 enable_interrupt: 1,            // 1 = timer int enabled
                 : 1,
                 enable_timer: 1,
                 run_in_debug: 1,        // 1 = run timer in debug
                 disable_free_timer: 1,
                 : 6,
                 free_counter_scale: 8;    // clk/(prescale+1)
    } control;
    unsigned irq_clear;             // write only
    unsigned raw_irq;               // read only
    unsigned masked_irq;            // read only
    unsigned reload;
    unsigned pre_div;               // not in real 804. [means?]
    unsigned free_counter;          // free running counter not in 804?
} arm_sys_timer_t;

#define ARM_TIMER_LOD 0x2000B400
#define ARM_TIMER_VAL 0x2000B404
#define ARM_TIMER_CTL 0x2000B408
#define ARM_TIMER_DIV 0x2000B41C
#define ARM_TIMER_CNT 0x2000B420
#define RPI_INTERRUPT_CONTROLLER_BASE   0x2000B200

volatile rpi_irq_controller_t *rpi = (void*)RPI_INTERRUPT_CONTROLLER_BASE;
#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_ARMTIMER_BASE               0x2000B400
volatile arm_sys_timer_t *sys_timer = (void*)RPI_ARMTIMER_BASE;

void armtimer_init(void) {
    unsigned* ctl = (unsigned*)ARM_TIMER_CTL;
    *ctl = 0x00000000;
    *ctl = 0x00000200;
}

unsigned armtimer_get_time(void) {
    return *((unsigned*)ARM_TIMER_CNT);
}

//code I added
void armtimer_stop() {
    sys_timer->control.enable_interrupt = 0;
}

#define AssertNow(x) switch(1) { case (x): case 0: ; }
void armtimer_start(unsigned interval) {
    AssertNow(sizeof sys_timer->control == 4);
    sys_timer->control.enable_timer = 1;
    sys_timer->load = interval;
    sys_timer->control.enable_interrupt = 1;
    sys_timer->control.prescale = 0b10;
    rpi->Enable_Basic_IRQs = 1;
}

void armtimer_clear_interrupt() {
    sys_timer->irq_clear = 1;
}