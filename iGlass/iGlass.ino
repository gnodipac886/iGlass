#include "iGlass.h"
#include "iGlass_imu.h"

iGlass_imu imu_i;

void setup() {
	#if DEBUG
		if (!Serial) {
			Serial.begin(115200);
			// while (!Serial);
		}
	#endif

	pinMode(LED_BUILTIN, OUTPUT);

	imu_i = iGlass_imu();
	imu_i.init();
}

void IMU_TEST() {
	// Serial.println("test");
	// int16_t buf[32 * 3];
	// int pts = imu_i.read(buf, ACC);

	#if DEBUG
		// Serial.println("IMU test case, points: " + String(pts) + "/32");
		imu_i.print();
	#endif
}

void loop() {
	IMU_TEST();
	delay(1000);
}
