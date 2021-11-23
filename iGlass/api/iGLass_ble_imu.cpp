#include "iGlass_ble_imu.h"

void iGlass_ble_send_imu::init()
{
	imu_i = iGlass_imu();
	imu_i.init();

	ble_i = iGlass_ble();
	acc_char_idx = ble_i.addNewCharacteristic(ACC_BUF_SIZE * sizeof(int16_t));
	gyro_char_idx = ble_i.addNewCharacteristic(GYRO_BUF_SIZE * sizeof(int16_t));
	mag_char_idx = ble_i.addNewCharacteristic(MAG_BUF_SIZE * sizeof(int16_t));
	ble_i.init();
}

void iGlass_ble_send_imu::main_task() {
	update_ble_imu();
}

/*
	Function:	Update IMU (ACC, GYRO, MAG) values via BLE
	Input:		None  
	Ret Val: 	None
*/
void iGlass_ble_send_imu::update_ble_imu(){
	if (ble_i.available()) {	//continue to update buffers
		update_ble_imu_char(ACC);
		update_ble_imu_char(GYRO);
		update_ble_imu_char(MAG);
	} else {
		clear_buf(ACC);
		clear_buf(GYRO);
		clear_buf(MAG);
	}
}

void iGlass_ble_send_imu::update_ble_imu_char(int sensor) {
	if (imu_i.read(&imu_buf_arr[sensor][*imu_buf_idx_arr[sensor]],3,sensor))	
		*imu_buf_idx_arr[sensor] += 3;
		
	if (*imu_buf_idx_arr[sensor] == imu_buf_size_arr[sensor]) {
		ble_i.write(*imu_char_idx_arr[sensor], (int8_t *)imu_buf_arr[sensor], sizeof(imu_buf_arr[sensor]));
		clear_buf(sensor);
	}
}

void iGlass_ble_send_imu::clear_buf(int sensor) {
	*imu_buf_idx_arr[sensor] = 0;
	memset(imu_buf_arr[sensor], 0, sizeof(imu_buf_arr[sensor]));
}