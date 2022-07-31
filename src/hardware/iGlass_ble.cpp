#include "iGlass_ble.h"

/*
	Function: 	Begin and setup current instance of BLE
	Input: 		None
	Ret Val: 	None
*/
void iGlass_ble::init() {
	if (ble_setup_flag == 1) 
		return;
	
	// initialize the rest of the array
	for(int i = ble_char_idx; i < MAX_NUM_CHAR; i++){
		ble_des[i] = NULL;
	}
	

	iGlass_ble_service = BLEService(BLE_UUID_TEST_SERVICE);
	

	if (configure_ble())
		digitalWrite(BLE_LED_PIN, HIGH);
	

	// rgb
	// rgb_setColor(0,0,255);
	ble_setup_flag = 1;
}

int iGlass_ble::isSetup() {
	return ble_setup_flag;
}

/*
	Function: 	add new characteristic to the ble stack
	Input: 		buf_size - buffer size
	Ret Val: 	idx - the handle index to the ble_des array like a file descriptor
*/
int iGlass_ble::addNewCharacteristic(int buf_size) {
	char uuid_buf[5];
	String(char_uuid++).toCharArray(uuid_buf, 5);
	ble_des[ble_char_idx] = new BLECharacteristic(uuid_buf, BLERead | BLENotify, buf_size, 2); //............................??
	// iGlass_ble_service.addCharacteristic(*ble_des[ble_char_idx]);
	return ble_char_idx++;
}

/*
	Function: 	Write values of iGlass characteristic
	Input: 		char_idx - index (to ble_des array) of the characteristic to be updated
				buf - data array containing values we want to write
				bytes_to_write - number of bytes to write
	Ret Val: 	1 - success; 0 - failure (on writing entire bytes_to_write data)
*/
int iGlass_ble::write(int char_idx, int8_t * buf, int bytes_to_write) {
	int bytes_written = 0;
	int bytes_left_to_write = bytes_to_write;
	while (bytes_left_to_write > BLE_MAX_SEND_SIZE) {
		if (!ble_des[char_idx]->writeValue(buf+bytes_written, BLE_MAX_SEND_SIZE)) return 0;
		bytes_written += BLE_MAX_SEND_SIZE;
		bytes_left_to_write -= BLE_MAX_SEND_SIZE;
	}
	return ble_des[char_idx]->writeValue(buf+bytes_written, (int32_t)bytes_left_to_write);
}

/*
	Function:	Nothing, placeholder
	Input:		None
	Ret Val: 	1 - success
*/
int iGlass_ble::read(){
	return 1;
}


/*
	Function: 	properly configures the ble stack
	Input: 		nothing
	Ret Val: 	true - succes; false - failure
*/
bool iGlass_ble::configure_ble() {
	if (!BLE.begin()){		
		return false;
	}
	
	// set advertised local name and service UUID:
	BLE.setDeviceName("NANO_IMU");
	BLE.setLocalName("NANO_IMU");
	BLE.setAdvertisedService(iGlass_ble_service);
	

	for(int i = 0; i < ble_char_idx; i++) {
		if(ble_des[i] != NULL)
			iGlass_ble_service.addCharacteristic(*ble_des[i]);
	}
	

	// add service
	BLE.addService(iGlass_ble_service);
	

	// set the initial value for the characeristic:
	for(int i = 0; i < ble_char_idx; i++) {
		if(ble_des[i] != NULL)
			ble_des[i]->writeValue((int32_t)0.0);
	}
	

	BLE.advertise();
	

	return true;
}

/*
	Function: 	checks if the host is available
	Input: 		nothing
	Ret Val: 	bool - return if host is connected or not
*/
bool iGlass_ble::available() {
	// listen for BLE central (clients kinda) to connect:
	if (!CENTRAL_FLAG){
		central = BLE.central();
		if(central)
			CENTRAL_FLAG = 1;
	}

	if(CENTRAL_FLAG && central.connected()){
		for(int i = 0; i < ble_char_idx; i++){
			// if(ble_des[i] != NULL)
			// 	ble_des[i]->writeValue((int32_t)0.0);
		}
	}

	return CENTRAL_FLAG;
}

/*
	Function: 	ends the current instance of BLE
	Input: 		nothing
	Ret Val: 	nothing
*/
void iGlass_ble::end() {
	if (ble_setup_flag == 0)
		return;
	

	for(int i = 0; i < ble_char_idx; i++){		
		if(ble_des[i] != NULL) {
			delete(ble_des[i]);
			ble_des[i] = NULL;
		}
	}

	ble_char_idx = 0;

	BLE.disconnect();
	BLE.end();
	ble_setup_flag = 0;	
}