#include "iGlass_imu.h"
#include "iGlass.h"

/*
	Function: 	Begin and setup current instance of IMU
	Input: 		None
	Ret Val: 	None
*/
void iGlass_imu::init() {
	if (imu_setup_flag == 1) 
		return;

	#if DEBUG
		if(!Serial){
			Serial.begin(115200);
			while (!Serial);
		}
	#endif

	if(!imu_setup_flag) {
		if (!IMU.begin()) {
			#if DEBUG
				Serial.println("Failed to initialize IMU!");
			#endif
			while (1);
		}
	}

	IMU.setContinuousMode();
	IMU.setAccelFS(3);			 // Full Scale's multiplication factor is +-8g (does not change output of read functions, just assigns more/less bits to sensor measurement)
	IMU.setAccelODR(3);			 // Sampling Rate is 476 Hz
	IMU.setAccelOffset(0, 0, 0); //   uncalibrated <---------why? because diff for walk/talk?...................
	IMU.setAccelSlope(1, 1, 1);	 //   uncalibrated <--------- can use instead of weights?..........

	IMU.setGyroFS(2);			 // Full Scale's multiplication factor is +-8g (does not change output of read functions, just assigns more/less bits to sensor measurement)
	IMU.setGyroODR(3);
	IMU.setGyroOffset(-2.10,-0.11, -0.005); //   uncalibrated <---------why? because diff for walk/talk?...................
	IMU.setGyroSlope(1.1675, 1.165, 1.160);	 //   uncalibrated <--------- can use instead of weights?..........

	IMU.setMagnetODR(8);
	imu_setup_flag = 1;
}

/*
	Function:	Reads sensor data to buffer
	Input:		buf - ptr to data buffer; sensor - ACC/GYRO/MAG  
	Ret Val: 	num of sensor data pts read
*/
int iGlass_imu::read(int16_t * buf, int num_samples, int sensor) {
	int samples_read;
	switch(sensor) {
		case ACC:
			samples_read = read_acc(buf, num_samples);
			break;
		case GYRO:
			samples_read = read_gyro(buf, num_samples);
			break;
		case MAG:
			samples_read = read_mag(buf, num_samples);
			break;

		default:
			samples_read = 0;
			break;
	}

	return samples_read;
}

/*
	Function:	Reads accelerometer and gyroscope data to buffer; helper function to read
	Input:		buf - ptr to data buffer
				num_samples - number of 16-bit samples to read
	Ret Val: 	num of acceleromter 16-bit data read
*/
int iGlass_imu::read_acc_gyro(int16_t * acc_buf, int16_t * gyro_buf, int num_samples) {
	int pts_to_read = min(int(num_samples / 3), IMU_FIFO_SIZE);
	int pts_read = 0;
	if (IMU.accelerationAvailable() >= pts_to_read) {
		pts_read = IMU.readMultiRawGyroAccelInt16(acc_buf, gyro_buf, pts_to_read);
		return pts_read*3;
	}
	return 0;
}

/*
	Function:	Reads accelerometer data to buffer; helper function to read
	Input:		buf - ptr to data buffer
				num_samples - number of 16-bit samples to read
	Ret Val: 	num of acceleromter 16-bit data read
*/
int iGlass_imu::read_acc(int16_t * buf, int num_samples) {
	int pts_to_read = min(int(num_samples / 3), IMU_FIFO_SIZE);
	int pts_read = 0;
	if (IMU.accelerationAvailable() >= pts_to_read) {
		for (int i = 0; i < pts_to_read; i++) {
			pts_read += IMU.readRawAccelInt16(buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
		}
		return pts_read*3;
	}
	return 0;
}

/*
	Function:	Reads gyroscope data to buffer; helper function to read
	Input:		buf - ptr to data buffer
				num_samples - number of 16-bit samples to read
	Ret Val: 	num of gyroscope 16-bit data read
*/
int iGlass_imu::read_gyro(int16_t * buf, int num_samples) {
	int pts_to_read = min(int(num_samples / 3), IMU_FIFO_SIZE);
	int pts_read = 0;
	if (IMU.gyroscopeAvailable() >= pts_to_read) {
		for (int i = 0; i < pts_to_read; i++) {
			pts_read += IMU.readRawGyroInt16(buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
			//buf[i] = sqrt(square(ax_WEIGHT * ax) + square(ay_WEIGHT * ay) + square(az_WEIGHT * az))
		}
		return pts_read*3;
	}
	return 0;
}

/*
	Function:	Reads magnetometer data to buffer; helper function to read
	Input:		buf - ptr to data buffer 
				num_samples - number of samples to read, here we only ever read 1 sample
	Ret Val: 	num of magnetometer data pts read
*/
int iGlass_imu::read_mag(int16_t * buf, int num_samples) {
	if (IMU.magnetAvailable()) {
		// Serial.println("magnetAvailable");
		int i = IMU.readRawMagnetInt16(buf[0], buf[1], buf[2]);	//.....................
		// Serial.println("readRawMagnetint16 return val: " + String(i));	//..........................
		return 3;
	}
	return 0;
}

/*
	Function:	Ends the current instance of IMU
	Input:		None
	Ret Val: 	None
*/
void iGlass_imu::end() {
	if (imu_setup_flag == 0)
		return;

	IMU.end();

	imu_setup_flag = 0;
}

/*
	Function: Reads first acceleration, gyroscope, and magnetometer points on their FIFO buffers
	Input: None
	Ret Val: None
*/
void iGlass_imu::print() {
	#if DEBUG
		int16_t ax, ay, az, wx, wy, wz, mx, my, mz;
		if (IMU.accelerationAvailable()) {
			IMU.readRawAccelInt16(ax, ay, az);
			Serial.println("acc:\t" + String(ax) + "\t" + String(ay) + "\t" + String(az) + "\t\t");
		}
		if (IMU.gyroscopeAvailable()) {
			IMU.readRawGyroInt16(wx, wy, wz);
			Serial.println("ang:\t" + String(wx) + "\t" + String(wy) + "\t" + String(wz) + "\t\t");
		}
		if (IMU.magnetAvailable()) {
			IMU.readRawMagnetInt16(mx, my, mz);
			Serial.println("mag:\t" + String(mx) + "\t" + String(my) + "\t" + String(mz) + "\t\t");
		}	
	#endif
}
