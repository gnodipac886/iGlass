#include <iGlass.h>

void setup() {

	/*Unittest init*/
	Serial.println("Before initialization...");

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	Serial.println("Initialized!");

	/*SD unittest*/
    iGlass_sd sd_i = iGlass_sd();

	Serial.println("Before SD init...");

	bool success = true;
	if (sd_i.init() == EXECUTION_FAILURE) {
        while(1);
    }

    Serial.println("SD initialized!");


    //testing addNewFile
	Serial.println("Before SD addNewFile...");

	int test_file_idx = sd_i.addNewFile("unittest_file");
	if (test_file_idx == EXECUTION_FAILURE) {
        Serial.println("Unable to add new file");
		sd_i.end();
        while(1);
	}
	Serial.println("SD new file added!");


    //testing write...
	Serial.println("Before SD write...");

	byte write_buf[2*SD_DATA_BUFFER_BYTE_LIMIT];
	memset(write_buf, (byte)65, 2*SD_DATA_BUFFER_BYTE_LIMIT);
	success = true;

	memset(write_buf, (byte)66, SD_DATA_BUFFER_BYTE_LIMIT/2);
	if (sd_i.write(test_file_idx, write_buf, SD_DATA_BUFFER_BYTE_LIMIT/2) == EXECUTION_FAILURE)
		success = false;

	memset(write_buf + SD_DATA_BUFFER_BYTE_LIMIT/2, (byte)67, SD_DATA_BUFFER_BYTE_LIMIT/10);
	if (sd_i.write(test_file_idx, write_buf + SD_DATA_BUFFER_BYTE_LIMIT/2, SD_DATA_BUFFER_BYTE_LIMIT/10) == EXECUTION_FAILURE)
		success = false;

	if (sd_i.write(test_file_idx, write_buf + SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10, 2*SD_DATA_BUFFER_BYTE_LIMIT - SD_DATA_BUFFER_BYTE_LIMIT/2 - SD_DATA_BUFFER_BYTE_LIMIT/10) == EXECUTION_FAILURE)
		success = false;

	if (!success) {
		Serial.println("SD write test failed!");
		sd_i.end();
		while(1);
	}

	Serial.println("After SD write");


    //testing read
	Serial.println("Before SD read...");

	success = true;

    byte read_buf[2*SD_DATA_BUFFER_BYTE_LIMIT];

	int num_bytes_read  = sd_i.read(test_file_idx, read_buf, SD_DATA_BUFFER_BYTE_LIMIT);
	if (num_bytes_read != SD_DATA_BUFFER_BYTE_LIMIT) {
        success = false;
		if (num_bytes_read != EXECUTION_FAILURE) {
			Serial.println("First read: " + String(num_bytes_read) + "/" + String(2*SD_DATA_BUFFER_BYTE_LIMIT) + " possible bytes read");
		}
    }
	num_bytes_read = sd_i.read(test_file_idx, read_buf + SD_DATA_BUFFER_BYTE_LIMIT, SD_DATA_BUFFER_BYTE_LIMIT);
    if (num_bytes_read != SD_DATA_BUFFER_BYTE_LIMIT) {
        success = false;
		if (num_bytes_read != EXECUTION_FAILURE) {
			Serial.println("Second read: " + String(num_bytes_read) + "/" + String(2*SD_DATA_BUFFER_BYTE_LIMIT) + " possible bytes read");
		}
    }
	
	if (!success) {
		Serial.println("SD read test failed!");
		sd_i.end();
		while(1);
	}

	Serial.println("SD read is successful!");

	//...testing write
	if (read_buf[SD_DATA_BUFFER_BYTE_LIMIT/2-1] != 66 || read_buf[SD_DATA_BUFFER_BYTE_LIMIT/2 + SD_DATA_BUFFER_BYTE_LIMIT/10-1] != 67 || read_buf[2*SD_DATA_BUFFER_BYTE_LIMIT-1] != 65) {
		Serial.println("SD values were not written correctly");
		sd_i.end();
		while(1);
	}

	sd_i.end();
	Serial.println("SD test succeeded!");
}


void loop() {
}



