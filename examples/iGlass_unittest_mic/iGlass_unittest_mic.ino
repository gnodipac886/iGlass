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

    iGlass_mic mic_i = iGlass_mic(PDM_BUF_SIZE, 1, LOW_FREQ, INT_MIC); // using Arduino Nano 33 Ble Sense Mic

    Serial.println("Before MIC init...");
    mic_i.init();
    Serial.println("MIC initialized!");


    Serial.println("Before MIC read...");

    int16_t * mic_data_buf = new int16_t[(int)(PDM_BUF_SIZE/sizeof(int16_t))];

    int read_samples = 0;

    do {
      	read_samples = mic_i.read(mic_data_buf, mic_i.num_samples_read());
    } while(read_samples == 0);

    for (int i = 0; i < read_samples; i++) { 
		Serial.print(mic_data_buf[i]);
		Serial.print(", ");
    }
    Serial.println();

    Serial.println("After MIC read...");


    Serial.println("Before MIC print...");
    while (mic_i.num_samples_read() == 0);
    mic_i.print();
    Serial.println("After MIC print...");


    mic_i.end();
    delete[] mic_data_buf;
    mic_data_buf = nullptr;


	/*MIC unittest -STEREO*/
    Serial.println("Testing STEREO MIC");

    mic_i = iGlass_mic(PDM_BUF_SIZE, 2, HIGH_FREQ, EXT_MIC);

    Serial.println("Before MIC init...");
    mic_i.init();
    Serial.println("MIC initialized!");


    Serial.println("Before MIC read...");

    mic_data_buf = new int16_t[(int)(PDM_BUF_SIZE/sizeof(int16_t))];
    read_samples = 0;

    do {
      	read_samples = mic_i.read(mic_data_buf, mic_i.num_samples_read());
    } while(read_samples == 0);

    for (int i = 0; i < read_samples; i++) {
        Serial.print(mic_data_buf[i]);
        Serial.print(" ");
        Serial.print(mic_data_buf[++i]);
        Serial.print(", ");
    }
    Serial.println();

    Serial.println("After MIC read...");


    Serial.println("Before MIC print...");
    while (mic_i.num_samples_read() == 0);
    mic_i.print();
    Serial.println("After MIC print...");
	

    mic_i.end();
    delete[] mic_data_buf;
    mic_data_buf = nullptr;


    Serial.println("End of MIC unittest!");
}


void loop() {
}


