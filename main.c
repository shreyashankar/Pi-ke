#include "gpio.h"
#include "interrupts.h"
#include "pin_constants.h"
#include "display.h"
#include "interrupt_handlers.h"
#include "gyro.h"

double speed = 0;    //mph
double distance = 0; //miles
int mode = 0;
double acceleration = 0;

static void setup_lights() {
  gpio_set_output(LEFT_INDICATOR_LIGHT_PIN);
  gpio_set_output(RIGHT_INDICATOR_LIGHT_PIN);
  gpio_set_output(BRAKE_LIGHT_PIN);
}


void main(void) {
  
  gyro_init();
  display_init();
  setup_lights();
  setup_interrupts();
  system_enable_interrupts();

  display_run();
  
}
