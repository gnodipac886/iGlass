#include <iGlass.h>

iGlass_sd sd_i = iGlass_sd();
iGlass_mic mic_i = iGlass_mic(PDM_BUF_SIZE, 2, HIGH_FREQ, EXT_MIC);

int mic_file_idx;

byte * mic_buf;

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

	mic_file_idx = sd_i.addNewFile("mic_file");
	if (mic_file_idx == -1) {
        Serial.println("Unable to add mic file");
        sd_i.end();
        while(1);
	}

    mic_buf = (byte*)malloc(PDM_BUF_SIZE);
    memset(mic_buf, 0, sizeof(mic_buf));

    mic_i.init();
}


void loop() {

    // Note: we currently set mic sample rate = 41667 Hz; sd spi clock = 50 Mhz
    int mic_samples_read = mic_i.read((int16_t *)mic_buf, mic_i.num_samples_read());       // *note: num_samples_read() ideally PDM_BUF_SIZE double check //maybe can save power by not reading at every loop???.....maybe add delay(100 or something) at end of loop?............
    int mic_buf_sd_written_idx = 0;

    while (mic_buf_sd_written_idx < mic_samples_read * sizeof(int16_t)) {
        mic_buf_sd_written_idx += sd_i.write(mic_file_idx, mic_buf + mic_buf_sd_written_idx, min(SD_DATA_BUFFER_BYTE_LIMIT, mic_samples_read*sizeof(int16_t) - mic_buf_sd_written_idx));
    }
}



