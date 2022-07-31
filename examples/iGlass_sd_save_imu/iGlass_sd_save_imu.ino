#include <iGlass.h>

#define IMU_SENSOR_SAMPLE_BYTE_SIZE                NUM_AXES * sizeof(int16_t)                                                                       // 3*2 = 6
#define IMU_FIFO_BUF_BYTE_SIZE                     IMU_FIFO_SIZE * IMU_SENSOR_SAMPLE_BYTE_SIZE                                                      // 32 * 6 = 192
#define ACC_GYRO_FIFO_READS_PER_SD_WRITE           (int)(SD_DATA_BUFFER_BYTE_LIMIT / IMU_FIFO_BUF_BYTE_SIZE)                                        // (int)(512/192) = 2

#define IMU_ACC_GYRO_BUF_BYTE_SIZE                  ACC_GYRO_FIFO_READS_PER_SD_WRITE * IMU_FIFO_BUF_BYTE_SIZE                                       // 2 * 192 = 384
#define IMU_MAG_BUF_BYTE_SIZE                       ((int)(SD_DATA_BUFFER_BYTE_LIMIT/IMU_SENSOR_SAMPLE_BYTE_SIZE))*IMU_SENSOR_SAMPLE_BYTE_SIZE      // ((int)(512/6))*6 = 510

/*
ACC GYRO sample rate = 476 Hz => (476 samples / 1 sec) * (6 bytes / 1 sample) = 2856 bytes/sec => (2856 bytes / 1 sec) * (1 full FIFO Buffer / 192 bytes) = 14.875 full FIFO Buffers/sec
MAG sample rate = 400 Hz => (400 samples / 1 sec) * (6 bytes / 1 sample) = 2400 bytes/sec 

SD Spi Clock = 50 Mhz
SD Data Buffer Byte Limit = 512 bytes = 4096 bits

Therefore, SD writes should be faster than IMU reads

SD_DATA_BUFFER_BYTE_LIMIT = 512
IMU_FIFO_BUF_BYTE_SIZE = 192
I chose to do 2 IMU full FIFO reads per more frequent SD writes of 384 bytes, instead of 2-3 IMU FIFO reads per less frequent SD writes of 510 bytes
However, I am not sure which way is better for not losing (older) ACC/GYRO FIFO samples or for getting more consistent MAG reads (as explained below), and which way is better for reducing power consumption. (Needs more research and math.)
*/

iGlass_sd sd_i = iGlass_sd();
iGlass_imu imu_i = iGlass_imu();

int imu_acc_file_idx, imu_gyro_file_idx, imu_mag_file_idx;
byte * acc_buf, *gyro_buf, *mag_buf;
int acc_fifo_reads, gyro_fifo_reads;
int acc_buf_num_data_int16s, gyro_buf_num_data_int16s, mag_buf_num_data_int16s;

void setup() {

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	sd_i.init();

	bool success = true;
	if (!sd_i.isSetup()) {
        Serial.println("init() did not successfully setup SD");
        success = false;
	}

    if (!sd_i.available()) {
        Serial.println("No SD card detected");
        success = false;  
    }
	if (!success) {
		sd_i.end();
		while(1);
	}

    // adding accelerometer data file
	imu_acc_file_idx = sd_i.addNewFile("imu_acc_file");
	if (imu_acc_file_idx == -1) {
        Serial.println("Unable to add imu acc file");
        sd_i.end();
        while(1);
	}

    // adding gyroscope data file
    imu_gyro_file_idx = sd_i.addNewFile("imu_gyro_file");
	if (imu_gyro_file_idx == -1) {
        Serial.println("Unable to add imu gyro file");
        sd_i.end();
        while(1);
	}

    // adding magnetometer data file
    imu_mag_file_idx = sd_i.addNewFile("imu_mag_file");
	if (imu_mag_file_idx == -1) {
        Serial.println("Unable to add imu mag file");
        sd_i.end();
        while(1);
	}

    acc_buf = (byte*)malloc(IMU_ACC_GYRO_BUF_BYTE_SIZE);
	gyro_buf = (byte*)malloc(IMU_ACC_GYRO_BUF_BYTE_SIZE);
	mag_buf = (byte*)malloc(IMU_MAG_BUF_BYTE_SIZE); 

    memset(acc_buf, 0, sizeof(acc_buf));
    memset(gyro_buf, 0, sizeof(gyro_buf));
    memset(mag_buf, 0, sizeof(mag_buf));

    acc_fifo_reads = 0;
    gyro_fifo_reads = 0;

    acc_buf_num_data_int16s = 0;
    gyro_buf_num_data_int16s = 0;
    mag_buf_num_data_int16s = 0;


    imu_i.init();
}


