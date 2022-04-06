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

    iGlass_mic mic_i = iGlass_mic(PDM_BUF_SIZE, 1, LOW_FREQ, EXT_MIC); // not sure if ext_mic or int_mic...........


    Serial.println("Before MIC init...");

    mic_i.init();

    Serial.println("MIC initialized!");


    Serial.println("Before MIC print...");

    mic_i.print();

    Serial.println("After MIC print...");


    mic_i.end();


	/*MIC unittest -STEREO*/
    Serial.println("Testing STEREO MIC");

	mic_i = iGlass_mic();

    Serial.println("Before MIC init...");

    mic_i.init();

    Serial.println("MIC initialized!");


    Serial.println("Before MIC read...");

    int16_t mic_data_buf[PDM_BUF_SIZE];

    while(mic_i.num_samples_read() == 0);

    bool mic_read = (mic_i.read(mic_data_buf, mic_i.num_samples_read()) != 0);

    Serial.println("After MIC read...");


    Serial.println("Before MIC print...");

    mic_i.print();

    Serial.println("After MIC print...");


    mic_i.end();

    if (!mic_read) Serial.println("MIC read failed...");

    if (mic_read) {
        Serial.println("MIC test succeeded!");
    } else {
        Serial.println("MIC test failed!");
    }
}


void loop() {
}


