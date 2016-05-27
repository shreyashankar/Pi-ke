/*
 * hall effect sensor 3144.   
 * http://www.allegromicro.com/~/media/Files/Datasheets/A3141-2-3-4-Datasheet.ashx?la=en
 *
 *
 * looking down from the top of the pyramid = power, ground, output (vout).
 * 	- vout to pin2.
 *	- enable pullup so don't need a resistor.
 *
 * http://www.raspberrypi-spy.co.uk/2015/09/how-to-use-a-hall-effect-sensor-with-the-raspberry-pi/
 *
 * The output of these devices (pin 3) switches low when the magnetic
 * field at the Hall element exceeds the operate point threshold (BOP). At
 * this point, the output voltage is VOUT(SAT). When the magnetic field
 * is reduced to below the release point threshold (BRP), the device
 * output goes high. The difference in the magnetic operate and release
 * points is called the hysteresis (Bhys) of the device. This built-in
 * hysteresis allows clean switching of the output even in the presence
 * of external mechanical vibration and electrical noise.
 */
#include "uart.h"
#include "timer.h"
#include "printf.h"
#include "reboot.h"

#include "gpio.h"
#include "gpioextra.h"

void hall_effect(void) {

	const unsigned vout= GPIO_PIN2;

  	gpio_set_function(vout, GPIO_FUNC_INPUT);
  	gpio_set_pullup(vout);

	for(int i = 0; i < 10; i++) {
  		while(gpio_read(vout) == 1) {}
		printf("magnet close!\n");
  		while(gpio_read(vout) == 0) {}
		printf("magnet out of range!\n");
	}

  	//reboot();
}