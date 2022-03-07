#ifndef iGLASS_BLE_ROUNED_ANGLE
#define iGLASS_BLE_ROUNED_ANGLE

#include "../hardware/iGlass_ble.h"
#include "../hardware/iGlass_imu.h"
#include "iGlass_api.h"

class iGlass_ble_rounded_angle : public iGlass_api
{
	public:
		void init();
		void main_task();

	private:
		void update_ble_rounded_angle();
        float calculate_theta(); 
		iGlass_ble ble_i;
		iGlass_imu imu_i;
		int angle_char_idx = 0;
};

#endif