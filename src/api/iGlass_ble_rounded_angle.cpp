#include "iGlass_ble_rounded_angle.h"

/*
	Function:	init; initiates imu and ble
	Input:		None  
	Variables:  None
	Ret Val: 	None
*/

void iGlass_ble_rounded_angle::init() {
	imu_i = iGlass_imu();
	imu_i.init();
	
	ble_i = iGlass_ble();
	angle_char_idx = ble_i.addNewCharacteristic(sizeof(int8_t));
	ble_i.init();
}

/*
	Function:	main_task; repeatidly updates the characteristic
	Input:		None  
	Variables:  None
	Ret Val: 	None
*/

void iGlass_ble_rounded_angle::main_task(){
    update_ble_rounded_angle();
}

/*
	Function:	Update theta; Send rounded angle index if updated via BLE
	Input:		None  
	Variables:  rounded_angle - orientation on xy-plane to nearest 10th degree
	Ret Val: 	None
*/

void iGlass_ble_rounded_angle::update_ble_rounded_angle(){
	static int rounded_angle = 0;
	int new_rounded_angle = (int(calculate_theta() + 5) / 10 * 10) % 360;

	if (ble_i.available() && rounded_angle != new_rounded_angle) {
		int8_t ble_send_angle_idx = int8_t(new_rounded_angle / 10);
		rounded_angle = new_rounded_angle;
		
		ble_i.write(angle_char_idx, &ble_send_angle_idx, sizeof(int8_t));

		#if DEBUG
			Serial.println(rounded_angle);
		#endif
	}
} 

/*
	Function:	Calculates change in orientation on the xy-plane
	Input:		None  
	Ret Val: 	angle (degrees)
*/
float iGlass_ble_rounded_angle::calculate_theta() {
	static float angle = 0;
	static float prev_time = micros();

	int16_t w_pts[3]; //angular velocity (degrees/s) 3-degrees
	bool read_gyro_works = imu_i.read(w_pts, 3, GYRO) != 0;
	
	if (!read_gyro_works) return angle;

	float curr_time = micros();
	float time_diff = curr_time - prev_time;

	if (abs(float(w_pts[1]) * time_diff / (32768.0 * 1000.0)) >= 0.015) {	//32768.0, 1000, 0,015 are calibration numbers
		angle += float(w_pts[1]) * (180/154.5) * time_diff / (32768.0 * 1000.0); 
		angle -= angle >= 360 ? 360 : 0;
		angle += angle < 0 ? 360 : 0;
	}

	prev_time = curr_time;

	return angle;
}