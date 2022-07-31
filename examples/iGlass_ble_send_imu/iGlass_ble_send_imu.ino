#include <iGlass.h>

#define ACC_BUF_SIZE 	(int)(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXES) * NUM_AXES
#define GYRO_BUF_SIZE 	(int)(BLE_MAX_SEND_SIZE / sizeof(int16_t) / NUM_AXES) * NUM_AXES
#define MAG_BUF_SIZE 	5 * NUM_AXES    //..............chose 5 arbitrarily

iGlass_imu imu_i = iGlass_imu();
iGlass_ble ble_i = iGlass_ble();

int acc_char_idx, gyro_char_idx, mag_char_idx;

int16_t *acc_buf, *gyro_buf, *mag_buf;
int acc_buf_num_data_int16s, gyro_buf_num_data_int16s, mag_buf_num_data_int16s;

void setup() {

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	imu_i.init();

    acc_char_idx = ble_i.addNewCharacteristic(ACC_BUF_SIZE * sizeof(int16_t));  //............make sure u know what addnewcharacteritic does
	gyro_char_idx = ble_i.addNewCharacteristic(GYRO_BUF_SIZE * sizeof(int16_t));
	mag_char_idx = ble_i.addNewCharacteristic(MAG_BUF_SIZE * sizeof(int16_t));

	ble_i.init();

    acc_buf = (int16_t*)malloc(ACC_BUF_SIZE * sizeof(int16_t));   //....greater than imu fifo size.......???????? find out imu vs ble frequency
	gyro_buf = (int16_t*)malloc(GYRO_BUF_SIZE * sizeof(int16_t));
	mag_buf = (int16_t*)malloc(MAG_BUF_SIZE * sizeof(int16_t));//((MAG_BUF_SIZE + 6) * sizeof(int16_t));

    memset(acc_buf, 0, sizeof(acc_buf));
    memset(gyro_buf, 0, sizeof(gyro_buf));
    memset(mag_buf, 0, sizeof(mag_buf));

    acc_buf_num_data_int16s = 0;
    gyro_buf_num_data_int16s = 0;
    mag_buf_num_data_int16s = 0;

	while(!ble_i.available());
	//..............delay(3000);

}


void loop() {

	/*Updating Gyroscope Data*/
	if (gyro_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE < GYRO_BUF_SIZE) {
		gyro_buf_num_data_int16s += imu_i.read((int16_t *)(gyro_buf + gyro_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE, GYRO);
	} else if (gyro_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE == GYRO_BUF_SIZE) {
		gyro_buf_num_data_int16s += imu_i.read((int16_t *)(gyro_buf + gyro_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE, GYRO);

		if (ble_i.available()) {
			if (!ble_i.write(gyro_char_idx, (int8_t *)gyro_buf, sizeof(gyro_buf))) {
				Serial.println("BLE ACC write failed!");
			}
		}
		memset(gyro_buf, 0, sizeof(gyro_buf));	//not really needed if we keep track of buf_num_data_int16s...............
		gyro_buf_num_data_int16s = 0;
	} else {	// gyro_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE > GYRO_BUF_SIZE
		int gyro_first_read_num_data_int16s = GYRO_BUF_SIZE - gyro_buf_num_data_int16s;
		gyro_buf_num_data_int16s += imu_i.read((int16_t *)(gyro_buf + gyro_buf_num_data_int16s), gyro_first_read_num_data_int16s, GYRO);

		if (ble_i.available()) {
			if (!ble_i.write(gyro_char_idx, (int8_t *)gyro_buf, sizeof(gyro_buf))) {
				Serial.println("BLE GYRO write failed!");
			}
		}
		memset(gyro_buf, 0, sizeof(gyro_buf));
		gyro_buf_num_data_int16s = 0;

		gyro_buf_num_data_int16s += imu_i.read((int16_t *)(gyro_buf + gyro_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE - gyro_first_read_num_data_int16s, GYRO);	
	}

	/*Updating Accelerometer Data*/
	if (acc_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE < ACC_BUF_SIZE) {
		acc_buf_num_data_int16s += imu_i.read((int16_t *)(acc_buf + acc_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE, ACC);
	} else if (acc_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE == ACC_BUF_SIZE) {
		acc_buf_num_data_int16s += imu_i.read((int16_t *)(acc_buf + acc_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE, ACC);

		if (ble_i.available()) {
			if (!ble_i.write(acc_char_idx, (int8_t *)acc_buf, sizeof(acc_buf))) {
				Serial.println("BLE write failed!");
			}
		}
		memset(acc_buf, 0, sizeof(acc_buf));	//not really needed if we keep track of buf_num_data_int16s...............
		acc_buf_num_data_int16s = 0;
	} else {	// acc_buf_num_data_int16s + NUM_AXES * IMU_FIFO_SIZE > ACC_BUF_SIZE
		int acc_first_read_num_data_int16s = ACC_BUF_SIZE - acc_buf_num_data_int16s;
		acc_buf_num_data_int16s += imu_i.read((int16_t *)(acc_buf + acc_buf_num_data_int16s), acc_first_read_num_data_int16s, ACC);	//needa check imu_i.read ret val!!!!!!!!.....................

		if (ble_i.available()) {
			if (!ble_i.write(acc_char_idx, (int8_t *)acc_buf, sizeof(acc_buf))) {
				Serial.println("BLE write failed!");
			}
		}
		memset(acc_buf, 0, sizeof(acc_buf));
		acc_buf_num_data_int16s = 0;

		acc_buf_num_data_int16s += imu_i.read((int16_t *)(acc_buf + acc_buf_num_data_int16s), NUM_AXES * IMU_FIFO_SIZE - acc_first_read_num_data_int16s, ACC);	
	}

	/*Updating Magnetometer Data*/
	if (mag_buf_num_data_int16s + NUM_AXES > MAG_BUF_SIZE) {
		if (ble_i.available()) {
			if (!ble_i.write(mag_char_idx, (int8_t *)mag_buf, sizeof(mag_buf))) {
				Serial.println("BLE MAG write failed!");
			}
		}
		memset(mag_buf, 0, sizeof(mag_buf));
		mag_buf_num_data_int16s = 0;
	}
	mag_buf_num_data_int16s += imu_i.read((int16_t *)(mag_buf + mag_buf_num_data_int16s), NUM_AXES, MAG);


    delay(10);
}