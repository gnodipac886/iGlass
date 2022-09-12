/*
iGlass_imu reads give raw (uncalibrated) 16-bit data (not floats). This is to reduce the frequency of BLE's IMU-data sends.


How to calibrate:
- Get the sensors' Full Scale (look at iGlass_imu.cpp's init() for the chosen FS settings)
float accelFS = 8.0; 				// {2.0, 24.0, 4.0, 8.0}  g
float gyroFS = 1000.0;				// {245.0, 500.0, 1000.0, 2000.0}  °/s
float magnetFS = 400.0;				// {400.0, 800.0, 1200.0, 1600.0}  µT

- Calculate the sensors' scales
float accelScale = accelFS/32768.0;
float gyroScale = gyroFS/32768.0;
float magnetScale = magnetFS/32768.0;

- Choose the desired units
float accelUnit = 1.0;            		//	GRAVITY (=1.0) METERPERSECOND2 (=9.81)  
float gyroUnit = 1.0;					//	DEGREEPERSECOND (=1.0)  RADIANSPERSECOND (=3.141592654/180)  REVSPERMINUTE (=60.0/360.0) REVSPERSECOND (=1.0/360.0) 
float magnetUnit = 1.0;			        //	GAUSS (=0.01)  MICROTESLA (=1.0)  NANOTESLA (=1000.0)

- Run Arduino Library Arduino_LSM9DS1's DIY_Calibration sketches 
We will be using the program's output code for the sensors' slopes (accelSlope[0..2], gyroSlope[0..2], magnetSlope[0..2]) and offsets (gyroOffset[0..2], magnetOffset[0..2]).
*Note: there is an exception for the accelerometer's offsets (accelOffset[0..2])
accelOffset[0] = x /(accelUnit * accelSlope[0]);  
accelOffset[1] = y /(accelUnit * accelSlope[1]);
accelOffset[2] = z /(accelUnit * accelSlope[2]);
xyz are the offset values from the program's output.
accelSlope should still be {1,1,1}, since apparently Arduino Library Arduino_LSM9DS1's sketches normally set accelOffset before accelSlope..........not sure why though, check this exception in the future

- Calibrate the raw data collected from iGlass_imu's reads
float calibratedAccelData[0] = accelUnit * accelSlope[0] * (accelScale * rawAccelData[0] - accelOffset[0]);
float calibratedAccelData[1] = accelUnit * accelSlope[1] * (accelScale * rawAccelData[1] - accelOffset[1]);
float calibratedAccelData[2] = accelUnit * accelSlope[2] * (accelScale * rawAccelData[2] - accelOffset[2]);
float calibratedGyroData[0] = gyroUnit * gyroSlope[0] * (gyroScale * rawGyroData[0] - gyroOffset[0]);
float calibratedGyroData[1] = gyroUnit * gyroSlope[1] * (gyroScale * rawGyroData[1] - gyroOffset[1]);
float calibratedGyroData[2] = gyroUnit * gyroSlope[2] * (gyroScale * rawGyroData[2] - gyroOffset[2]);
float calibratedMagData[0] = magnetUnit * magnetSlope[0] * (magnetScale * rawMagData[0] - magnetOffset[0]);
float calibratedMagData[1] = magnetUnit * magnetSlope[1] * (magnetScale * rawMagData[1] - magnetOffset[1]);
float calibratedMagData[2] = magnetUnit * magnetSlope[2] * (magnetScale * rawMagData[2] - magnetOffset[2]);


For more information/details about this linear calibration method, read Arduino Library Arduino_LSM9DS1's readme.
*/

#ifndef iGLASS_IMU_H
#define iGLASS_IMU_H

#include "arduino.h"
#include <Arduino_LSM9DS1.h>
#include <math.h>
#include "iGlass_macros.h"

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

// read definitions
#define ACC							0
#define GYRO 						1
#define MAG 						2
#define ACCGYRO						3

// other
#define NUM_AXES 					3
#define IMU_FIFO_SAMPLE_CAPACITY    32			                            // only for ACC and GYRO; note: no interrupts are signaled when samples fill up the FIFO buffers because the LSM9DS1's interrupt pins are not soldered, on the Arduino Nano 33 BLE Sense
#define IMU_SAMPLE_BYTE_SIZE        NUM_AXES * sizeof(int16_t)           	// 3*2 = 6

//----------------------------------------------------------------------------------------------------------------------
// iGlass imu class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_imu {
	public:
		int 	init(); 
		void 	end();
		int 	print();
		int 	read(int16_t * buf, int num_samples, int sensor); 
		int 	read_acc_gyro(int16_t * acc_buf, int16_t * gyro_buf, int num_samples);
		int 	write();

	private:
		int 	imu_setup_flag = 0;
		int 	read_acc(int16_t *buf, int num_samples);
		int 	read_gyro(int16_t *buf, int num_samples);
		int 	read_mag(int16_t *buf, int num_samples);

};

#endif