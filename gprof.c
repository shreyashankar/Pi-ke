#include "gprof.h"
#include "armtimer.h"
#include "malloc.h"
#include "printf.h"

/* Assignment 7: EXTENSION
* Shreya Shankar
* This file contains the gprof functions so we can obtain the counts of the program counters.
* COMMANDS:
* profileon
* profileoff
* profileresults
* profilestatus
*/

volatile static unsigned int* data;
extern unsigned int __text_end__;
volatile static int interrupts;

/* Sets up timer interrupts and initialize address counts */
void gprof_init(void) { 
	unsigned int size = (unsigned int)((unsigned int)(&__text_end__) - 0x8000);
	data = malloc(sizeof(unsigned int) * size);
	for (int i = 0; i < size; i++) {
    	*(data + i) = 0; 
    }
    interrupts = 1;
	armtimer_init();
	armtimer_start(GPROF_TIMER_INTERVAL);
}

void gprof_off() {
	armtimer_stop();
	interrupts = 0;
}

//Called by shell to see if interrupts are on or not
int interruptsOn() {
	if (interrupts) {
		return 1;
	}
	return 0;
}

//prints out the pc counts for each pc. cycles through all addresses in text segment and print out their counts
void gprof_dump(void) {
    console_printf("******************\n");
    console_printf(" PC    |     COUNT\n");
   
	unsigned int size = (unsigned int)((unsigned int)(&__text_end__) - 0x8000);
    for (int i = 0; i < size; i++) {
    	int address = 0x8000 + i;
    	if (*(data + i) != 0) {
    		console_printf("0x%x: %d counts\n", address, *(data + i));
    	}
    }
}

//Increments the count for the given address
void gprof_vector(unsigned pc) {
	armtimer_clear_interrupt();
	unsigned int index = (unsigned int)(pc - 0x8000);
	//int index = pc - 0x8000;
	*(data + index) = *(data + index) + 1;
}
