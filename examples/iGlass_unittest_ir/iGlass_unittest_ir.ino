#include <iGlass.h>

void setup() {

	/*Unittest init*/
	Serial.println("Before initialization...");

	if (!Serial){				
		Serial.begin(115200);
		while(!Serial);
	}

	Serial.println("Initialized!");

	//pinMode(LED_BUILTIN, OUTPUT);		//.........do we need this?

	/*IR unittest*/
	iGlass_ir ir_i = iGlass_ir();

    Serial.println("Before IR init...");

    ir_i.init();

    Serial.println("IR initialized!");

    Serial.println("Before IR read...");

    int gesture, proximity;
    int gesture_count = 0;
	int col_available = 0;
	int prox_available = 0;
    int rgb_col[3];

    // while (gesture_count < 10) {

	// 	if(ir_i.read(rgb_col,1,COL)) {
	// 		col_available = 1;
	// 	}
    // 	// Serial.println("1");
	// 	if(ir_i.read(&proximity,1,PROX)) {
	// 		prox_available = 1;
	// 	}
    // 	// Serial.println("2");

    // 	if (ir_i.read(&gesture, 1, GES) && col_available && prox_available) {
    //       	Serial.print("Gesture ");
	// 		Serial.print(gesture_count);
	// 		Serial.print(": ");
	// 		Serial.println(gesture);

	// 		Serial.print("RGB color ");
	// 		Serial.print(gesture_count);
	// 		Serial.print(": ");
	// 		Serial.print(rgb_col[0]);
	// 		Serial.print(" ");
	// 		Serial.print(rgb_col[1]);
	// 		Serial.print(" ");
	// 		Serial.println(rgb_col[2]);

	// 		Serial.print("Proximity ");
	// 		Serial.print(gesture_count);
	// 		Serial.print(": ");
	// 		Serial.println(proximity);

	// 		gesture_count++;
	// 		col_available = 0;
	// 		prox_available = 0;
    //   	}
    // }

//.................................make sure that disablegesture call added in source code is valid by reading documentation; and then I can delete block of code above..............
//......................................disableGesture turns of PGSAT bit in status register, maybe that is why it only checks for proximity enabled in gestureavailable???
	for (int i = 0; i < 5; i++) {
		Serial.print("Proximity ");
		Serial.println(i);
		while(!ir_i.read(&proximity,1,PROX));
		Serial.print("Gesture ");
		Serial.println(i);
		while(!ir_i.read(&gesture, 1, GES));
		Serial.print("RGB color ");
		Serial.println(i);
		while(!ir_i.read(rgb_col,1,COL));
	}


    Serial.println("After IR read...");

    Serial.println("Before IR print...");

    for (int i = 0; i < 50; i++) {
        ir_i.print(); 
    }

    Serial.println("After IR print...");

    ir_i.end();

    Serial.println("IR test succeeded!");
}


void loop() {
}


