#ifndef iGLASS_UNITTEST_H
#define iGLASS_UNITTEST_H

#include "arduino.h"
#include "../hardware/iGlass_ble.h"
#include "../hardware/iGlass_imu.h"
#include "../hardware/iGlass_sd.h"

#define SUCCESS 1
#define FAIL 	0
#define DEBUG 1

class iGlass_unittest{
	public:
		int init();
		int unittest_all();
		int unittest_imu();
		int unittest_ble_send_imu();
		int unittest_ble();
		int unittest_sd();

	private:
};

#endif