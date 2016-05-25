/* Interrupt Handlers */
#include "gpio.h"

void interrupt_vector(unsigned pc) {
  // FIXME: Handle interrupts.
  if (gpio_read(GPIO_PIN23) == 0) {
  	keyboard_vector(pc);
  }
  else {
  	gprof_vector(pc);
  }                                                                   
}

// The rest of these should never be called
void fast_interrupt_vector(int pc) {}
void reset_vector(int pc) {}
void undefined_instruction_vector(int pc) {}
void software_interrupt_vector(int pc) {}
void prefetch_abort_vector(int pc) {}
void data_abort_vector(int pc) {}
