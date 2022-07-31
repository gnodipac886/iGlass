#include <iGlass.h>

void setup() {

	/*Unittest init*/
	Serial.println("Before initialization...");

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	Serial.println("Initialized!");

    /*RGB unittest*/
    iGlass_rgb rgb_i = iGlass_rgb();

    Serial.println("Before RGB init...");

    rgb_i.init();

    Serial.println("RGB initialized!");

    Serial.println("RGB LED should be black/off!");
    delay(5000);


    Serial.println("Before RGB write...");

    int rgb_write_val[3] = {255,0,0};
    rgb_i.write(rgb_write_val);
    Serial.println("RGB LED should be red!");
    delay(5000);

    rgb_write_val[0] = 0;
    rgb_write_val[1] = 255;
    rgb_write_val[2] = 0;
    rgb_i.write(rgb_write_val);
    Serial.println("RGB LED should be green!");
    delay(5000);

    rgb_i.write(0,0,255);
    Serial.println("RGB LED should be blue!");
    delay(5000);

    rgb_i.write(255,255,255);
    Serial.println("RGB LED should be white!");
    delay(5000);

    Serial.println("After RGB write!");


    Serial.println("Before RGB read...");

    int rgb_read_val[3] = {0,0,0};

    rgb_i.read(rgb_read_val);

    Serial.println("After RGB read!");

    if (rgb_read_val[0] == 255 && rgb_read_val[1] == 255 && rgb_read_val[2] == 255) {
        Serial.println("RGB read succeeded!");
    } else {
        Serial.println("RGB read failed!");
    }


    rgb_i.end();

    Serial.println("End of RGB unittest!");
}


void loop() {
}

