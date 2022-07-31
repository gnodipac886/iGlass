#include <iGlass.h>

void setup() {
  	/*Unittest init*/
	Serial.println("Before initialization...");

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	Serial.println("Initialized!");

	/*IMU unittest*/
	iGlass_imu imu_i = iGlass_imu();

	Serial.println("Before IMU init...");
	imu_i.init();
	Serial.println("IMU initialized!");


	int16_t imu_data_buf[IMU_FIFO_SIZE*3];

	Serial.println("Before IMU read...");

	Serial.println("Before reading accelerometer");
	while(!imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, ACC));
	Serial.println("Accelerometer data read!");

	Serial.println("Before reading gyroscope");	
	while(!imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, GYRO));
	Serial.println("Gyroscope data read!");

	Serial.println("Before reading magnetometer");
	while(!imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, MAG));
	Serial.println("Magnetometer data read!");

	Serial.println("After IMU read...");


	Serial.println("Before IMU print...");
	for(int i = 0; i < 200; i++) {
		imu_i.print();
	}
	Serial.println("After IMU print...");

	imu_i.end();

	Serial.println("IMU test succeeded!");
}

void loop() {
}
