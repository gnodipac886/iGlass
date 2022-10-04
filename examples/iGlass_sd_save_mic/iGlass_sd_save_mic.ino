#include <iGlass.h>

iGlass_sd sd_i = iGlass_sd();
iGlass_mic mic_i = iGlass_mic(PDM_BUF_SAMPLE_CAPACITY, 2, HIGH_FREQ, EXT_MIC);

int mic_file_idx;

byte * mic_buf;

void setup() {

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	if (sd_i.init() == EXECUTION_FAILURE) {
        while(1);
    }

	mic_file_idx = sd_i.addNewFile("mic_file");
	if (mic_file_idx == EXECUTION_FAILURE) {
        Serial.println("Unable to add mic file");
        sd_i.end();
        while(1);
	}

    mic_buf = (byte*)malloc(PDM_BUF_SAMPLE_CAPACITY * MIC_SAMPLE_BYTE_SIZE);
    memset(mic_buf, 0, sizeof(mic_buf));

    if (mic_i.init() == EXECUTION_FAILURE) {
        while(1);
    }
}


void loop() {

    // Note: we currently set mic sample rate = 41667 Hz; sd spi clock = 50 Mhz
    int mic_samples_read = mic_i.read((int16_t *)mic_buf, mic_i.samplesAvailable());       // *note: samplesAvailable() ideally PDM_BUF_SAMPLE_CAPACITY double check //maybe can save power by not reading at every loop???.....maybe add delay(100 or something) at end of loop?............
    int mic_buf_sd_written_idx = 0;

    while (mic_buf_sd_written_idx < mic_samples_read * MIC_SAMPLE_BYTE_SIZE) {
        mic_buf_sd_written_idx += sd_i.write(mic_file_idx, mic_buf + mic_buf_sd_written_idx, min(SD_DATA_BUFFER_BYTE_LIMIT, mic_samples_read*sizeof(int16_t) - mic_buf_sd_written_idx));
    }
}



