#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "circular.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"
#include "pin_constants.h"
#include "timer.h"
#include "armtimer.h"
#include "printf.h"
#include "i2c.h"
#include "accel.h"
#include "LSM6DS33.h"

#define MIN_INTERVAL 300000
#define TIMER_INTERVAL 10000000

static int left_on;
static int right_on;
static int current_time;

void indicator_handler(unsigned);
void blink_handler(unsigned);

// should not get called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}

/* we only enable GPIO interrupts, so just forward it blindly  */
void interrupt_vector(unsigned pc) {
  // if (gpio_read(LEFT_INDICATOR_BUTTON_PIN) == 0 || gpio_read(RIGHT_INDICATOR_BUTTON_PIN) == 0) {
  //   indicator_handler(pc);
  // }
  //else {
    blink_handler(pc);
  //}
}

void blink_handler(unsigned pc) {
  if (!left_on) {
    if (gpio_read(LEFT_INDICATOR_LIGHT_PIN) == 0) {
      printf("1 hi\n");
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 1);
    }
    else {
      printf("2 hi\n");
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    }
  }
  else if (!right_on) {
    if (gpio_read(RIGHT_INDICATOR_LIGHT_PIN) == 0) {
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 1);
    }
    else {
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    }
  }
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
      right_on = 0;
      //armtimer_stop();
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    }
    if (left_on) {
      //turn off left armtimer
      left_on = 0;
      //armtimer_stop();
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    }
    else {
      //init left armtimer
      left_on = 1;
      //armtimer_init();
      //armtimer_start(TIMER_INTERVAL);
      //gpio_write(LEFT_INDICATOR_LIGHT_PIN, 1);
    }
  }

  if (gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN)) {
    if (left_on) {
      left_on = 0;
      //armtimer_stop();
      gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    }
    if (right_on) {
      right_on = 0;
      //armtimer_stop();
      gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    }
    else {
      right_on = 1;
      //armtimer_init();
      //armtimer_start(TIMER_INTERVAL);
      //gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 1);
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
    gpio_detect_rising_edge(LEFT_INDICATOR_BUTTON_PIN);
    gpio_set_function(RIGHT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
    gpio_set_pullup(RIGHT_INDICATOR_BUTTON_PIN);
    gpio_detect_falling_edge(RIGHT_INDICATOR_BUTTON_PIN);
    gpio_detect_rising_edge(RIGHT_INDICATOR_BUTTON_PIN);
    interrupts_enable(INTERRUPTS_GPIO3);
    system_enable_interrupts();
}


void main(void) {
  
  //TESTING ARMTIMER
  gpio_set_function(LEFT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
  gpio_set_function(RIGHT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
  //armtimer_init();
  printf_init();
  // armtimer_init(2000000); // 1s 
  // armtimer_set_prescalar(125); 
  // armtimer_enable(); 

  // armtimer_enable_interrupt(); 
  // interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ); 
  // system_enable_interrupts(); 
  //setup_interrupts();
  // armtimer_start(TIMER_INTERVAL);
  // current_time = timer_get_time();
  // while(1) {

  // }

  /***************************** MAY 25 WEDNESDAY LAB *****************************/

  //MAGNET STUFF: see halleffect.c for doc. halleffect.c is never run

  /*gpio_set_function(HALL_PIN, GPIO_FUNC_INPUT);
  gpio_set_pullup(HALL_PIN);

  for(int i = 0; i < 10; i++) {
      while(gpio_read(HALL_PIN) == 1) {}
    printf("magnet close!\n");
      while(gpio_read(HALL_PIN) == 0) {}
    printf("magnet out of range!\n");
  }*/ 

  //ACCELEROMETER STUFF

 //run_accel();
  
}
