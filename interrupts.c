#include "memory.h"
#include "interrupts.h"

// http://xinu.mscs.mu.edu/BCM2835_Interrupt_Controller
// BCM: p196
typedef volatile struct {
    unsigned pending_basic;;
    unsigned pending1;
    unsigned pending2;
    unsigned fiq_control;
    unsigned enable1;
    unsigned enable2;
    unsigned enable_basic;
    unsigned disable1;
    unsigned disable2;
    unsigned disable_basic;
} interrupt_t;

#define INTERRUPT_CONTROLLER_BASE 0x2000B200

volatile interrupt_t *interrupt = (volatile interrupt_t *)INTERRUPT_CONTROLLER_BASE;


void interrupts_enable_basic(unsigned mask) {
    interrupt->enable_basic |=  mask;
}

void interrupts_disable_basic(unsigned mask) {
    interrupt->disable_basic |= mask;
}

void interrupts_enable(unsigned n) {
    if (n >=  0 && n < 32) {
        interrupt->enable1 |= (1 << n);
    }
    if (n >= 32 && n < 64) {
        interrupt->enable2 |= (1 << (n - 32));
    }
}

void interrupts_disable(unsigned n) {
    if (n >=  0 && n < 32) {
        interrupt->disable1 |= (1 << n);
    }
    if (n >= 32 && n < 64) {
        interrupt->disable2 |= (1 << (n - 32));
    }
}

