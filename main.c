// #include "gpio.h"
// #include "gpioextra.h"
// #include "gpioevent.h"
// #include "circular.h"
// #include "printf.h"
// #include "console.h"
// #include "interrupts.h"
// #include "pin_constants.h"
// #include "timer.h"
// #include "armtimer.h"
// #include "printf.h"
// #include "i2c.h"
// #include "accel.h"
// #include "halleffect.h"
// #include "LSM6DS33.h"

// #define MIN_INTERVAL 100000
// #define TIMER_INTERVAL 500000

// static int left_on;
// static int right_on;
// static int current_time;

// void indicator_handler(unsigned);
// void blink_handler(unsigned);

// // should not get called.
// void impossible_vector(unsigned pc) {
//   printf("impossible exception at pc=%x\n", pc);
// }

// /* we only enable GPIO interrupts, so just forward it blindly  */
// void interrupt_vector(unsigned pc) {
//   //if (gpio_check_event(LEFT_INDICATOR_BUTTON_PIN) || gpio_check_event(RIGHT_INDICATOR_BUTTON_PIN)) {
//   if (gpio_read(LEFT_INDICATOR_BUTTON_PIN) == 0 || gpio_read(RIGHT_INDICATOR_BUTTON_PIN) == 0) {
//     indicator_handler(pc);
//   }
//   else {
//     blink_handler(pc);
//   }
// }

// void blink_handler(unsigned pc) {
//   armtimer_clear_interrupt();
//   // if (timer_get_time() - indicator_interval_time < TIMER_INTERVAL) {
//   //   return;
//   // }
//   // indicator_interval_time = timer_get_time();
//   if (left_on) {
//     if (gpio_read(LEFT_INDICATOR_LIGHT_PIN) == 0) {
//       gpio_write(LEFT_INDICATOR_LIGHT_PIN, 1);
//     }
//     else {
//       gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
//     }
//   }
//   if (right_on) {
//     if (gpio_read(RIGHT_INDICATOR_LIGHT_PIN) == 0) {
//       gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 1);
//     }
//     else {
//       gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
//     }
//   }
// }

// void indicator_handler(unsigned pc) {
//   if (timer_get_time() - current_time < MIN_INTERVAL) {
//     gpio_check_and_clear_event(LEFT_INDICATOR_BUTTON_PIN);
//     gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN);
//     current_time = timer_get_time();
//     return;
//   }

//   if (gpio_check_and_clear_event(LEFT_INDICATOR_BUTTON_PIN)) {
//     if (right_on) {
//       right_on = 0;
//       gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
//     }
//     if (left_on) {
//       left_on = 0;
//       gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
//     }
//     else {
//       left_on = 1;
//     }
//   }

//   if (gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN)) {
//     if (left_on) {
//       left_on = 0;
//       gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
//     }
//     if (right_on) {
//       right_on = 0;
//       gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
//     }
//     else {
//       right_on = 1;
//     }
//   }
//   current_time = timer_get_time();
 
// }

// void button_test() {
//   gpio_set_function(GPIO_PIN21, GPIO_FUNC_INPUT);
//   gpio_set_pullup(GPIO_PIN21);
//   while (gpio_read(GPIO_PIN21) == 1);
//   printf("+");
//   while (gpio_read(GPIO_PIN21) == 0);
// }


// static void setup_indicator_interrupts() {
//   gpio_set_function(LEFT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
//   gpio_set_function(RIGHT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
//   gpio_set_function(LEFT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
//   gpio_set_pullup(LEFT_INDICATOR_BUTTON_PIN);
//   gpio_detect_falling_edge(LEFT_INDICATOR_BUTTON_PIN);
//   gpio_detect_rising_edge(LEFT_INDICATOR_BUTTON_PIN);
//   gpio_set_function(RIGHT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
//   gpio_set_pullup(RIGHT_INDICATOR_BUTTON_PIN);
//   gpio_detect_falling_edge(RIGHT_INDICATOR_BUTTON_PIN);
//   gpio_detect_rising_edge(RIGHT_INDICATOR_BUTTON_PIN);
//   interrupts_enable(INTERRUPTS_GPIO3);
// }

// static void setup_armtimer_interrupts() {
//   armtimer_init(TIMER_INTERVAL); // 1s 
//   armtimer_enable(); 

//   armtimer_enable_interrupt(); 
//   interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ);
// }


// void main(void) {
  
//   //TESTING LEFT/RIGHT INDICATORS
//   printf_init();
//   setup_indicator_interrupts();
//   setup_armtimer_interrupts();
//   system_enable_interrupts();
//   current_time = timer_get_time();
//   //indicator_interval_time = timer_get_time();

//   while(1) {

//   }

//   /***************************** MAY 25 WEDNESDAY LAB *****************************/

//   // //MAGNET STUFF: see halleffect.c for doc

//   //hall_effect();

//   //ACCELEROMETER STUFF

//  //run_accel();
  
// }


#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "circular.h"
#include "printf.h"
#include "console.h"
#include "interrupts.h"
#include "pin_constants.h"
#include "timer.h"
//#include "armtimer.h"
#include "printf.h"
#include "i2c.h"
#include "accel.h"
#include "halleffect.h"
#include "LSM6DS33.h"
#include "display.h"
#include "interrupt_handlers.h"

double speed = 0;    //mph
double distance = 0; //miles
int mode = 0;

static void setup_lights() {
  gpio_set_function(LEFT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
  gpio_set_function(RIGHT_INDICATOR_LIGHT_PIN, GPIO_FUNC_OUTPUT);
}

/*static void setup_armtimer_interrupts() {
  armtimer_init(1000000); // 1s 
  armtimer_enable(); 

  armtimer_enable_interrupt(); 
  interrupts_enable_basic(INTERRUPTS_BASIC_ARM_TIMER_IRQ);
}*/

void main(void) {
  
  printf_init();
  setup_lights();
  setup_interrupts();
  system_enable_interrupts();

  // while(1) {

  // }

  /***************************** MAY 25 WEDNESDAY LAB *****************************/

  // //MAGNET STUFF: see halleffect.c for doc

  //hall_effect();
  //display_init();
  //display_run();
  //ACCELEROMETER STUFF

 run_accel();
  
}
