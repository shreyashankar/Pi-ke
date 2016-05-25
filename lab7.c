#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "circular.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"

void int_handler(unsigned);

// should not get called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}

/* we only enable GPIO interrupts, so just forward it blindly  */
void interrupt_vector(unsigned pc) {
  int_handler(pc);
}

void int_handler(unsigned pc) {
  gpio_check_and_clear_event(GPIO_PIN21);
}

void button_test() {

	gpio_
}

/*
static void setup_interrupts() {
    gpio_detect_falling_edge(GPIO_PIN21);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}
*/ 

void main(void) {
  printf_init();
  while (1) {
    // Do nothing, for now.
  }
}
