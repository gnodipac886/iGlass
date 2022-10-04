#include "iGlass_imu.h"

/*
	Function: 	Begin and setup current instance of IMU
	Input: 		None
	Ret Val: 	EXECUTION_SUCCESS
				EXECUTION_FAILURE - failure in beginning instance of IMU...........maybe include failure in setting up instance of IMU in the future
*/
int iGlass_imu::init() {
	if (imu_setup_flag == 1) 
		return EXECUTION_SUCCESS;

	#if DEBUG_IMU
		if(!Serial){
			Serial.begin(115200);
			while (!Serial);
		}
	#endif

	if (!IMU.begin()) {
		#if DEBUG_IMU
			Serial.println("Failed to initialize IMU!");
		#endif
		return EXECUTION_FAILURE;
	}

	IMU.setContinuousMode();      // FIFO continuously filled

	/*ODR Output Data Rate (Sampling Rate)*/
	IMU.setAccelODR(5);			 // (0..5) -> { off, 10, 50, 119, 238, 476}  Hz  default = 119hz
	IMU.setGyroODR(5);			 // (0..5) -> { off, 10, 50, 119, 238, 476}  Hz  default = 119hz
	IMU.setMagnetODR(8);		 // (0..8) -> {0.625, 1.25, 2.5, 5, 10, 20, 40, 80, 400} Hz  default = 40hz

	/*FS Full Scale*/
	IMU.setAccelFS(3);          // (0..3) -> {±2, ±24, ±4, ±8}  g  default = ±4g
	IMU.setGyroFS(2);	        // (0..3) -> {±245, ±500, ±1000, ±2000}  °/s  default = ±2000°/s
	IMU.setMagnetFS(0);         // (0..3) -> {±400, ±800, ±1200, ±1600}  µT  default = ±400µT.........................first collect magnetometer data then choose FS

	/*Bandwidth*/
	// float setAccelBW(uint8_t range); //0,1,2,3 Override autoBandwidth setting see doc.table 67; if this function is called, the current accelBW should not change (see the function definition comments)
	// int   setGyroBW(uint8_t range);  //Bandwidth setting 0,1,2,3  see documentation table 46-47 and figure 28; if this function is called, the current gyroBW should not change (see the function definition comments*)

	/*****************************************************************************************
	Since iGlass_imu reads give raw data, the settings below will not affect the data read. Look at iGlass_imu.h for notes on how to calibrate the data.
	******************************************************************************************/

	// /*Output Unit*/
	// IMU.accelUnit = GRAVITY;            // 	GRAVITY  METERPERSECOND2; default = GRAVITY
	// IMU.gyroUnit = DEGREEPERSECOND;     //   DEGREEPERSECOND  RADIANSPERSECOND  REVSPERMINUTE  REVSPERSECOND; default = DEGREEPERSECOND  
	// IMU.magnetUnit = MICROTESLA;        //   GAUSS   MICROTESLA   NANOTESLA; default = MICROTESLA

	// /*For an improved accuracy run (Arduino Library) Arduino_LSM9DS1's DIY_Calibration_Accelerometer sketch first. Copy/Replace the lines below by the code output of the program.*/
	// IMU.setAccelOffset(0, 0, 0);  //   uncalibrated
	// IMU.setAccelSlope (1, 1, 1);  //   uncalibrated

	// /*The gyroscope needs to be calibrated. Offset controls drift and Slope scales the measured rotation angle. Copy/Replace the lines below by the output of (Arduino Library) Arduino_LSM9DS1's DIY_Calibration_Gyroscope sketch.*/
	// IMU.setGyroOffset (0, 0, 0);  // = uncalibrated
	// IMU.setGyroSlope  (1, 1, 1);  // = uncalibrated

	// /*For a proper functioning of the magnetometer it needs to be calibrated. (An "in-situ" calibration is advised.) Copy/Replace the lines below by the output of (Arduino Library) Arduino_LSM9DS1's DIY_Calibration_Magnetometer sketch.*/
	// IMU.setMagnetOffset(0,0,0);  //  uncalibrated
	// IMU.setMagnetSlope (1,1,1);  //  uncalibrated

	imu_setup_flag = 1;

    return EXECUTION_SUCCESS;
}

