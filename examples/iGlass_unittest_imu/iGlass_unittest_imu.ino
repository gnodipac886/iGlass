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
    if (imu_i.init() == EXECUTION_FAILURE) {
        while(1);
    }
	Serial.println("IMU initialized!");


	Serial.println("Before IMU read...");
    int16_t imu_data_buf[IMU_FIFO_SAMPLE_CAPACITY*NUM_AXES];

	Serial.println("Before reading accelerometer");
    int read_status = 0;
    while(read_status == 0) {
        read_status = imu_i.read(imu_data_buf, IMU_FIFO_SAMPLE_CAPACITY, ACC);
    }
    if (read_status == EXECUTION_FAILURE) {
        while(1);
    }
	Serial.println("Accelerometer data read!");

	Serial.println("Before reading gyroscope");	
    read_status = 0;
    while(read_status == 0) {
        read_status = imu_i.read(imu_data_buf, IMU_FIFO_SAMPLE_CAPACITY, GYRO);
    }
    if (read_status == EXECUTION_FAILURE) {
        while(1);
    }
	Serial.println("Gyroscope data read!");

	Serial.println("Before reading magnetometer");
    read_status = 0;
    while(read_status == 0) {
        read_status = imu_i.read(imu_data_buf, IMU_FIFO_SAMPLE_CAPACITY, MAG);
    }
    if (read_status == EXECUTION_FAILURE) {
        while(1);
    }
	Serial.println("Magnetometer data read!");

    Serial.println("Before reading ACC_GYRO");
    int16_t imu_acc_data_buf[IMU_FIFO_SAMPLE_CAPACITY*NUM_AXES];
    int16_t imu_gyro_data_buf[IMU_FIFO_SAMPLE_CAPACITY*NUM_AXES];
    read_status = 0;
    while(read_status == 0) {
        read_status = imu_i.read_acc_gyro(imu_acc_data_buf, imu_gyro_data_buf, IMU_FIFO_SAMPLE_CAPACITY);
    }
    if (read_status == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("ACC_GYRO data read!");

	Serial.println("After IMU read...");


    Serial.println("Before IMU write...");
    if (imu_i.write() == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("After IMU write...");


	Serial.println("Before IMU print...");
	for(int i = 0; i < 200; i++) {
        if (imu_i.print() == EXECUTION_FAILURE) {
            while(1);
        }
	}
	Serial.println("After IMU print...");


	imu_i.end();
	Serial.println("IMU test succeeded!");
}

void loop() {
}
