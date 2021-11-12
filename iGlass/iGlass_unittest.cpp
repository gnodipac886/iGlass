#include "iGlass_unittest.h"

/*
	Function: 	Initialize the unittest class
	Input: 		None
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::init(){

	#if DEBUG
		Serial.println("Before initialization...");
	#endif

	if(!Serial && DEBUG){
		Serial.begin(115200);
		while(!Serial);
	}

	#if DEBUG
		Serial.println("Initialized!");
	#endif

	return SUCCESS;
}

/*
	Function: 	Test the functionality of imu module
	Input: 		None
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_imu() {
	iGlass_imu imu_i = iGlass_imu();
	
	#if DEBUG
		Serial.println("Before IMU init...");
	#endif

	imu_i.init();

	#if DEBUG
		Serial.println("IMU initialized!");
	#endif

	int16_t imu_data_buf[IMU_FIFO_SIZE*3];

	#if DEBUG
		Serial.println("Before IMU read...");
	#endif
	
	bool read_acc_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, ACC) != 0);
	bool read_gyro_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, GYRO) != 0);
	bool read_mag_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, MAG) != 0);

	imu_i.end();

	#if DEBUG
		if (!read_acc_works) Serial.println("IMU read ACC failed...");
		if (!read_gyro_works) Serial.println("IMU read GYRO failed...");
		if (!read_mag_works) Serial.println("IMU read MAG failed...");
	#endif

	if (read_acc_works && read_gyro_works && read_mag_works) {
		#if DEBUG
			Serial.println("IMU test succeeded!");
		#endif
		return SUCCESS;
	}
	#if DEBUG
		Serial.println("IMU test failed!");
	#endif
	return FAIL;
}

/*
	Function: 	Test the functionality of the ble module
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_ble() {
	iGlass_ble ble_i = iGlass_ble();

	int test_char_idx = ble_i.addNewCharacteristic(BLE_MAX_SEND_SIZE);

	ble_i.init();

	uint8_t test_buf[BLE_MAX_SEND_SIZE];
	memset(test_buf, 1, BLE_MAX_SEND_SIZE);

	#if DEBUG
		Serial.println("Connecting to host...");
	#endif

	while(!ble_i.available());

	delay(3000);

	#if DEBUG
		Serial.println("Connected!");
		Serial.println("Starting to test BLE...");
	#endif
	
	for(int i = 0; i < 500; i++){
		int status = SUCCESS;
		
		if(!ble_i.available())
			status = FAIL;

		if(!ble_i.write(test_char_idx, (int8_t *)test_buf, sizeof(test_buf)))
			status = FAIL;

		if(status == FAIL){
			#if DEBUG
				Serial.println("BLE test failed!");
			#endif
			ble_i.end();
			return FAIL;
		}

		delay(10);
	}

	ble_i.end();
	#if DEBUG
		Serial.println("BLE test succeeded!");
	#endif
	return SUCCESS;
}


/*
	Function: 	Test if ble properly sends IMU
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_ble_send_imu(){
	iGlass_ble ble_i = iGlass_ble();
	iGlass_imu imu_i = iGlass_imu();
	const int imu_buf_size = 120;
	int ble_imu_buf_idx = 0;
	int imu_char_idx = 0;

	imu_char_idx = ble_i.addNewCharacteristic(imu_buf_size * sizeof(int16_t));

	ble_i.init();
	imu_i.init();

	int16_t imu_buf[imu_buf_size];
	memset(imu_buf, 0, imu_buf_size);

	#if DEBUG
		Serial.println("Connecting to host...");
	#endif

	while(!ble_i.available());

	delay(3000);

	#if DEBUG
		Serial.println("Connected!");
		Serial.println("Starting to test BLE...");
	#endif

	for(int i = 0; i < 1000; i++){
		int status = SUCCESS;

		if (ble_imu_buf_idx < imu_buf_size) {
			int samples_read = imu_i.read(&imu_buf[ble_imu_buf_idx], imu_buf_size - ble_imu_buf_idx, ACC);
			ble_imu_buf_idx += samples_read;			//3 * samples_read : 0;   //kinda want to send separate/spread-out samples instead of blocks of samples............
			
			#if DEBUG
				Serial.println("IMU samples read: " + String(samples_read) + " ble_idx: " + String(ble_imu_buf_idx));
			#endif
		}
		
		if(!ble_i.available())
			status = FAIL;

		if (ble_imu_buf_idx == imu_buf_size){
			if (!ble_i.write(imu_char_idx, (int8_t *)imu_buf, sizeof(imu_buf)))
				status = FAIL;
			else{
				ble_imu_buf_idx = 0;
				memset(imu_buf, 0, sizeof(imu_buf));
			}
		}

		if(status == FAIL){
			#if DEBUG
				Serial.println("BLE test failed!");
			#endif
			ble_i.end();
			imu_i.end();
			return FAIL;
		}

		delay(10);
	}
	
	ble_i.end();
	imu_i.end();
	#if DEBUG
		Serial.println("BLE test succeeded!");
	#endif
	return SUCCESS;
}

/*
	Function: 	Test all the unittests
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_all() {
	int imu_works = unittest_imu();
	// Hardware Problem: After BLE.end(), BLE.begin() doesn't run succesfully 
	// int ble_works = unittest_ble();				
	int ble_send_imu_works = unittest_ble_send_imu();

	if (imu_works && ble_send_imu_works){ //ble_works && ble_send_imu_works){
		#if DEBUG
			Serial.println("All tests succeeded!");
		#endif
		return SUCCESS;
	}

	#if DEBUG
		Serial.println("Tests failed!");
	#endif
	return FAIL;
}