#include <iGlass.h>

void setup() {

	/*Unittest init*/
	Serial.println("Before initialization...");
	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}
	Serial.println("Initialized!");
    

    /*MIC unittest - MONO*/
    Serial.println("Testing MONO MIC");
    iGlass_mic mic_i = iGlass_mic(PDM_BUF_SAMPLE_CAPACITY, 1, LOW_FREQ, INT_MIC); // using Arduino Nano 33 Ble Sense Mic


    Serial.println("Before MIC init...");
    if (mic_i.init() == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("MIC initialized!");


    Serial.println("Before MIC read...");

    int16_t * mic_data_buf = new int16_t[PDM_BUF_SAMPLE_CAPACITY];

    int read_samples = 0;
    while (read_samples == 0) {
        read_samples = mic_i.read(mic_data_buf, PDM_BUF_SAMPLE_CAPACITY);
    }
    if (read_samples == EXECUTION_FAILURE) {
        while(1);
    }

    for (int i = 0; i < read_samples; i++) { 
		Serial.print(mic_data_buf[i]);
		Serial.print(", ");
    }
    Serial.println();

    Serial.println("After MIC read...");
    

    Serial.println("Before MIC write...");
    if (mic_i.write() == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("After MIC write...");


    Serial.println("Before MIC print...");
    int printed_samples = 0;
    while (printed_samples == 0) {
        printed_samples = mic_i.print();
    }
    if (printed_samples == EXECUTION_FAILURE) {     
        while(1);
    }
    Serial.println("After MIC print...");


    mic_i.end();
    delete[] mic_data_buf;
    mic_data_buf = nullptr;


	/*MIC unittest -STEREO*/
    Serial.println("Testing STEREO MIC");
    mic_i = iGlass_mic(PDM_BUF_SAMPLE_CAPACITY, 2, HIGH_FREQ, EXT_MIC);


    Serial.println("Before MIC init...");
    if (mic_i.init() == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("MIC initialized!");


    Serial.println("Before MIC read...");

    mic_data_buf = new int16_t[PDM_BUF_SAMPLE_CAPACITY];

    read_samples = 0;
    while (read_samples == 0) {
        read_samples = mic_i.read(mic_data_buf, PDM_BUF_SAMPLE_CAPACITY);
    }
    if (read_samples == EXECUTION_FAILURE) {
        while(1);
    }

    for (int i = 0; i < read_samples; i++) {
        Serial.print(mic_data_buf[i]);
        Serial.print(" ");
        Serial.print(mic_data_buf[++i]);
        Serial.print(", ");
    }
    Serial.println();

    Serial.println("After MIC read...");


    Serial.println("Before MIC write...");
    if (mic_i.write() == EXECUTION_FAILURE) {
        while(1);
    }
    Serial.println("After MIC write...");


    Serial.println("Before MIC print...");
    printed_samples = 0;
    while (printed_samples == 0) {
        printed_samples = mic_i.print();
    }
    if (printed_samples == EXECUTION_FAILURE) {     
        while(1);
    }
    Serial.println("After MIC print...");
	

    mic_i.end();
    delete[] mic_data_buf;
    mic_data_buf = nullptr;


    Serial.println("End of MIC unittest!");
}


void loop() {
}


