/*
	https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial
  We are using v1 I2c.   
	SDA = gpio pin2 
	SCL = gpio pin3
  https://www.pololu.com/product/2468
  pin holes on chip (oriented with them on the left)
	scl
	sda
	gnd
	vin
	vdd
  slave address;
  	LSM6DS33 (gyro and accelerometer)	1101011b
	LIS3MDL (magnetometer)			0011110b
   basic idea:
	- everything is one byte.
	- device has a large number of "registers" that you have to write
	  values to or read from.
     communicate using I2C
	- have to use the right r/pi pins.
	- have to get the right device address (from data sheet)
	- to write reg=val,  use i2c to write two bytes [reg#,v] to the
	  device address
	- to read reg, use i2c to 
			1) *write* the reg number [reg#]
			2) do a 1-byte read to get the value.
    void i2c_init(void);
    void i2c_read(unsigned slave_address, char *data, int data_length);
    void i2c_write(unsigned slave_address, char *data, int data_length);
 */
#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "printf.h"
#include "reboot.h"

#include "gpio.h"
#include "gpioextra.h"
#include "LSM6DS33.h"

const unsigned vdd = GPIO_PIN6;
const unsigned lsm6ds33_address = 0b1101011; // this is the gyro/accel;
static double absoluteTurn = 0;

#define STABLE_Z (-193.8)	//these three values determined experimentaly
#define DEGREE_FACTOR (15538)
#define TIME_FACTOR (7374)
#define SCALING_FACTOR (114577212)


static double turn = 0;

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

void run_accel(void) {
	uart_init();
	timer_init();
	delay_ms(30);   // allow time for device to boot up.
	i2c_init();
	delay_ms(30);   // allow time to settle after init.

#if 1
	// https://www.pololu.com/file/0J1088/LSM6DS33-AN4682.pdf
	// this is from p23 
	// for gyroscope
	writeReg(CTRL10_C, 0x38);  // GYRO: enable x,y,z
	writeReg(CTRL2_G, 0x60);   // 416Hz (high perf)
	// writeReg(INT1_CTRL, 0x02);

	writeReg(CTRL9_XL, 0x38);  // ACCEL: x,y,z enabled
	writeReg(CTRL1_XL, 0x60);  // 
	//write(INT1_CTRL, 0x01);
#else
	// https://github.com/pololu/lsm6-arduino/blob/master/LSM6.cpp
    	// 0x80 = 0b10000000
    	// ODR = 1000 (1.66 kHz (high performance)); FS_XL = 00 (+/-2 g full scale)
    	writeReg(CTRL1_XL, 0x80);
    	// Gyro
    	// 0x80 = 0b010000000
    	// ODR = 1000 (1.66 kHz (high performance)); FS_XL = 00 (245 dps)
    	writeReg(CTRL2_G, 0x80);
#endif

	delay_ms(20);
	// from p8: should be 0x69
	printf("whoami=%x\n", readReg(WHO_AM_I));

	int cnt = 0;
	unsigned int curr_time;
	unsigned int begin = timer_get_time();
	double sum = 0;
	int count = 0;
	double timeDiff;
	unsigned int prevTime = begin;

	while(1) { 
		unsigned char status;
		// need to check GDA/XDA
		while(((status = readReg(STATUS_REG)) & (0x2|0x1)) == 0) 
			;
	
		// p26 interprets the data.
		short x,y,z;
		if(status & 0x2) {
			unsigned char x1, x2, y1, y2, z1, z2;
			x1 = readReg(OUTX_L_G);
			x2 = readReg(OUTX_H_G);

			y1 = readReg(OUTY_L_G);
			y2 = readReg(OUTY_H_G);

			z1 = readReg(OUTZ_L_G);
			z2 = readReg(OUTZ_H_G);

			x = (x2 << 8) | x1;
			y = (y2 << 8) | y1;
			z = (z2 << 8) | z1;

			//double zScaled = z * .061 - avgZ;
			//absoluteTurn += zScaled;
			unsigned int currentTime = timer_get_time();
			timeDiff = ((double)(currentTime - prevTime)) / TIME_FACTOR;
			//absoluteTurn += ((double)z - STABLE_Z) * timeDiff;
			absoluteTurn += (((double)z - STABLE_Z) * (currentTime - prevTime)) / SCALING_FACTOR;
			//printf("%d\n", currentTime - prevTime);
			prevTime = currentTime;
			//absoluteTurn += ((double)z - STABLE_Z) / DEGREE_FACTOR;
			//absoluteTurn += ((double)z - STABLE_Z);
			count ++;




			delay_ms(10);


			/*
			x =  readReg(OUTX_L_G);
			x |= readReg(OUTX_H_G) << 8;
	
			y =  readReg(OUTY_L_G);
			y |= readReg(OUTY_H_G) << 8;
	
			z =  readReg(OUTZ_L_G);
			z |= readReg(OUTZ_H_G) << 8;*/


			//sum += z;
			//count++;
	
			if(cnt%100==0) {
				//curr_time = timer_get_time() - begin;
				//printf("%d\n", (int) (sum / count * 10000));
				// printf("time: %d, gyro=(%dmg,%dmg,%dmg)\n", curr_time,
				// 		(int)(x*.061),
				// 		(int)(y*.061),
				// 		(int)(z*.061));
				//printf("Turn: %d\n", (int) absoluteTurn / DEGREE_FACTOR);
				printf("Turn: %d\n", (int) absoluteTurn);
				//printf("time diff: %d\n", (int) (timeDiff*10000));// * 10000000);
				//printf("Avg time: %d\n", (int) (timeDiff / count * 1000));
				//printf("Turn: %d\n", (int) absoluteTurn / 1000);
				/*printf("time: %d, gyro=(%dmg,%dmg,%dmg)\n", curr_time,
						x,y,z);*/
			}
		}
		//if((status & 0x1) == 0) {
			//if(cnt%100==0) printf("\n");
		//} else { 
			unsigned char x1, x2, y1, y2, z1, z2;
			x1 = readReg(OUTX_L_XL);
			x2 = readReg(OUTX_H_XL);

			y1 = readReg(OUTY_L_XL);
			y2 = readReg(OUTY_H_XL);

			z1 = readReg(OUTZ_L_XL);
			z2 = readReg(OUTZ_H_XL);

			x = (x2 << 8) | x1;
			y = (y2 << 8) | y1;
			z = (z2 << 8) | z1;
	
			// if(cnt%50==0) {
			// 	curr_time = timer_get_time() - begin;
			// 	printf("time: %d, accel=(%dmg,%dmg,%dmg)\n", curr_time,
			// 				(int)(x*.061),
			// 				(int)(y*.061),
			// 				(int)(z*.061));
			// }
		//}
		cnt++;
	}
	//reboot();
}