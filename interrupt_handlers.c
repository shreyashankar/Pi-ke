/* Interrupt Handlers */
#include "gpio.h"
#include "pin_constants.h"
#include "interrupts.h"
#include "timer.h"

#define MIN_INTERVAL 300000
#define TIMER_INTERVAL 500000

static int left_on;
static int right_on;
static int current_time;
static int indicator_interval_time;

extern int mode;

void clear_debouncing() {
	printf("before\n");
	printf("%d\n", timer_get_time() - current_time );
	if (timer_get_time() - current_time < MIN_INTERVAL) {
		printf("here\n");
    	gpio_clear_event(LEFT_INDICATOR_BUTTON_PIN);
    	gpio_clear_event(RIGHT_INDICATOR_BUTTON_PIN);
    	gpio_check_and_clear_event(CHANGE_MODE_BUTTON_PIN);
    	current_time = timer_get_time();
  	}
}

void interrupt_vector(unsigned pc) {
	clear_debouncing();
  	mode_vector();                                                                
}

static void setup_indicator_interrupts() {
	//todo
}

static void setup_mode_interrupts() {
	gpio_set_function(CHANGE_MODE_BUTTON_PIN, GPIO_FUNC_INPUT);
 	gpio_set_pullup(CHANGE_MODE_BUTTON_PIN);
 	gpio_detect_falling_edge(CHANGE_MODE_BUTTON_PIN);
}


void setup_interrupts() {
	setup_indicator_interrupts();
	setup_mode_interrupts();
	indicator_interval_time = timer_get_time();
	current_time = timer_get_time();
	interrupts_enable(INTERRUPTS_GPIO3);
}

// The rest of these should never be called
void fast_interrupt_vector(int pc) {}
void reset_vector(int pc) {}
void undefined_instruction_vector(int pc) {}
void software_interrupt_vector(int pc) {}
void prefetch_abort_vector(int pc) {}
void data_abort_vector(int pc) {}

void mode_vector(pc) {
	if (gpio_check_and_clear_event(CHANGE_MODE_BUTTON_PIN)) {
		mode = (mode + 1) % 3;
	}
	current_time = timer_get_time();
}