#include "gpio.h"
#include "timer.h"
#include "display.h"
#include "pin_constants.h"
#include "printf.h"
#include "assert.h"

extern double speed;
extern double distance;
extern int mode;

#define DELAY 2500
#define MILLION 1000000

/* Initializes the GPIO pins for the display */
void display_init() {
	for (int i = DIGIT_GPIO_START; i <= DIGIT_GPIO_END; i++) {
		gpio_set_output(i);//, GPIO_FUNC_OUTPUT);
	}

	for (int i = SEGMENT_GPIO_START; i <= SEGMENT_GPIO_END; i++) {
		gpio_set_output(i);//, GPIO_FUNC_OUTPUT);
	}

	//gpio_set_input(2);//, GPIO_FUNC_INPUT);
	//gpio_set_input(3);//, GPIO_FUNC_INPUT);

}

/*
 * Turns on a digit on the 7-segment display
 * 
 * @param digit - Which digit to turn on (0 - 15)
 * @param location - Where to turn on the digit (0 - 3)
 */
void display_digit(int digit, int location) {
	static char digits_binary[16] = 
		{0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, //0-4
		0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111,  //5-9
		0b01110111, 0b01111111, 0b00111001, 0b00111111, 0b01111001,  //A-E
		0b01110001}; 	
	gpio_write(location + 10, 1); 	//turn on the correct digit
	for (int i = 0; i <= SEGMENT_GPIO_END - SEGMENT_GPIO_START; i++){
		gpio_write(i + SEGMENT_GPIO_START, ((1 << i) & digits_binary[digit]) >> i); //extract and write to appripate digit

	}
}

/*
 * Clears the display for all 4 digits
 */
void clearDigits() {
	//clear GPIO 10-13
	for (int i = DIGIT_GPIO_START; i <= DIGIT_GPIO_END; i++) {
		gpio_write(i, 0);
	}
	//clear GPIO 20-27
	for (int i = SEGMENT_GPIO_START; i <= SEGMENT_GPIO_END; i++) {
		gpio_write(i, 0);
	}
}

/*
 * Updates the display with the requested number of seconds
 *
 * @param seconds - Number of seconds to display
 */
void display_seconds(int seconds) {
	unsigned int digits[4];
	digits[3] = seconds % 10; 		//seconds ones
	digits[2] = (seconds / 10) % 6; //seconds tens
	digits[1] = (seconds/ 60) % 10; //minutes ones
	digits[0] = (seconds/ 600) % 6; //minutes tens
	for (int digitLoc = 0; digitLoc < 4; digitLoc ++) {
		display_digit(digits[digitLoc], digitLoc);
		if (digitLoc == 1) {
			gpio_write(GPIO_PIN27, 1); //write period
		}
		delay_us(DELAY);
		clearDigits();
	}
}

void display_num(double number, int numDec) {
	for (int i = 0; i < numDec; i++) {
		number *= 10;
	}
	unsigned rounded_number = (unsigned) number;
	printf("%d\n", rounded_number);
	for (int digitLoc = 3; digitLoc >= 0; digitLoc--) {
		display_digit(rounded_number % 10, digitLoc);
		if (digitLoc == (3 - numDec)) {
			gpio_write(GPIO_PIN27, 1); //write the period
		}

		rounded_number /= 10;
		delay_us(DELAY);
		clearDigits();
	}
}

/* Dispalys the current speed variable with 1 decimal */
void display_speed() {

	display_num(speed,1);

}

void display_distance() {
	display_num(distance,2);
}


/* Runs the clock */
void display_run() {
	unsigned int start_time = timer_get_time(); //when the clock starts
	unsigned int offset = 0;  	//used when setting the time


	while (1) {

		//get the difference since starting in seconds
		switch(mode) {
			
			case 0: ;
				unsigned int time_diff = (timer_get_time() - start_time) / MILLION + offset;
				display_seconds(time_diff); //display that time
				break;
			case 1:
				display_distance();
				break;
			case 2:
				display_speed();
				break;
			default:
				assert(0);
		}


		//check if both buttons are pressed; if so, switch modes
		/*if ((gpio_read(2) == PRESSED) && (gpio_read(3) == PRESSED)) { //both pressed
			offset = set_time(time_diff); 	//gives number of seconds set to
			start_time = timer_get_time(); 	//reset start time
		}*/
		
	}
}