#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "circular.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"
#include "pin_constants.h"
#include "timer.h"

#define MIN_INTERVAL 100000

static int left_on;
static int right_on;
static int current_time;

void int_handler(unsigned);

// should not get called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}

/* we only enable GPIO interrupts, so just forward it blindly  */
void interrupt_vector(unsigned pc) {
  indicator_handler(pc);
}

void indicator_handler(unsigned pc) {
  if (timer_get_time() - current_time < MIN_INTERVAL) {
    gpio_check_and_clear_event(LEFT_INDICATOR_BUTTON_PIN);
    gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN);
    current_time = timer_get_time();
    return;
  }

  if (gpio_check_and_clear_event(LEFT_INDICATOR_BUTTON_PIN)) {
    if (right_on) {
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
      right_on = 0;
    }
    if (left_on) {
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
      left_on = 0;
    }
    else {
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 1);
      left_on = 1;
    }
  }

  if (gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN)) {
    if (left_on) {
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
      left_on = 0;
    }
    if (right_on) {
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
      right_on = 0;
    }
    else {
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 1);
      right_on = 1;
    }
  }

  current_time = timer_get_time();
 
}

void button_test() {
  gpio_set_function(GPIO_PIN21, GPIO_FUNC_INPUT);
  gpio_set_pullup(GPIO_PIN21);
  while (gpio_read(GPIO_PIN21) == 1);
  printf("+");
  while (gpio_read(GPIO_PIN21) == 0);
}


static void setup_interrupts() {
    gpio_set_function(LEFT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
    gpio_set_pullup(LEFT_INDICATOR_BUTTON_PIN);
    gpio_detect_falling_edge(LEFT_INDICATOR_BUTTON_PIN);
    //gpio_detect_rising_edge(LEFT_INDICATOR_BUTTON_PIN);
    gpio_set_function(RIGHT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
    gpio_set_pullup(RIGHT_INDICATOR_BUTTON_PIN);
    gpio_detect_falling_edge(RIGHT_INDICATOR_BUTTON_PIN);
    //gpio_detect_rising_edge(RIGHT_INDICATOR_BUTTON_PIN);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}


void main(void) {
  //buffer20 = cir_new();
  //buffer21 = cir_new();
  //printf_init();
  setup_interrupts();
  gpio_set_function(LEFT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
  gpio_set_function(RIGHT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
  current_time = timer_get_time();
  while(1) {

  }
  // while (1) {
  //   if (!cir_empty(buffer20)) {
  //     printf("20: %d\n", cir_dequeue(buffer20));
  //   }
  //   if (!cir_empty(buffer21)) {
  //     printf("21: %d\n", cir_dequeue(buffer21));
  //   }
    
  //   //printf("%d\n", counter);
  //   //button_test()
  // }
}