void loop() {

    /*Updating Accelerometer Data*/
    int num_acc_read_int16s = imu_i.read((int16_t *)acc_buf + acc_buf_num_data_int16s, NUM_AXES * IMU_FIFO_SIZE, ACC);
    if (num_acc_read_int16s) {
        acc_fifo_reads++;
        acc_buf_num_data_int16s += num_acc_read_int16s;

        if (acc_fifo_reads == ACC_GYRO_FIFO_READS_PER_SD_WRITE) {
            if (!sd_i.write(imu_acc_file_idx, acc_buf, acc_buf_num_data_int16s*sizeof(int16_t))) {
                Serial.println("SD ACC write failed!");
            }
            memset(acc_buf, 0, acc_buf_num_data_int16s*sizeof(int16_t));	//not really needed if we keep track of buf_num_data_int16s...............
            acc_fifo_reads = 0;
            acc_buf_num_data_int16s = 0;
        }
    } 


    /*Updating Gyroscope Data*/
    int num_gyro_read_int16s = imu_i.read((int16_t *)gyro_buf + gyro_buf_num_data_int16s, NUM_AXES * IMU_FIFO_SIZE, GYRO);
    if (num_gyro_read_int16s) {
        gyro_fifo_reads++;
        gyro_buf_num_data_int16s += num_gyro_read_int16s;

        if (gyro_fifo_reads == ACC_GYRO_FIFO_READS_PER_SD_WRITE) {
            if (!sd_i.write(imu_gyro_file_idx, gyro_buf, gyro_buf_num_data_int16s*sizeof(int16_t))) {
                Serial.println("SD ACC write failed!");
            }
            memset(gyro_buf, 0, gyro_buf_num_data_int16s*sizeof(int16_t));	//not really needed if we keep track of buf_num_data_int16s...............
            gyro_fifo_reads = 0;
            gyro_buf_num_data_int16s = 0;
        }
    } 


    /*Updating Magnetometer Data*/
    mag_buf_num_data_int16s += imu_i.read((int16_t *)mag_buf + mag_buf_num_data_int16s, NUM_AXES, MAG);
    if (mag_buf_num_data_int16s*sizeof(int16_t) ==  IMU_MAG_BUF_BYTE_SIZE) {
        if (!sd_i.write(imu_mag_file_idx, mag_buf, IMU_MAG_BUF_BYTE_SIZE)) {
            Serial.println("SD ACC write failed!");
        }
        memset(mag_buf, 0, IMU_MAG_BUF_BYTE_SIZE);
        mag_buf_num_data_int16s = 0;
    }

    /*
    We can only read one magnetometer sample per loop
    + The number of imu reads and sd writes vary every loop
    => magnetometer samples will not be read with equal time intervals

    Ideally, we want to be able to perform a full ACC/GYRO FIFO Buffer read every 67.227 miliseconds (since 1000 miliseconds per 14.875 full FIFO Buffers), to not lose old FIFO samples
    => Ideally, we want to be able to perform an SD ACC/GYRO write every ((67.227 miliseconds per IMU ACC/GYRO read) * (2 IMU ACC/GYRO reads per SD ACC/GYRO write) =) 134.454 miliseconds

    Considering how much time the imu reads and sd writes take, we might be able to reduce power consumption as well as the magnetometer read time interval variance by adding a time delay at every loop
    */
   // delay(...);
}

