#include "arduino.h"
#include <Arduino_LSM9DS1.h>
#include <math.h>

#ifndef iGLASS_IMU_H
#define iGLASS_IMU_H

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

#define LSM9DS1_ADDRESS				0x6b

#define LSM9DS1_WHO_AM_I			0x0f
#define LSM9DS1_CTRL_REG1_G			0x10
#define LSM9DS1_STATUS_REG			0x17
#define LSM9DS1_OUT_X_G				0x18
#define LSM9DS1_CTRL_REG6_XL 		0x20
#define LSM9DS1_CTRL_REG8			0x22
#define LSM9DS1_OUT_X_XL		 	0x28

#define IMU_FIFO_SIZE				32

// magnetometer
#define LSM9DS1_ADDRESS_M			0x1e

#define LSM9DS1_CTRL_REG1_M 		0x20
#define LSM9DS1_CTRL_REG2_M 		0x21
#define LSM9DS1_CTRL_REG3_M 		0x22
#define LSM9DS1_CTRL_REG4_M 		0x23
#define LSM9DS1_STATUS_REG_M		0x27
#define LSM9DS1_OUT_X_L_M   		0x28

// accelerometer
#define ax_WEIGHT					0.5
#define ay_WEIGHT					1.0
#define az_WEIGHT					0.5

// read definitions
#define ACC							1
#define GYRO 						2
#define MAG 						3

//----------------------------------------------------------------------------------------------------------------------
// iGlass imu class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_imu {
	public:
		void 	init();
		void 	end();
		void 	print();
		int 	read(int16_t *buf, int sensor);

	private:
		int 	imu_setup_flag;
		int 	read_acc(int16_t *buf);
		int 	read_gyro(int16_t *buf);
		int 	read_mag(int16_t *buf);

};

#endif