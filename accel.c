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
	while(1) { 
		unsigned char status;
		// need to check GDA/XDA
		while(((status = readReg(STATUS_REG)) & (0x2|0x1)) == 0) 
			;
	
		// p26 interprets the data.
		unsigned x,y,z;
		if(status & 0x2) {
			x =  readReg(OUTX_L_G);
			x |= readReg(OUTX_H_G) << 8;
	
			y =  readReg(OUTY_L_G);
			y |= readReg(OUTY_H_G) << 8;
	
			z =  readReg(OUTZ_L_G);
			z |= readReg(OUTZ_H_G) << 8;
	
			if(cnt%100==0)
			printf("gyro=(%dmg,%dmg,%dmg)", 
						(unsigned)(x*.061),
						(unsigned)(y*.061),
						(unsigned)(z*.061));
		}
		if((status & 0x1) == 0) {
			if(cnt%100==0) printf("\n");
		} else { 
			x =  readReg(OUTX_L_XL);
			x |= readReg(OUTX_H_XL) << 8;
	
			y =  readReg(OUTY_L_XL);
			y |= readReg(OUTY_H_XL) << 8;
	
			z =  readReg(OUTZ_L_XL);
			z |= readReg(OUTZ_H_XL) << 8;
	
			if(cnt%100==0)
			printf("\taccel=(%dmg,%dmg,%dmg)\n", 
						(unsigned)(x*.061),
						(unsigned)(y*.061),
						(unsigned)(z*.061));
		}
		cnt++;
	}
	//reboot();
}