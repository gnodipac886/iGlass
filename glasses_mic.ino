#include <PDM.h>
//#include <SdFat.h>
#include <hal/nrf_pdm.h>

void setup_mic() {
	#if DEBUG
	if (!Serial) {
		Serial.begin(115200);
		while (!Serial);
	}
	#endif

	// Configure the data receive callback
	PDM.onReceive(onPDMdata);
	PDM.setBufferSize(PDM_BUF_SIZE * pdm_sample_size);

	mic_print_arr = 0;

	// Optionally set the gain
	// Defaults to 20 on the BLE Sense and -10 on the Portenta Vision Shield
	// PDM.setGain(30);

	if(!mic_setup_flag){
		if (!PDM.begin(channels, frequency)) {
			#if DEBUG
			Serial.println("Failed to start PDM!");
			#endif
			while (1);
		}
	}

	mic_setup_flag = 1;
}

void end_mic() {
	PDM.end();
	mic_setup_flag = 0;
}

void save_mic_data() {
  // Wait for samples to be read
	if (samplesRead) {

	//    int start = micros();
		int_mic = 0;
	//    NVIC_DisableIRQ(PDM_IRQn);
		mic_file.write((byte*)mic_sampleBuffer, (size_t)(PDM_BUF_SIZE * pdm_sample_size));
	//    NVIC_EnableIRQ(PDM_IRQn);
	//    Serial.println("Bytes: " + String(PDM_BUF_SIZE * pdm_sample_size));
		if(int_mic){
			#if DEBUG
			Serial.println("mic int");
			#endif
		}
		if (++mic_flush_counter == 10) {
			mic_flush_counter = 0;
			mic_file.flush();
			#if DEBUG
			Serial.println("flushed!");
			#endif
		}
		//		Serial.println("B/s: " + String(float(PDM_BUF_SIZE) / (float(micros() - start) / 1000000.0)));


		// Clear the read count
		samplesRead = 0;
	}
}

/**
	 Callback function to process the data from the PDM microphone.
	 NOTE: This callback is executed as part of an ISR.
	 Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
	// Query the number of available bytes
	int bytesAvailable = PDM.available();

	// Read into the sample buffer
	PDM.read(mic_sampleBuffer, bytesAvailable);

	// 16-bit, 2 bytes per sample
	samplesRead = bytesAvailable / 2;

	int_mic = 1;
	int_imu = 1;
}
