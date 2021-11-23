#ifndef iGLASS_BLE_SEND_IMU
#define iGLASS_BLE_SEND_IMU

#include "../hardware/iGlass_ble.h"
#include "../hardware/iGlass_imu.h"
#include "iGlass_api.h"

#define NUM_AXIS 		3
#define NUM_IMU_SENSORS	3
#define ACC_BUF_SIZE 	(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXIS) * NUM_AXIS
#define GYRO_BUF_SIZE 	(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXIS) * NUM_AXIS
#define MAG_BUF_SIZE 	5 * NUM_AXIS

class iGlass_ble_send_imu : public iGlass_api {
	public:
		// ~iGlass_ble_send_imu();
		void init();
		void main_task();

	private:
		void update_ble_imu();
		void update_ble_imu_char(int sensor);
		void clear_buf(int sensor);

		iGlass_ble ble_i;
		iGlass_imu imu_i;
		int acc_char_idx = 0;
		int gyro_char_idx = 0;
		int mag_char_idx = 0;
		int acc_buf_idx = 0;
		int gyro_buf_idx = 0;
		int mag_buf_idx = 0;
		int16_t acc_buf[ACC_BUF_SIZE];			//3-degrees of ACC; 120 = int(BLE_IMU_BUF_SIZE/3)*3
		int16_t gyro_buf[GYRO_BUF_SIZE];		//3-degrees of GYRO
		int16_t mag_buf[MAG_BUF_SIZE];			//3-degrees of MAG
		int16_t * imu_buf_arr[NUM_IMU_SENSORS] = {acc_buf, gyro_buf, mag_buf};
		int * imu_buf_idx_arr[NUM_IMU_SENSORS] = {&acc_buf_idx, &gyro_buf_idx, &mag_buf_idx};
		int imu_buf_size_arr[NUM_IMU_SENSORS] = {ACC_BUF_SIZE, GYRO_BUF_SIZE, MAG_BUF_SIZE};
		int * imu_char_idx_arr[NUM_IMU_SENSORS] = {&acc_char_idx, &gyro_char_idx, &mag_char_idx};
};

#endif