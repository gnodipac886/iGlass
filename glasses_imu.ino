//#include <SdFat.h>
// #include <Arduino_LSM9DS1.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>

#define LSM9DS1_ADDRESS            0x6b

#define LSM9DS1_WHO_AM_I           0x0f
#define LSM9DS1_CTRL_REG1_G        0x10
#define LSM9DS1_STATUS_REG         0x17
#define LSM9DS1_OUT_X_G            0x18
#define LSM9DS1_CTRL_REG6_XL       0x20
#define LSM9DS1_CTRL_REG8          0x22
#define LSM9DS1_OUT_X_XL           0x28

// magnetometer
#define LSM9DS1_ADDRESS_M          0x1e

#define LSM9DS1_CTRL_REG1_M        0x20
#define LSM9DS1_CTRL_REG2_M        0x21
#define LSM9DS1_CTRL_REG3_M        0x22
#define LSM9DS1_CTRL_REG4_M        0x23
#define LSM9DS1_STATUS_REG_M       0x27
#define LSM9DS1_OUT_X_L_M          0x28

// accelerometer
#define ax_WEIGHT					0.5
#define ay_WEIGHT					1.0
#define az_WEIGHT					0.5

void setup_imu() {
  if (imu_setup_flag == 1) {return;}
	#if DEBUG
	if(!Serial){
		Serial.begin(115200);
		while (!Serial);
	}
	#endif

	if(!imu_setup_flag){
		if (!IMU.begin()) {
			#if DEBUG
			Serial.println("Failed to initialize IMU!");
			#endif
			while (1);
		}
	}
	IMU.setContinuousMode();
	IMU.setAccelFS(3);			 // Full Scale's multiplication factor is +-8g (does not change output of read functions, just assigns more/less bits to sensor measurement)
	IMU.setAccelODR(5);			 // Sampling Rate is 476 Hz
	IMU.setAccelOffset(0, 0, 0); //   uncalibrated <---------why? because diff for walk/talk?...................
	IMU.setAccelSlope(1, 1, 1);	 //   uncalibrated <--------- can use instead of weights?..........
	imu_setup_flag = 1;
}

void end_imu() {
  if (imu_setup_flag == 0) {return;}
  Serial.println("In IMU end");
	IMU.end();
  Serial.println("IMU ended!!!");
	imu_setup_flag = 0;
}

void save_imu_data(){
	int_imu = 0;
	imu_file.write((byte*)imu_sampleBuffer, (size_t)IMU_BUF_SIZE); //....................
	imu_file.flush();
	if(int_imu){
		#if DEBUG
	    Serial.println("imu int");
	    #endif
  	}
}

int update_IMU(float * buf) {
	int avail[3];
	int ax, ay, az;
	//IMU.readAcceleration(ax, ay, az)
	if (avail[0] = IMU.accelerationAvailable()) {
		if (avail[0] < 32) return 0;
		for(int i = 0; i < 61; i++){					//20; i++){
			IMU.readAcceleration(ax, ay, az)			//buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
			buf[i] = sqrt(square(ax_WEIGHT*ax)+square(ay_WEIGHT*ay)+square(az_WEIGHT*az))
		}
		return 61;			//20;
	}

	// if (avail[1] = IMU.gyroscopeAvailable()) {
	// 	IMU.readGyroscope(wx, wy, wz);
	// }

	// if (avail[2] = IMU.magneticFieldAvailable()) {
	// 	IMU.readMagneticField(mx, my, mz);
	// }

	if(avail[0]){
		float temp_data = sqrt(square(ax_WEIGHT*ax)+square(ay_WEIGHT*ay)+square(az_WEIGHT*az))				//temp_data[3] = {ax, ay, az};
		memcpy(buf, temp_data, sizeof(float))	// * 3);
		// float temp_data[9] = {ax, ay, az, wx, wy, wz, mx, my, mz};
		// memcpy(buf, temp_data, sizeof(float) * 9);
		return 1;
		// FIXME: add to sd card
		// if(imu_buf_idx == IMU_BUF_SIZE){
		// 	#if DEBUG
		// 	Serial.println("saving IMU data");
		// 	#endif
		// 	save_imu_data();
		// 	imu_buf_idx = 0;
		// }
	}

	return 0;
}

void print_IMU() {
	#if DEBUG
	Serial.print("acc:\t" + String(ax) + "\t" + String(ay) + "\t" + String(az) + "\t\t");
	Serial.print("ang:\t" + String(wx) + "\t" + String(wy) + "\t" + String(wz) + "\t\t");
	Serial.println("mag:\t" + String(mx) + "\t" + String(my) + "\t" + String(mz) + "\t\t");
	#endif
}
