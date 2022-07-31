#include <iGlass.h>

void setup() {

	/*Unittest init*/
	Serial.println("Before initialization...");

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	Serial.println("Initialized!");

    /*BLE unittest*/
    iGlass_ble ble_i = iGlass_ble();


    Serial.println("Before BLE addNewCharacteristic...");
	int test_char_idx = ble_i.addNewCharacteristic(BLE_MAX_SEND_SIZE);
    Serial.println("BLE new characteristic added!");


    Serial.println("Before BLE init...");
	ble_i.init();
    Serial.println("BLE initialized!");

    uint8_t test_buf[2*BLE_MAX_SEND_SIZE];
	memset(test_buf, 1, 2*BLE_MAX_SEND_SIZE);

	Serial.println("Connecting to host...");
	while(!ble_i.available());
	delay(3000);
	Serial.println("Connected!");

    
	Serial.println("Starting to test BLE...");

	bool success;

	for(int i = 0; i < 500; i++){
		success = true;
		
		if(!ble_i.available()) {
			success = false;
			Serial.println("BLE available() is false");
			break;
		}

		if(!ble_i.write(test_char_idx, (int8_t *)test_buf, sizeof(test_buf))) {
			success = false;
			Serial.println("BLE write # " + String(i) +" failed"); 
			break;
		}

		delay(10);
	}

	ble_i.end();

	if (success) {
		Serial.println("BLE test succeeded!");
	} else {
		Serial.println("BLE test failed!");
	}
}


void loop() {
}
