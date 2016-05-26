#ifndef ACCEL_H
#define ACCEL_H

/*
 * Header file for accelerometer/gyrometer/compass
 *
 */

void writeReg(unsigned char reg, unsigned char v);
unsigned readReg(unsigned char reg);
void run_accel(void);

#endif