/*
	Function:	Reads raw sensor data to buffer; uses 0,0,0 for each sample's read failure
	Input:		buf - ptr to data buffer
				num_samples - requested number of samples to read 
				sensor - ACC/GYRO/MAG  
	Ret Val: 	number of sensor samples successfully read (samples_read)
				EXECUTION_SUCCESS - not enough sensor samples available yet; buffer values not changed
				EXECUTION_FAILURE - iGlass_imu instance not setup; invalid argument(s); failure to read available data point(s) during execution -> buffer values still changed
*/
int iGlass_imu::read(int16_t * buf, int num_samples, int sensor) {   
	if (imu_setup_flag == 0) {
		#if DEBUG_MIC
            Serial.println("iGlass_imu instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	int samples_read;
	switch(sensor) {
		case ACC:
			samples_read = read_acc(buf, num_samples);
			break;
		case GYRO:
			samples_read = read_gyro(buf, num_samples);
			break;
		case MAG:
			samples_read = read_mag(buf, num_samples);
			break;

		default:
            #if DEBUG_IMU
                Serial.println("Invalid sensor argument!");
            #endif
			samples_read = EXECUTION_FAILURE;
			break;
	}

	return samples_read;
}

/*
	Function:	Reads raw accelerometer data to buffer; helper function to read
	Input:		buf - ptr to data buffer
				num_samples - requested number of samples to read; can read up to IMU_FIFO_SAMPLE_CAPACITY samples
	Ret Val: 	number of accelerometer samples successfully read (num_samples_to_read)
				EXECUTION_SUCCESS - not enough accelerometer samples available yet; buffer values not changed
				EXECUTION_FAILURE - invalid argument(s); failure to read available data point(s) during execution -> buffer values still changed
*/
int iGlass_imu::read_acc(int16_t * buf, int num_samples) {
	if (buf == nullptr) {
		#if DEBUG_IMU
			Serial.println("Invalid ACC buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (num_samples <= 0) {
		#if DEBUG_IMU
			Serial.println("Invalid ACC num_samples argument (<= 0)!");
		#endif
		return EXECUTION_FAILURE;
	}

    int num_samples_to_read = min(num_samples, IMU_FIFO_SAMPLE_CAPACITY);
    int samples_read = 0;

    if (IMU.accelAvailable() >= num_samples_to_read) {
        for (int i = 0; i < num_samples_to_read; i++) {
			samples_read += IMU.readRawAccelInt16(buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
        }

		if (samples_read != num_samples_to_read) {
			#if DEBUG_IMU
				Serial.println("execution failure in reading available ACC sample(s)");
			#endif
			return EXECUTION_FAILURE;
		}
		return num_samples_to_read;
    }

	#if DEBUG_IMU
		Serial.println("Not enough ACC samples yet!");
	#endif
	return EXECUTION_SUCCESS;
}

/*
	Function:	Reads raw gyroscope data to buffer; helper function to read
	Input:		buf - ptr to data buffer
				num_samples - requested number of samples to read; can read up to IMU_FIFO_SAMPLE_CAPACITY samples
	Ret Val: 	number of gyroscope samples successfully read (num_samples_to_read)
				EXECUTION_SUCCESS - not enough gyroscope samples available yet; buffer values not changed
				EXECUTION_FAILURE - invalid argument(s); failure to read available data point(s) during execution -> buffer values still changed
*/
int iGlass_imu::read_gyro(int16_t * buf, int num_samples) {
	if (buf == nullptr) {
		#if DEBUG_IMU
			Serial.println("Invalid GYRO buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (num_samples <= 0) {
		#if DEBUG_IMU
			Serial.println("Invalid GYRO num_samples argument (<= 0)!");
		#endif
		return EXECUTION_FAILURE;
	}

    int num_samples_to_read = min(num_samples, IMU_FIFO_SAMPLE_CAPACITY);
    int samples_read = 0;

    if (IMU.gyroAvailable() >= num_samples_to_read) {
        for (int i = 0; i < num_samples_to_read; i++) {
			samples_read += IMU.readRawGyroInt16(buf[i * 3], buf[i * 3 + 1], buf[i * 3 + 2]);
        }

		if (samples_read != num_samples_to_read) {
			#if DEBUG_IMU
				Serial.println("execution failure in reading available GYRO sample(s)");
			#endif
			return EXECUTION_FAILURE;
		}
		return num_samples_to_read;
    }

	#if DEBUG_IMU
		Serial.println("Not enough GYRO samples yet!");
	#endif
	return EXECUTION_SUCCESS;
}

/*
	Function:	Reads raw magnetometer data to buffer; helper function to read
	Input:		buf - ptr to data buffer 
				num_samples - requested number of samples to read, can only read up to 1 sample
	Ret Val: 	number of magnetometer samples successfully read (1)
				EXECUTION_SUCCESS - not enough magnetometer sample(s) available yet; buffer values not changed
				EXECUTION_FAILURE - invalid argument(s); failure to read available data point during execution -> buffer values still changed
*/
int iGlass_imu::read_mag(int16_t * buf, int num_samples) {
	if (buf == nullptr) {
		#if DEBUG_IMU
			Serial.println("Invalid MAG buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (num_samples <= 0) {
		#if DEBUG_IMU
			Serial.println("Invalid MAG num_samples argument (<= 0)!");
		#endif
		return EXECUTION_FAILURE;
	}

	if (IMU.magnetAvailable()) {
		if (!IMU.readRawMagnetInt16(buf[0], buf[1], buf[2])) {
			#if DEBUG_IMU
				Serial.println("execution failure in reading available MAG sample");
			#endif
			return EXECUTION_FAILURE;
		}
		return 1;
	}

	#if DEBUG_IMU
		Serial.println("No MAG sample yet!");
	#endif
	return EXECUTION_SUCCESS;
}

/*
	Function:	Reads raw accelerometer and gyroscope data to their respective buffers;  uses 0,0,0 for each sample read failure
	Input:		acc_buf - ptr to accelerometer data buffer
				gyro_buf - ptr to gyroscope data buffer
				num_samples - requested number of samples to read; can read up to IMU_FIFO_SAMPLE_CAPACITY samples
	Ret Val: 	number of accelerometer-gyroscope samples successfully read (num_samples_to_read)
				EXECUTION_SUCCESS - not enough accelerometer-gyroscope samples available yet; buffer values not changed
				EXECUTION_FAILURE - iGlass_imu instance not setup; invalid argument(s); failure to read available data point(s) during execution -> buffer values still changed
*/
int iGlass_imu::read_acc_gyro(int16_t * acc_buf, int16_t * gyro_buf, int num_samples) {
	if (imu_setup_flag == 0) {
		#if DEBUG_IMU
            Serial.println("iGlass_imu instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	if (acc_buf == nullptr) {
		#if DEBUG_IMU
			Serial.println("Invalid acc_buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (gyro_buf == nullptr) {
		#if DEBUG_IMU
			Serial.println("Invalid gyro_buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (num_samples <= 0) {
		#if DEBUG_IMU
			Serial.println("Invalid ACC GYRO num_samples argument (<= 0)!");
		#endif
		return EXECUTION_FAILURE;
	}
	
    int num_samples_to_read = min(num_samples, IMU_FIFO_SAMPLE_CAPACITY);

    if (IMU.accelAvailable() >= num_samples_to_read) {		// In operation mode Gyro+Accel, accelerometer and gyroscope share the same ODR
		if (IMU.readMultiRawGyroAccelInt16(acc_buf, gyro_buf, num_samples_to_read) != num_samples_to_read) {
			#if DEBUG_IMU
				Serial.println("execution failure in reading available ACC GYRO sample(s)");
			#endif
			return EXECUTION_FAILURE;
		}
		return num_samples_to_read; 
    }

    #if DEBUG_IMU
		Serial.println("Not enough ACC GYRO samples yet!");
	#endif
	return EXECUTION_SUCCESS;
}

/*
	Function:	Nothing, placeholder
	Input:		None
	Ret Val: 	EXECUTION_SUCCESS
				EXECUTION_FAILURE - iGlass_imu instance not setup
*/
int iGlass_imu::write(){
	if (imu_setup_flag == 0) {
		#if DEBUG_IMU
            Serial.println("iGlass_imu instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	return EXECUTION_SUCCESS;
}

/*
	Function:	Ends the current instance of iGlass_imu
	Input:		None
	Ret Val: 	None
*/
void iGlass_imu::end() {
	if (imu_setup_flag == 0)
		return;

	IMU.end();	//..........in the future, edit the source code to return a value

	imu_setup_flag = 0;
}

/*
	Function: 	Prints one raw data point from the accelerometer, the gyroscope, and the magentometer, if available
	Input: 		None
	Ret Val:	EXECUTION_SUCCESS
				EXECUTION_FAILURE - iGlass_imu instance not setup; unable to read available data
*/
int iGlass_imu::print() {
	if (imu_setup_flag == 0) {
		#if DEBUG_IMU
            Serial.println("iGlass_imu instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	#if DEBUG_IMU
		int16_t ax, ay, az, wx, wy, wz, mx, my, mz;
		bool a_read = false, g_read = false, m_read = false;
        bool execution_failure = false;

        Serial.println("--------------------------------");

		if (IMU.accelAvailable()) {
            if (IMU.readRawAccelInt16(ax, ay, az)) {
                a_read = true;
            } else {
                execution_failure = true;
                Serial.println("execution failure in reading available ACC sample");
            }
		}

		if (IMU.gyroAvailable()) {
			if (IMU.readRawGyroInt16(wx, wy, wz)) {
                g_read = true;
            } else {
                execution_failure = true;
                Serial.println("execution failure in reading available GYRO sample");
            }
		}

		if (IMU.magnetAvailable()) {
			if (IMU.readRawMagnetInt16(mx, my, mz)) {
                m_read = true;
            } else {
                execution_failure = true;
                Serial.println("execution failure in reading available MAG sample");
            }
		}

		if (a_read) {
			Serial.print("acc: ");
			Serial.print(ax);
			Serial.print(" ");
			Serial.print(ay);
			Serial.print(" ");
			Serial.println(az);
		}
		if (g_read) {
			Serial.print("ang: ");
			Serial.print(wx);
			Serial.print(" ");
			Serial.print(wy);
			Serial.print(" ");
			Serial.println(wz);
		}
		if (m_read) {
			Serial.print("mag: ");
			Serial.print(mx);
			Serial.print(" ");
			Serial.print(my);
			Serial.print(" ");
			Serial.println(mz);
		}

        if (execution_failure) {
            return EXECUTION_FAILURE;
        }
	#endif

    return EXECUTION_SUCCESS;
}
