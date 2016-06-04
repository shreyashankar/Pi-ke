#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "printf.h"
#include "reboot.h"
#include "gyro.h"

#include "gpio.h"
#include "gpioextra.h"
#include "LSM6DS33.h"

const unsigned vdd = GPIO_PIN6;
const unsigned lsm6ds33_address = 0b1101011; // this is the gyro/accel;
double absolute_turn = 0;

#define STABLE_Z (-193.8)	//these three values determined experimentaly
#define SCALING_FACTOR (114577212)

void writeReg(unsigned char reg, unsigned char v) {
	char data[2];
	data[0] = reg;
	data[1] = v;
	i2c_write(lsm6ds33_address, data, 2);
	printf("writeReg: %x=%x\n", reg, v);
}

unsigned readReg(unsigned char reg) {
	i2c_write(lsm6ds33_address, (void*) &reg, 1);
	unsigned char uc = 0;
	i2c_read(lsm6ds33_address, (void*) &uc, 1);
	return uc;
}

void gyro_init() {
	uart_init();
	timer_init();
	delay_ms(30);   // allow time for device to boot up.
	i2c_init();
	delay_ms(30);   // allow time to settle after init.

	writeReg(CTRL10_C, 0x38);  // GYRO: enable x,y,z
	writeReg(CTRL2_G, 0x60);   // 416Hz (high perf)

	writeReg(CTRL9_XL, 0x38);  // ACCEL: x,y,z enabled
	writeReg(CTRL1_XL, 0x60);  // 

	delay_ms(20);
}


void gyro_delay(unsigned delay_micro) {
	unsigned start_time = timer_get_time();
	unsigned prev_time = start_time;
	while (timer_get_time() - start_time < delay_micro) {
		unsigned char status;
		// need to check GDA/XDA
		while(((status = readReg(STATUS_REG)) & (0x2|0x1)) == 0) ;
		if(status & 0x2) {
			short z;
			unsigned char  z1, z2;

			z1 = readReg(OUTZ_L_G);
			z2 = readReg(OUTZ_H_G);

			z = (z2 << 8) | z1;

			unsigned int current_time = timer_get_time();
			absolute_turn += (((double)z - STABLE_Z) * (current_time - prev_time)) / SCALING_FACTOR;
			prev_time = current_time;
		}

	}
}

