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
	// iGlass_imu imu_i = iGlass_imu();
	
	// #if DEBUG
	// 	Serial.println("Before IMU init...");
	// #endif

	// imu_i.init();

	// #if DEBUG
	// 	Serial.println("IMU initialized!");
	// #endif

	// int16_t imu_data_buf[IMU_FIFO_SIZE*3];

	// #if DEBUG
	// 	Serial.println("Before IMU read...");
	// #endif
	
	// bool read_acc_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, ACC) != 0);
	// bool read_gyro_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, GYRO) != 0);
	// bool read_mag_works = (imu_i.read(imu_data_buf, IMU_FIFO_SIZE*3, MAG) != 0);

	// imu_i.end();

	// #if DEBUG
	// 	if (!read_acc_works) Serial.println("IMU read ACC failed...");
	// 	if (!read_gyro_works) Serial.println("IMU read GYRO failed...");
	// 	if (!read_mag_works) Serial.println("IMU read MAG failed...");
	// #endif

	// if (read_acc_works && read_gyro_works && read_mag_works) {
	// 	#if DEBUG
	// 		Serial.println("IMU test succeeded!");
	// 	#endif
	// 	return SUCCESS;
	// }
	// #if DEBUG
	// 	Serial.println("IMU test failed!");
	// #endif
	// return FAIL;
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
	Function: 	Test the functionality of the sd module
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_sd() {
	iGlass_sd sd_i = iGlass_sd();

	//testing init
	#if DEBUG
		Serial.println("Before SD init...");
	#endif

	sd_i.init();

	int status = SUCCESS;
	if (!sd_i.isSetup()) {
		#if DEBUG
            Serial.println("init() did not successfully setup SD");
        #endif
        status = FAIL;  
	}

	#if DEBUG
		Serial.println("SD initialized!");
	#endif

    if (!sd_i.available()) {
        #if DEBUG
            Serial.println("No SD card detected");
        #endif
        status = FAIL;  
    }
	if (status == FAIL) {
		sd_i.end();
		return FAIL;
	}

	//testing addNewFile
	#if DEBUG
		Serial.println("Before SD addNewFile...");
	#endif

	int test_file_idx = sd_i.addNewFile("unittest_file");
	if (test_file_idx == -1) {
		#if DEBUG
            Serial.println("Unable to add new file");
        #endif
        return FAIL;
	}
	Serial.println("test_file_idx: " + String(test_file_idx));//.........................
	#if DEBUG
		Serial.println("SD new file added!");
	#endif

	//testing write...
	#if DEBUG
		Serial.println("Before SD writes...");
	#endif

	byte test_buf[2*SD_DATA_BUFFER_BYTE_LIMIT];
	byte answer_buf[2*SD_DATA_BUFFER_BYTE_LIMIT];//.......................
	memset(test_buf, (byte)65, 2*SD_DATA_BUFFER_BYTE_LIMIT);//............added(byte)
	status = SUCCESS;
	int num_bytes = 0; //........................................
	memset(test_buf, (byte)66, SD_DATA_BUFFER_BYTE_LIMIT/2);//............added(byte)
	num_bytes = sd_i.write(test_file_idx, test_buf, SD_DATA_BUFFER_BYTE_LIMIT/2);	//...............
	if(!num_bytes)//...........................................
	//if (!sd_i.write(test_file_idx, test_buf, SD_DATA_BUFFER_BYTE_LIMIT/2))
		status = FAIL;
	Serial.println("num_bytes: " + String(num_bytes));	//.....................
	memset(test_buf + SD_DATA_BUFFER_BYTE_LIMIT/2, (byte)67, SD_DATA_BUFFER_BYTE_LIMIT/10);//............added(byte)
	num_bytes = sd_i.write(test_file_idx, test_buf + SD_DATA_BUFFER_BYTE_LIMIT/2, SD_DATA_BUFFER_BYTE_LIMIT/10);//.......................
	if(!num_bytes)//.................................................
	//if (!sd_i.write(test_file_idx, test_buf + SD_DATA_BUFFER_BYTE_LIMIT/2, SD_DATA_BUFFER_BYTE_LIMIT/10))
		status = FAIL;
	Serial.println("num_bytes: " + String(num_bytes));	//.....................
	num_bytes = sd_i.write(test_file_idx, test_buf + SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10, 2*SD_DATA_BUFFER_BYTE_LIMIT - SD_DATA_BUFFER_BYTE_LIMIT/2 - SD_DATA_BUFFER_BYTE_LIMIT/10);//.........................
	if(!num_bytes)
	//if (!sd_i.write(test_file_idx, test_buf + SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10, 2*SD_DATA_BUFFER_BYTE_LIMIT - SD_DATA_BUFFER_BYTE_LIMIT/2 - SD_DATA_BUFFER_BYTE_LIMIT/10))
		status = FAIL;
	Serial.println("num_bytes: " + String(num_bytes));	//.....................
	if (status == FAIL) {
		#if DEBUG
			Serial.println("SD write test failed!");
		#endif
		sd_i.end();
		return FAIL;
	}

	#if DEBUG
		Serial.println("After SD writes");
	#endif

	//testing read
	#if DEBUG
		Serial.println("Before SD read...");
	#endif

	status = SUCCESS;
	Serial.println("Before: " + String(answer_buf[0]));//............................
	//newwwwwwwwint num_bytes_read = sd_i.read(test_file_idx,answer_buf,2*SD_DATA_BUFFER_BYTE_LIMIT);//....test_buf to answer_buf...does not return num bytes read!! 
	sd_i.read(test_file_idx, answer_buf, SD_DATA_BUFFER_BYTE_LIMIT);//newwwwwwwwwwwww
	sd_i.read(test_file_idx, answer_buf + SD_DATA_BUFFER_BYTE_LIMIT, SD_DATA_BUFFER_BYTE_LIMIT);//newwwwwwwwwww
	Serial.println("After: " + String(answer_buf[0]));//.................................
	// if (!num_bytes_read) 
	// 	status = FAIL;
	// if (num_bytes_read < 2*SD_DATA_BUFFER_BYTE_LIMIT) {
	// 	#if DEBUG
	// 		Serial.println("SD only read " + String(num_bytes_read) + "/" + String(2*SD_DATA_BUFFER_BYTE_LIMIT) + " possible bytes");
	// 	#endif
	// 	status = FAIL;
	// }
	// if (status == FAIL) {
	// 	#if DEBUG
	// 		Serial.println("SD read test failed!");
	// 	#endif
	// 	sd_i.end();
	// 	return FAIL;
	// }.................

	#if DEBUG
		Serial.println("SD read is successful!");
	#endif

	//...testing write
	Serial.println(String(answer_buf[SD_DATA_BUFFER_BYTE_LIMIT/2-1]));//.......
	Serial.println(String(answer_buf[SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10-1]));//.......
	Serial.println(String(answer_buf[2*SD_DATA_BUFFER_BYTE_LIMIT-1]));//.......
	if (answer_buf[SD_DATA_BUFFER_BYTE_LIMIT/2-1] != 66 || answer_buf[SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10-1] != 67 || answer_buf[2*SD_DATA_BUFFER_BYTE_LIMIT-1] != 65) {//........test_buf to answer_buf
		#if DEBUG
			Serial.println("SD values were not written correctly");
		#endif
		sd_i.end();
		return FAIL;
	}



	byte answer_buf2[2*SD_DATA_BUFFER_BYTE_LIMIT+11];//.....should get all data correct, meaning that seek for second read is reset, cuz of the write in between..................
	for (int i = 0; i < 69; i++) {
		Serial.println("oldd: " + String(answer_buf2[15*i]));
	}
	//newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	sd_i.read(test_file_idx, answer_buf2, 2*SD_DATA_BUFFER_BYTE_LIMIT);
	//newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	for (int i = 0; i < 69; i++) {
		Serial.println("neww: " + String(answer_buf2[15*i]));
	}
	// Serial.println("Most lkely not 68: " + String(answer_buf2[SD_DATA_BUFFER_BYTE_LIMIT + 9]));
	// sd_i.read(test_file_idx, answer_buf2, 2*SD_DATA_BUFFER_BYTE_LIMIT + 11);
	// Serial.println("Should be 68: " + String(answer_buf2[SD_DATA_BUFFER_BYTE_LIMIT + 9]));
	// Serial.println("random number: " + String(answer_buf2[SD_DATA_BUFFER_BYTE_LIMIT + 9+3]));
	// for (int i = 0; i < 68; i++) {
	// 	Serial.println("new: " + String(answer_buf2[15*i]));
	// }
	// byte answer_buf3[11];//........cyeck if restart read from 0...............
	// for (int i = 0; i < 11; i++) {
	// 	Serial.println("old should start form start: " + String(answer_buf3[i]));
	// }
	// sd_i.read(test_file_idx, answer_buf3, 11);
	// for (int i = 0; i < 11; i++) {
	// 	Serial.println("new: " + String(answer_buf3[i]));
	// }


	sd_i.end();
	#if DEBUG
		Serial.println("SD test succeeded!");
	#endif

	return SUCCESS;
}

