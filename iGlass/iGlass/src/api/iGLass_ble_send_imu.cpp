#include "iGlass_ble_send_imu.h"

iGlass_ble_send_imu::iGlass_ble_send_imu() : iGlass_api(), 
	// imu_buf_arr{acc_buf, gyro_buf, mag_buf}, 
	imu_buf_idx_arr{0, 0, 0}
	// imu_buf_size_arr{ACC_BUF_SIZE, GYRO_BUF_SIZE, MAG_BUF_SIZE}
	 {

	acc_buf = (int16_t*)malloc(ACC_BUF_SIZE * sizeof(int16_t));
	gyro_buf = (int16_t*)malloc(GYRO_BUF_SIZE * sizeof(int16_t));
	mag_buf = (int16_t*)malloc((MAG_BUF_SIZE + 6) * sizeof(int16_t));
	imu_buf_arr[0] = acc_buf;
	imu_buf_arr[1] = gyro_buf;
	imu_buf_arr[2] = mag_buf;
	clear_buf(ACC);
	clear_buf(GYRO);
	clear_buf(MAG);
}

void iGlass_ble_send_imu::init()
{
	imu_i = iGlass_imu();
	imu_i.init();

	ble_i = iGlass_ble();
	imu_char_idx_arr[ACC] = ble_i.addNewCharacteristic(ACC_BUF_SIZE * sizeof(int16_t));
	imu_char_idx_arr[GYRO] = ble_i.addNewCharacteristic(GYRO_BUF_SIZE * sizeof(int16_t));
	imu_char_idx_arr[MAG] = ble_i.addNewCharacteristic(MAG_BUF_SIZE * sizeof(int16_t));
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
		update_ble_imu_char_accgyro(ACCGYRO);
		update_ble_imu_char(MAG);
	} else {
		clear_buf(ACC);
		clear_buf(GYRO);
		clear_buf(MAG);
	}
}

void iGlass_ble_send_imu::update_ble_imu_char_accgyro(int sensor) {
	if (imu_buf_idx_arr[ACC] < imu_buf_size_arr[ACC]){
		int read_size = min(NUM_AXIS * IMU_FIFO_SIZE, imu_buf_size_arr[ACC] - imu_buf_idx_arr[ACC]);
		int i = imu_i.read_acc_gyro(&imu_buf_arr[ACC][imu_buf_idx_arr[ACC]], &imu_buf_arr[GYRO][imu_buf_idx_arr[GYRO]], read_size);
		imu_buf_idx_arr[ACC] += i;
		imu_buf_idx_arr[GYRO] += i;
	}
	if (imu_buf_idx_arr[ACC] == imu_buf_size_arr[ACC]) {
		ble_i.write(imu_char_idx_arr[ACC], (int8_t *)imu_buf_arr[ACC], imu_buf_size_arr[ACC]);
		clear_buf(ACC);
	}
	if (imu_buf_idx_arr[GYRO] == imu_buf_size_arr[GYRO]) {
		ble_i.write(imu_char_idx_arr[GYRO], (int8_t *)imu_buf_arr[GYRO], imu_buf_size_arr[GYRO]);
		clear_buf(GYRO);
	}
}


void iGlass_ble_send_imu::update_ble_imu_char(int sensor) {
	if (imu_buf_idx_arr[sensor] < imu_buf_size_arr[sensor]){
		int read_size = min(NUM_AXIS * IMU_FIFO_SIZE, imu_buf_size_arr[sensor] - imu_buf_idx_arr[sensor]);
		int i = imu_i.read(&imu_buf_arr[sensor][imu_buf_idx_arr[sensor]], read_size, sensor);
		imu_buf_idx_arr[sensor] += i;
	}
	if (imu_buf_idx_arr[sensor] == imu_buf_size_arr[sensor]) {
		clear_buf(sensor);
		ble_i.write(imu_char_idx_arr[sensor], (int8_t *)imu_buf_arr[sensor], imu_buf_size_arr[sensor]);
		clear_buf(sensor);
	}
}

void iGlass_ble_send_imu::clear_buf(int sensor) {
	imu_buf_idx_arr[sensor] = 0;
	memset(imu_buf_arr[sensor], 0, sizeof(imu_buf_arr[sensor]));
}