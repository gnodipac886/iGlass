#ifndef iGLASS_BLE_SEND_IMU
#define iGLASS_BLE_SEND_IMU

#include "../hardware/iGlass_ble.h"
#include "../hardware/iGlass_imu.h"
#include "iGlass_api.h"
#include <stdint.h> 	//debugggg

#define NUM_AXIS 		3
#define NUM_IMU_SENSORS	3
#define ACC_BUF_SIZE 	(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXIS) * NUM_AXIS
#define GYRO_BUF_SIZE 	(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXIS) * NUM_AXIS
#define MAG_BUF_SIZE 	5 * NUM_AXIS

class iGlass_ble_send_imu : public iGlass_api {
	public:
		iGlass_ble_send_imu();
		void init();
		void main_task();

	private:
		void update_ble_imu();
		void update_ble_imu_char(int sensor);
		void clear_buf(int sensor);

		iGlass_ble ble_i;
		iGlass_imu imu_i;

		int16_t acc_buf[ACC_BUF_SIZE];			//3-degrees of ACC; 120 = int(BLE_IMU_BUF_SIZE/3)*3
		int16_t gyro_buf[GYRO_BUF_SIZE];		//3-degrees of GYRO
		int16_t mag_buf[MAG_BUF_SIZE+6];		//3-degrees of MAG

		int16_t * imu_buf_arr[NUM_IMU_SENSORS];	// ptr to ACC buf, ptr to GYRO buf, ptr to MAG bug
		int imu_buf_idx_arr[NUM_IMU_SENSORS];	// ACC buf idx, GYRO buf idx, MAG buf idx
		int imu_buf_size_arr[NUM_IMU_SENSORS];	// ACC max buf size, GYRO max buf size, MAG max buf size
		int imu_char_idx_arr[NUM_IMU_SENSORS];	// ACC ble char idx, GYRO ble char idx, MAG ble char idx
};




#endif