/*
    Function:   Test the functionality of ir module
    Input:      None
    Ret Val:    int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_ir() {
    iGlass_ir ir_i = iGlass_ir();

    #if DEBUG
        Serial.println("Before IR init...");
    #endif

    ir_i.init();

    #if DEBUG
        Serial.println("IR initialized!");
    #endif

    #if DEBUG
        Serial.println("Before IR read...");
    #endif

    int gesture, proximity;
    int rgb_col[3];
    for (int i = 0; i < 10; i++) {
        while (!ir_i.read_ges(&gesture, 1));
        Serial.println("Gesture " + String(i) + ": " + String(gesture));
        while (!ir_i.read_col(rgb_col,1));
        Serial.println("RGB color " + String(i) + ": " + String(rgb_col[0]) + "  " + String(rgb_col[1]) + "  " + String(rgb_col[2]));
        while (!ir_i.read_prox(&proximity, 1));
        Serial.println("Proximity " + String(i) + ": " + String(proximity));
    }

    #if DEBUG
        Serial.println("After IR read...");
    #endif

    #if DEBUG
        Serial.println("Before IR print...");
    #endif

    for (i = 0; i < 10; i++) {
        ir_i.print();
    }

    #if DEBUG
        Serial.println("After IR print...");
    #endif

    ir_i.end();

    #if DEBUG
        Serial.println("IR test succeeded!");
    #endif
    return SUCCESS;
}

/*
	Function: 	Test if ble properly sends IMU
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_ble_send_imu(){
	// iGlass_ble ble_i = iGlass_ble();
	// iGlass_imu imu_i = iGlass_imu();
	// const int imu_buf_size = 120;
	// int ble_imu_buf_idx = 0;
	// int imu_char_idx = 0;

	// imu_char_idx = ble_i.addNewCharacteristic(imu_buf_size * sizeof(int16_t));

	// ble_i.init();
	// imu_i.init();

	// int16_t imu_buf[imu_buf_size];
	// memset(imu_buf, 0, imu_buf_size);

	// #if DEBUG
	// 	Serial.println("Connecting to host...");
	// #endif

	// while(!ble_i.available());

	// delay(3000);

	// #if DEBUG
	// 	Serial.println("Connected!");
	// 	Serial.println("Starting to test BLE...");
	// #endif

	// for(int i = 0; i < 1000; i++){
	// 	int status = SUCCESS;

	// 	if (ble_imu_buf_idx < imu_buf_size) {
	// 		int samples_read = imu_i.read(&imu_buf[ble_imu_buf_idx], imu_buf_size - ble_imu_buf_idx, ACC);
	// 		ble_imu_buf_idx += samples_read;			//3 * samples_read : 0;   //kinda want to send separate/spread-out samples instead of blocks of samples............
			
	// 		#if DEBUG
	// 			Serial.println("IMU samples read: " + String(samples_read) + " ble_idx: " + String(ble_imu_buf_idx));
	// 		#endif
	// 	}
		
	// 	if(!ble_i.available())
	// 		status = FAIL;

	// 	if (ble_imu_buf_idx == imu_buf_size){
	// 		if (!ble_i.write(imu_char_idx, (int8_t *)imu_buf, sizeof(imu_buf)))
	// 			status = FAIL;
	// 		else{
	// 			ble_imu_buf_idx = 0;
	// 			memset(imu_buf, 0, sizeof(imu_buf));
	// 		}
	// 	}

	// 	if(status == FAIL){
	// 		#if DEBUG
	// 			Serial.println("BLE test failed!");
	// 		#endif
	// 		ble_i.end();
	// 		imu_i.end();
	// 		return FAIL;
	// 	}

	// 	delay(10);
	// }
	
	// ble_i.end();
	// imu_i.end();
	// #if DEBUG
	// 	Serial.println("BLE test succeeded!");
	// #endif
	// return SUCCESS;
}

/*
	Function: 	Test all the unittests
	Input: 		nothing
	Ret Val: 	int - SUCCESS or FAIL
*/
int iGlass_unittest::unittest_all() {
	int sd_works = unittest_sd();
	// int imu_works = unittest_imu();
	// Hardware Problem: After BLE.end(), BLE.begin() doesn't run succesfully 
	// int ble_works = unittest_ble();				
	//int ble_send_imu_works = unittest_ble_send_imu();

	if (sd_works){ //imu_works && ble_works && ble_send_imu_works){
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