#ifndef iGLASS_UNITTEST_H
#define iGLASS_UNITTEST_H

#include "arduino.h"
#include "../iGlass.h"
#include "../hardware/iGlass_ble.h"
#include "../hardware/iGlass_imu.h"

#define SUCCESS 1
#define FAIL 	0

class iGlass_unittest{
	public:
		int init();
		int unittest_all();
		int unittest_imu();
		int unittest_ble_send_imu();
		int unittest_ble();

	private:
};


#endif
