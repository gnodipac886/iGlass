#include <iGlass.h>

#define IMU_FIFO_BUF_BYTE_SIZE                      IMU_FIFO_SAMPLE_CAPACITY * IMU_SAMPLE_BYTE_SIZE     //192                                                // 32 * 6 = 192
#define ACC_GYRO_FIFO_READS_PER_SD_WRITE            (int)ceil((SD_DATA_BUFFER_BYTE_LIMIT / IMU_FIFO_BUF_BYTE_SIZE)) //3                                       // (int)(512/192) = 2

#define IMU_SAMPLES_PER_SD_BUFFER                   (int)(SD_DATA_BUFFER_BYTE_LIMIT/IMU_SAMPLE_BYTE_SIZE)   //(int)(512/6) = 85
#define IMU_BYTES_PER_SD_BUFFER                     IMU_SAMPLES_PER_SD_BUFFER*IMU_SAMPLE_BYTE_SIZE     // 85*6 = 510
#define IMU_MAG_BUF_BYTE_SIZE                       IMU_BYTES_PER_SD_BUFFER

#define ACC_GYRO_FIFO_READ_SAMPLE_SIZE              ((int)(IMU_SAMPLES_PER_SD_BUFFER/ACC_GYRO_FIFO_READS_PER_SD_WRITE))    //(int)(85/3) = 28
#define ACC_GYRO_FIFO_READ_BYTE_SIZE                ACC_GYRO_FIFO_READ_SAMPLE_SIZE * IMU_SAMPLE_BYTE_SIZE   //28*6 = 168 bytes
#define IMU_ACC_GYRO_BUF_BYTE_SIZE                  ACC_GYRO_FIFO_READ_BYTE_SIZE * ACC_GYRO_FIFO_READS_PER_SD_WRITE //168*3 = 504 bytes



/*
ACC GYRO sample rate = 476 Hz => (476 samples / 1 sec) * (6 bytes / 1 sample) = 2856 bytes/sec => (2856 bytes / 1 sec) * (1 full FIFO Buffer / 192 bytes) = 14.875 full FIFO Buffers/sec
MAG sample rate = 400 Hz => (400 samples / 1 sec) * (6 bytes / 1 sample) = 2400 bytes/sec 

SD Spi Clock = 50 Mhz
SD Data Buffer Byte Limit = 512 bytes = 4096 bits

Therefore, SD writes should be faster than (fastest) IMU reads

SD_DATA_BUFFER_BYTE_LIMIT = 512
IMU_FIFO_BUF_BYTE_SIZE = 192
I chose to do 3 IMU FIFO reads per less frequent SD writes of 510 bytes, instead of 2 IMU full FIFO reads per more frequent SD writes of 384 bytes
*Note: This is because IMU full FIFO reads might make us lose (older) FIFO samples, because the IMU does not interrupt when the FIFO is full (LSM9DS1's interrupt pins are not soldered)
=> ACC GYRO FIFO Buffer Read Rate = (2856 bytes / 1 sec) * (1 FIFO Buffer Read / 168 bytes) = 17 FIFO Buffer Reads/sec
However, I am not sure which way is better for getting more consistent MAG reads (as explained below), and for reducing power consumption. (Needs more research and math.)
*/

iGlass_sd sd_i = iGlass_sd();
iGlass_imu imu_i = iGlass_imu();

int imu_acc_file_idx, imu_gyro_file_idx, imu_mag_file_idx;
byte * acc_buf, *gyro_buf, *mag_buf;
int acc_fifo_reads, gyro_fifo_reads;
int acc_buf_num_data_bytes, gyro_buf_num_data_bytes, mag_buf_num_data_bytes;

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

    acc_buf_num_data_bytes = 0;
    gyro_buf_num_data_bytes = 0;
    mag_buf_num_data_bytes = 0;


    imu_i.init();
}


void loop() {

    /*Updating Accelerometer Data*/
    int num_acc_read_bytes = imu_i.read((int16_t *)(acc_buf + acc_buf_num_data_bytes), ACC_GYRO_FIFO_READ_SAMPLE_SIZE, ACC)*IMU_SAMPLE_BYTE_SIZE;
    if (num_acc_read_bytes) {
        acc_fifo_reads++;
        acc_buf_num_data_bytes += num_acc_read_bytes;

        if (acc_fifo_reads == ACC_GYRO_FIFO_READS_PER_SD_WRITE) {
            if (!sd_i.write(imu_acc_file_idx, acc_buf, acc_buf_num_data_bytes)) {
                Serial.println("SD ACC write failed!");
            }
            memset(acc_buf, 0, acc_buf_num_data_bytes); //not really needed if we keep track of buf_num_data_bytes...............
            acc_fifo_reads = 0;
            acc_buf_num_data_bytes = 0;
        }
    }


    /*Updating Gyroscope Data*/
    int num_gyro_read_bytes = imu_i.read((int16_t *)(gyro_buf + gyro_buf_num_data_bytes), ACC_GYRO_FIFO_READ_SAMPLE_SIZE, GYRO)*IMU_SAMPLE_BYTE_SIZE;
    if (num_gyro_read_bytes) {
        gyro_fifo_reads++;
        gyro_buf_num_data_bytes += num_gyro_read_bytes;

        if (gyro_fifo_reads == ACC_GYRO_FIFO_READS_PER_SD_WRITE) {
            if (!sd_i.write(imu_gyro_file_idx, gyro_buf, gyro_buf_num_data_bytes)) {
                Serial.println("SD GYRO write failed!");
            }
            memset(gyro_buf, 0, gyro_buf_num_data_bytes); //not really needed if we keep track of buf_num_data_bytes...............
            gyro_fifo_reads = 0;
            gyro_buf_num_data_bytes = 0;
        }
    }
    

    /*Updating Magnetometer Data*/
    mag_buf_num_data_bytes += imu_i.read((int16_t *)(mag_buf + mag_buf_num_data_bytes), 1, MAG)*IMU_SAMPLE_BYTE_SIZE;
    if (mag_buf_num_data_bytes ==  IMU_MAG_BUF_BYTE_SIZE) {
        if (!sd_i.write(imu_mag_file_idx, mag_buf, IMU_MAG_BUF_BYTE_SIZE)) {
            Serial.println("SD MAG write failed!");
        }
        memset(mag_buf, 0, IMU_MAG_BUF_BYTE_SIZE); //not really needed if we keep track of buf_num_data_bytes...............
        mag_buf_num_data_bytes = 0;
    }

    /*
    We can only read one magnetometer sample per loop
    + The number of imu reads and sd writes vary every loop
    => magnetometer samples will not be read with equal time intervals

    Ideally, we want to be able to perform an ACC/GYRO FIFO Buffer read by every 58.823 miliseconds (since 1000 miliseconds per 17 FIFO Buffer Read), to not lose old FIFO samples
    => Ideally, we want to be able to perform an SD ACC/GYRO write every ((58.823 miliseconds per IMU ACC/GYRO FIFO Read) * (3 IMU ACC/GYRO FIFO Reads per SD ACC/GYRO write) =) 176.470 miliseconds

    Considering how much time the imu reads and sd writes take, we might be able to reduce power consumption as well as the magnetometer read time interval variance by adding a time delay at every loop
    */
   // delay(...);
}





