#include <PDM.h>
#include "SdFat.h"
#include "sdios.h"
#include <Time.h>

#define PDM_BUF_SIZE 1024 * 16
#define IMU_BUF_SIZE 1024 * 9

#define DEBUG 0

#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASSs


// IR Commands
#define GES_WRITE_SD 		    2     	//F
#define GES_STOP_WRITE_SD 		1     	//B
#define GES_SLEEP 			    3     	//U
#define GES_WAKE 			    4     	//D
#define GES_BLE_START			12		//FB
#define GES_BLE_STOP			21		//BF

// Others
#define USING_SD				0
#define USING_BLE				1

/****************Global Variables***************/

// sd card variables
	int chip_select = D10;
	int chip_detect = D9;

	SdExFat SD;
	ExFile mic_file;
	ExFile imu_file;

	String mic_fname = "mic_data";
	String imu_fname = "imu_data";

	volatile int card_present = 0;


// pdm microphone variables
	PDMClass PDM(D8, D7, D6);
	PDMClass PDM2(PIN_PDM_DIN, PIN_PDM_CLK, PIN_PDM_PWR);

	int mic_prev_time, mic_flush_counter = 0;

	// default number of output channels
	static char channels = 2;

	// default PCM output frequency, 41667
	static int frequency = 41667;

	// Buffer to read samples into, each sample is 16-bits
	short mic_sampleBuffer[PDM_BUF_SIZE];
	int pdm_sample_size = sizeof(short);// mic_sampleBuffer[0]);
	volatile int mic_print_arr;

	// Number of audio samples read
	volatile int samplesRead;


// imu variables
	float ax, ay, az, wx, wy, wz, mx, my, mz;
	float imu_sampleBuffer[IMU_BUF_SIZE];
	int imu_buf_idx = 0;


// IR variables
	int IR_command_given = 0;
	int IR_command = -1;

// others
	volatile int setup_complete, int_mic, int_imu = 0;
  unsigned long rgb_timer;
 
//Flags
	char SD_WRITE_FLAG = 0;
	int mic_setup_flag, imu_setup_flag, sd_setup_flag, ble_setup_flag = 0;
/***********************************************/

void setup() {
	#if DEBUG
	if(!Serial){
		Serial.begin(115200);
		// while (!Serial);
	}
	#endif

  pinMode(LED_BUILTIN, OUTPUT);
	setup_rgb();
  setup_ir();
	// setup_sd_card();
	attachInterrupt(digitalPinToInterrupt(chip_detect), card_detect, CHANGE);
	setup_complete = 1;
	ble_start_handler();
}

void loop() {
  //IR_gesture_check();........................................................................................
	// if(card_present && setup_complete && IR_command_given){    
	// 	//update_IMU();
	// 	IR_command_given = 0;
	// 	//Process IR_commands
	// 	switch(IR_command) {
	// 		case GES_WRITE_SD:
	// 			write_sd_handler(); 
	// 			break;
			
	// 		case GES_STOP_WRITE_SD:
	// 			stop_write_sd_handler();
	// 			break;

	// 		case GES_WAKE:
	// 			wake_from_sleep_handler();
	// 			break;

	// 		case GES_SLEEP:
	// 			go_to_sleep_handler();
	// 			break;

	// 		case GES_BLE_START:
	// 			ble_start_handler();					//flag + update ble function
	// 			break;

	// 		case GES_BLE_STOP:
	// 			ble_end_handler();
	// 			break;

	// 		default:
	// 			break;
    // 	}
    //     IR_command = -1;
	// } else if(card_present && !setup_complete){
	// 	imu_file = SD.open(imu_fname, O_WRITE | O_CREAT);
	// 	setup_complete = 1;
    // 	delay(100);
	// } else {
	// 	//Serial.println("SD card not present");
	// 	//delay(250);
	// }

	// if (SD_WRITE_FLAG == 1) {
	// 	if(mic_file) {
	// 		save_mic_data();
	// 	} else {
	// 		#if DEBUG
	// 		Serial.println("error opening mic_data file");
	// 		#endif
	// 	}
	// 	if(imu_file) {
	// 		update_IMU(imu_sampleBuffer);
	// 	} else {
	// 		#if DEBUG
	// 		Serial.println("error opening imu_data file");
	// 		#endif
	// 	}
	// }

	if (ble_setup_flag == 1) {
    // Serial.println("updating ble!!");
		update_ble();
	}
	//rgb
	// if (SD_WRITE_FLAG == 1 || ble_setup_flag == 1) {
	// 	if (millis()- rgb_timer >= 40) {
	// 	update_rgb();
	// 	rgb_timer = millis();
	// 	}
	// }
}
