#include "iGlass.h"

//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------
iGlass_ble 	ble_i;
int 		imu_char_idx, tmp_char_idx, clr_char_idx, mic_char_idx;
int16_t 	ble_IMU_buf[BLE_IMU_BUF_SIZE];
int			ble_imu_buf_idx = 0;
int 		angle_char_idx = 0;
int     acc_gyro_char_idx = 0;

//----------------------------------------------------------------------------------------------------------------------
// IMU
//----------------------------------------------------------------------------------------------------------------------
iGlass_imu imu_i;

//----------------------------------------------------------------------------------------------------------------------
// unittesting
//----------------------------------------------------------------------------------------------------------------------
iGlass_unittest unittest_i;

void setup() {
	#if DEBUG
		if (!Serial) {
			Serial.begin(115200);
			while (!Serial);
		}
	#endif

	pinMode(LED_BUILTIN, OUTPUT);

	//setup_ble_angle();
	setup_ble_acc_gyro();
	// unittest_i = iGlass_unittest();
	// unittest_i.init();
	// unittest_i.unittest_imu();
	// unittest_i.unittest_ble();
	// unittest_i.unittest_all();

	#if DEBUG
		Serial.println("BLE setup flag: " + String(ble_i.isSetup()));
	#endif
}

void setup_ble_angle() {
	imu_i = iGlass_imu();
	imu_i.init();
	
	ble_i = iGlass_ble();
	angle_char_idx = ble_i.addNewCharacteristic(sizeof(int8_t));
	ble_i.init();
}

void setup_ble_acc_gyro() {
	imu_i = iGlass_imu();
	imu_i.init();
	
	ble_i = iGlass_ble();
	acc_gyro_char_idx = ble_i.addNewCharacteristic(int(BLE_IMU_BUF_SIZE/6)*6*sizeof(int16_t));
	ble_i.init();
}

/*
	Function:	Calculates change in orientation on the xy-plane
	Input:		None  
	Ret Val: 	angle (degrees)
*/
float calculate_theta() {
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


/*
	Function:	Update theta; Send rounded angle index if updated via BLE
	Input:		None  
	Variables:  rounded_angle - orientation on xy-plane to nearest 10th degree
	Ret Val: 	None
*/

void update_ble_rounded_angle(){
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
	Function:	Update IMU accelerometer and gyroscope values; Sends 3-degrees of accelerometer samples alternating with 3-degress of gyroscope samples, if updated, via BLE
	Input:		None  
	Ret Val: 	None
*/
void update_ble_acc_gyro(){
	static int acc_gyro_buf_idx = 0;
	static int16_t acc_gyro_buf[int(BLE_IMU_BUF_SIZE/6)*6];		//3-degrees of ACC; 3-degrees of GYRO

	bool read_acc_works = imu_i.read(&acc_gyro_buf[acc_gyro_buf_idx],3,ACC) != 0;
	bool read_gyro_works = imu_i.read(&acc_gyro_buf[acc_gyro_buf_idx+3],3,GYRO) != 0;

	if (!read_acc_works && !read_gyro_works) {
		#if DEBUG
			Serial.println("No acc_gyro samples read!");
		#endif
		return;
	}

	acc_gyro_buf_idx += 6;

	if (ble_i.available() && acc_gyro_buf_idx == int(BLE_IMU_BUF_SIZE/6)*6) {
		#if DEBUG
			Serial.println("BLE write!");
		#endif
		ble_i.write(acc_gyro_char_idx, (int8_t *)acc_gyro_buf, sizeof(acc_gyro_buf));
		acc_gyro_buf_idx = 0;
		memset(acc_gyro_buf,0,sizeof(acc_gyro_buf));
	} else if (acc_gyro_buf_idx == int(BLE_IMU_BUF_SIZE/6)*6) {
		acc_gyro_buf_idx = 0;
		#if DEBUG
			Serial.println("Ble not available");
		#endif
	}
}

void loop() {
	//update_ble_rounded_angle();
	update_ble_acc_gyro();
}
