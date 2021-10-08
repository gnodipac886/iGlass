//#include <SdFat.h>
#include <Arduino_LSM9DS1.h>

void setup_imu() {
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
	imu_setup_flag = 1;
}

void end_imu() {
	IMU.end();
	imu_setup_flag = 0;
}

void save_imu_data(){
	int_imu = 0;
	imu_file.write((byte*)imu_sampleBuffer, (size_t)IMU_BUF_SIZE);
	imu_file.flush();
	if(int_imu){
		#if DEBUG
	    Serial.println("imu int");
	    #endif
  	}
}

void update_IMU() {
	bool avail[3];

	if (avail[0] = IMU.accelerationAvailable()) {
		IMU.readAcceleration(ax, ay, az);
	}

	if (avail[1] = IMU.gyroscopeAvailable()) {
		IMU.readGyroscope(wx, wy, wz);
	}

	if (avail[2] = IMU.magneticFieldAvailable()) {
		IMU.readMagneticField(mx, my, mz);
	}

	if(avail[0] || avail[1] || avail[2]){
		float temp_data[9] = {ax, ay, az, wx, wy, wz, mx, my, mz};
		memcpy(&imu_sampleBuffer[imu_buf_idx], temp_data, sizeof(float) * 9);
		imu_buf_idx += 9;
		if(imu_buf_idx == IMU_BUF_SIZE){
			#if DEBUG
			Serial.println("saving IMU data");
			#endif
			save_imu_data();
			imu_buf_idx = 0;
		}
	}
}

void print_IMU() {
	#if DEBUG
	Serial.print("acc:\t" + String(ax) + "\t" + String(ay) + "\t" + String(az) + "\t\t");
	Serial.print("ang:\t" + String(wx) + "\t" + String(wy) + "\t" + String(wz) + "\t\t");
	Serial.println("mag:\t" + String(mx) + "\t" + String(my) + "\t" + String(mz) + "\t\t");
	#endif
}
