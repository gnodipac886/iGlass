#ifndef iGLASS_BLE_H
#define iGLASS_BLE_H

#include "arduino.h"
#include <ArduinoBLE.h>

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define BLE_MAX_SEND_SIZE             	244
#define MAX_NUM_CHAR 					6

//---------------------------------------------------------------------------------------------------------------------
// Others
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// iGlass ble class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_ble {
	public:
		void 		init();
		int 		write(int char_idx, int8_t *buf, int buf_size);
		int 		read();
		void 		end();
		int 		addNewCharacteristic(int buf_size);
		bool		available();
		int 		isSetup();

	private:
		BLEDevice 	central;
		BLECharacteristic * ble_des[MAX_NUM_CHAR];
		int 		ble_char_idx = 0;			// keeps track of next available characteristic index
		BLEService 	iGlass_ble_service;
		int 		CENTRAL_FLAG = 0;
		int 		char_uuid = 2713;
		int 		ble_imu_buf_idx = 0;
		char *		BLE_UUID_TEST_SERVICE = "9A48ECBA-2E92-082F-C079-9E75AAE428B1";
		int 		BLE_LED_PIN = LED_BUILTIN;
		int 		RSSI_LED_PIN = LED_PWR;
		int 		ble_setup_flag = 0;
		
		bool 		configure_ble();
};

#endif