/* Interrupt Handlers */
#include "gpio.h"
#include "gpioextra.h"
#include "gpioevent.h"
#include "armtimer.h"
#include "pin_constants.h"
#include "interrupts.h"
#include "timer.h"
#include "printf.h"

#define MIN_INTERVAL 300000
#define TIMER_INTERVAL 500000
#define ARMTIMER_INTERVAL 0x10

#define WHEEL_CIRCUMFERENCE 0.001289 // in miles
#define MICROSECONDS_IN_SECOND 1000000
#define SECONDS_IN_HOUR 3600
#define ACCELERATION_THRESHOLD 1 // (miles per hour per second)

static int left_on;
static int right_on;
static int current_time;
static int indicator_interval_time;
static double prev_speed = 0;

int last_rev_time;
extern double speed;
extern double distance;
extern int mode;
extern double acceleration;

static void mode_vector(unsigned pc);
static void left_indicator_vector(unsigned pc);
static void right_indicator_vector(unsigned pc);
static void hall_vector(unsigned pc);
static void blink_vector(unsigned pc);

void interrupt_vector(unsigned pc) {
  	mode_vector(pc);
	left_indicator_vector(pc);
	right_indicator_vector(pc);
	hall_vector(pc);
	blink_vector(pc);
}

/*** INTERRUPT HANDLING VECTORS ***/
static void mode_vector(unsigned pc) {
	if (gpio_check_and_clear_event(CHANGE_MODE_BUTTON_PIN) && (timer_get_time() - current_time) > MIN_INTERVAL) {
		mode = (mode + 1) % 3;
		current_time = timer_get_time();
	}
}

static void left_indicator_vector(unsigned pc) {
  	if (gpio_check_and_clear_event(LEFT_INDICATOR_BUTTON_PIN) && (timer_get_time() - current_time) > MIN_INTERVAL) {
    		if (right_on) {
      			right_on = 0;
      			gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    		}
		
		if (left_on) {
      			left_on = 0;
      			gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    		} else {
      			left_on = 1;
    		}
		current_time = timer_get_time();
  	}
}

static void right_indicator_vector(unsigned pc) {
	if (gpio_check_and_clear_event(RIGHT_INDICATOR_BUTTON_PIN) && (timer_get_time() - current_time) > MIN_INTERVAL) {
    		if (left_on) {
      			left_on = 0;
      			gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    		} 
		
		if (right_on) {
      			right_on = 0;
      			gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    		} else {
      			right_on = 1;
    		}
		current_time = timer_get_time();
  	}
}

static void blink_vector(unsigned pc) {
	armtimer_clear_interrupt();
  	if (timer_get_time() - indicator_interval_time < TIMER_INTERVAL) {
    		return;
  	}
  	indicator_interval_time = timer_get_time();

  	if (left_on) {
    		if (gpio_read(LEFT_INDICATOR_LIGHT_PIN) == 0) {
      			gpio_write(LEFT_INDICATOR_LIGHT_PIN, 1);
    		} else {
      			gpio_write(LEFT_INDICATOR_LIGHT_PIN, 0);
    		}
  	} else if (right_on) {
    		if (gpio_read(RIGHT_INDICATOR_LIGHT_PIN) == 0) {
      			gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 1);
    		} else {
      			gpio_write(RIGHT_INDICATOR_LIGHT_PIN, 0);
    		}
  	}
}

static void hall_vector(unsigned pc) {
	if (gpio_check_and_clear_event(HALL_PIN)) { 
		distance += WHEEL_CIRCUMFERENCE;
		double time_diff = (double) (timer_get_time() - last_rev_time);
		time_diff /= MICROSECONDS_IN_SECOND;
		time_diff /= SECONDS_IN_HOUR;
		speed = WHEEL_CIRCUMFERENCE / time_diff; 
    acceleration = (speed - prev_speed) / (time_diff * 3600);
    prev_speed = speed;
    
    if (acceleration < (-ACCELERATION_THRESHOLD)) {
      gpio_write(BRAKE_LIGHT_PIN, 1);
    } else {
      gpio_write(BRAKE_LIGHT_PIN, 0 );
    }
    
		//printf("magnet close\n");
		last_rev_time = timer_get_time();
	}
}

/*** INTERRUPT SETUP ***/
static void setup_indicator_interrupts() {
	gpio_set_function(LEFT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
 	gpio_set_pullup(LEFT_INDICATOR_BUTTON_PIN);
 	gpio_detect_falling_edge(LEFT_INDICATOR_BUTTON_PIN);

	gpio_set_function(RIGHT_INDICATOR_BUTTON_PIN, GPIO_FUNC_INPUT);
 	gpio_set_pullup(RIGHT_INDICATOR_BUTTON_PIN);
 	gpio_detect_falling_edge(RIGHT_INDICATOR_BUTTON_PIN);
}

static void setup_mode_interrupts() {
	gpio_set_function(CHANGE_MODE_BUTTON_PIN, GPIO_FUNC_INPUT);
 	gpio_set_pullup(CHANGE_MODE_BUTTON_PIN);
 	gpio_detect_falling_edge(CHANGE_MODE_BUTTON_PIN);
}

static void setup_armtimer_interrupts() {
	armtimer_init();
	armtimer_start(ARMTIMER_INTERVAL);
}

static void setup_hall_interrupts() {
	gpio_set_function(HALL_PIN, GPIO_FUNC_INPUT);
 	gpio_set_pullup(HALL_PIN);
 	gpio_detect_falling_edge(HALL_PIN);
}

void setup_interrupts() {
	setup_mode_interrupts();
	setup_indicator_interrupts();
	setup_armtimer_interrupts();
	setup_hall_interrupts();
	indicator_interval_time = timer_get_time();
	current_time = timer_get_time();
	last_rev_time = timer_get_time();
	interrupts_enable(INTERRUPTS_GPIO3);
}

// The rest of these should never be called
void fast_interrupt_vector(int pc) {}
void reset_vector(int pc) {}
void undefined_instruction_vector(int pc) {}
void software_interrupt_vector(int pc) {}
void prefetch_abort_vector(int pc) {}
void data_abort_vector(int pc) {}
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
}
