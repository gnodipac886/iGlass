#include <iGlass.h>

// iGlass_ble_send_imu ble_send_imu_i;
// iGlass_ble_rounded_angle rounded_angle_i;
iGlass_unittest unittest_i;

void setup() {
	#if DEBUG
		if (!Serial) {
			Serial.begin(115200);
			while (!Serial);
		}
	#endif

	pinMode(LED_BUILTIN, OUTPUT);

	unittest_i = iGlass_unittest();
	unittest_i.init();
	// unittest_i.unittest_imu();
	// unittest_i.unittest_ble();
	unittest_i.unittest_sd();
	// unittest_i.unittest_all();

	#if DEBUG
		//......Serial.println("BLE setup flag: " + String(ble_i.isSetup()));
	#endif
}


void loop() {
	// ble_send_imu_i.main_task();
	// rounded_angle_i.main_task();
}