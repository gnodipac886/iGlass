#include "iGlass_ir.h"
#include "iGlass.h"

/*
	Function: 	Begin and setup current instance of IR
	Input: 		None
	Ret Val: 	None
*/
void iGlass_ir::init() {
	if (ir_setup_flag == 1) 
		return;

	#if DEBUG
		if(!Serial){
			Serial.begin(115200);
			while (!Serial);
		}
	#endif

    // for setGestureSensitivity(..) a value between 1 and 100 is required.
    // Higher values makes the gesture recognition more sensible but less accurate
    // (a wrong gesture may be detected). Lower values makes the gesture recognition
    // more accurate but less sensible (some gestures may be missed).
    // Default is 80
    // APDS.setGestureSensitivity(ges_sensitivity);

    // Library tries to set correct pin automatically based on available board data. The sensor is read
    // even if the interrupt pin is not set or automatically found; setting the interrupt pin manually,
    // if not automatically found, will increase performance.
    // pin is -1 if not connected
    // APDS.setInterruptPin(int_pin);

    // Sets the LED current boost value:
    // 0=100%, 1=150%, 2=200%, 3=300%
    // *Note: More boost results in more power consumption
    // APDS.setLEDBoost(led_boost_val);

    if (!APDS.begin()) {
        #if DEBUG
            Serial.println("Error initializing APDS9960 sensor!");
        #endif
        return;//while(1)
    }
    
	ir_setup_flag = 1;
}

/*
	Function: 	Returns ir_setup_flag
	Input: 		None
	Ret Val: 	1 - init() was successfully run
                0 - otherwise (init() was not called, init() was unsuccessful)
*/
bool iGlass_ir::isSetup() {                      
    return ir_setup_flag;
}

/*
	Function:	Reads sensor data to buffer
	Input:		buf - ptr to data buffer; sensor - GES/COL/PROX
	Ret Val: 	num of sensor data pts read
*/
int iGlass_ir::read(int * buf, int num_samples, int sensor) { 
	int samples_read;
	switch(sensor) {
		case GES:
			samples_read = read_ges(buf, num_samples);
			break;
		case COL:
			samples_read = read_col(buf, num_samples);
			break;
		case PROX:
			samples_read = read_prox(buf, num_samples);
			break;

		default:
			samples_read = 0;
            #if DEBUG
                Serial.println("Invalid sensor argument!");
            #endif
			break;
	}

	return samples_read;
}


/*
	Function:	Reads gesture data to buffer; helper function to read
	Input:		buf - ptr to data buffer 
				num_samples - number of samples to read, here we only ever read 1 sample
	Ret Val: 	num of gesture data pts read
*/
int iGlass_ir::read_ges(int * buf, int num_samples) {//.............num_samples????????????????
    if (APDS.gestureAvailable()){
        // Gesture directions are as follows:
        //   - 0 GESTURE_UP:    from USB connector towards antenna
        //   - 1 GESTURE_DOWN:  from antenna towards USB connector
        //   - 2 GESTURE_LEFT:  from analog pins side towards digital pins side
        //   - 3 GESTURE_RIGHT: from digital pins side towards analog pins side
        buf[0] = APDS.readGesture();
        #if DEBUG
            Serial.print("readGesture ret val: ");
            Serial.println(buf[0]);
	    #endif
        return 1;
    }
    // - currently, -1 GESTURE_NONE: the gesture doesn’t match any of the above.
    #if DEBUG
        Serial.println("Gesture not available");
	#endif
	return 0;
}

/*
	Function:	Reads color data to buffer; helper function to read
	Input:		buf - ptr to data buffer 
				num_samples - number of samples to read, here we only ever read 1 sample
	Ret Val: 	num of color data pts read
*/
int iGlass_ir::read_col(int * buf, int num_samples) {//............num_samples???????how to deal w/ it??????
    if (APDS.colorAvailable()){
        int r, g, b;
        if(APDS.readColor(r,g,b)) {
            buf[0] = r;
            buf[1] = g;
            buf[2] = b;
            #if DEBUG
                Serial.print("readColor RGB ret val: ");
                Serial.print(r);
                Serial.print(" ");
                Serial.print(g);
                Serial.print(" ");
                Serial.println(b);
            #endif
            return 1;
        } else {
            #if DEBUG
                Serial.println("readColor unsuccessful...");
            #endif
        }
    } else {
        #if DEBUG
            Serial.println("Color not available");
        #endif
    }
	return 0;
}

/*
	Function:	Reads proximity data to buffer; helper function to read
	Input:		buf - ptr to data buffer 
				num_samples - number of samples to read, here we only ever read 1 sample
	Ret Val: 	num of proximity data pts read
*/
int iGlass_ir::read_prox(int * buf, int num_samples) {//............num_samples???????how to deal w/ it??????
    if (APDS.proximityAvailable()) {
        // read the proximity
        // - 0   => close
        // - 255 => far
        // - -1  => error
        int proximity = APDS.readProximity();
        if (proximity == -1) {
            #if DEBUG
                Serial.println("readProximity unsuccessful...");
            #endif
        } else {
            buf[0] = proximity;
            #if DEBUG
                Serial.print("readProximity ret val: ");
                Serial.println(proximity);
            #endif
            return 1;
        }
    } else {
        #if DEBUG
            Serial.println("Proximity not available");
        #endif
    }
	return 0;
}

/*
	Function:	Ends the current instance of IR
	Input:		None
	Ret Val: 	None
*/
void iGlass_ir::end() {
	if (ir_setup_flag == 0)
		return;

	APDS.end();

	ir_setup_flag = 0;
}

/*
	Function: Reads and prints first/current raw gesture, color, and proximity data points
	Input: None
	Ret Val: None
*/
void iGlass_ir::print() {
	#if DEBUG
        if (APDS.gestureAvailable()){
            // Gesture directions are as follows:
            //   - 0 GESTURE_UP:    from USB connector towards antenna
            //   - 1 GESTURE_DOWN:  from antenna towards USB connector
            //   - 2 GESTURE_LEFT:  from analog pins side towards digital pins side
            //   - 3 GESTURE_RIGHT: from digital pins side towards analog pins side
            //   - -1 GESTURE_NONE: the gesture doesn’t match any of the above.
            int gesture = APDS.readGesture();
            switch (gesture) {
                case GESTURE_UP:
                    Serial.println("readGesture ret val: GESTURE_UP");
                    break;

                case GESTURE_DOWN:
                    Serial.println("readGesture ret val: GESTURE_DOWN");
                    break;

                case GESTURE_LEFT:
                    Serial.println("readGesture ret val: GESTURE_LEFT");
                    break;

                case GESTURE_RIGHT:
                    Serial.println("readGesture ret val: GESTURE_RIGHT");
                    break;

                default:
                    break;
                }
        }
        
        if (APDS.colorAvailable()){
            int r, g, b;
            APDS.readColor(r,g,b);

            Serial.print("readColor RGB ret val: ");
            Serial.print(r);
            Serial.print(" ");
            Serial.print(g);
            Serial.print(" ");
            Serial.println(b);
        }

        if (APDS.proximityAvailable()) {
            // read the proximity
            // - 0   => close
            // - 255 => far
            // - -1  => error
            int proximity = APDS.readProximity();

            Serial.print("readProximity ret val: ");
            Serial.println(proximity);
        }
	#endif
}