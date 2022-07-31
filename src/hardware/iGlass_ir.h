#ifndef iGLASS_IR_H
#define iGLASS_IR_H

#include "arduino.h"
#include "Arduino_APDS9960.h"   //find hardware i2c, also same issue for imu!!!....................

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

// read definitions
#define GES							0       // Gesture
#define COL 						1       // Color
#define PROX 						2       // Proximity
#define DEBUG						1 		

//----------------------------------------------------------------------------------------------------------------------
// iGlass ir class
//----------------------------------------------------------------------------------------------------------------------

class iGlass_ir {
    public:
        iGlass_ir(int ges_sensitivity = 80, int int_pin = 2, int led_boost_val = 0): ges_sensitivity(ges_sensitivity), int_pin(int_pin), led_boost_val(led_boost_val) {}//.....int_pin=2??????..................
		void 	init();
		void 	end();
        int 	read(int * buf, int num_samples, int sensor);
		void 	print();
        bool 	isSetup(); 

	private:
        int     ges_sensitivity, int_pin, led_boost_val;
		int 	ir_setup_flag = 0;
		int 	read_ges(int *buf, int num_samples);
		int 	read_col(int *buf, int num_samples);
		int 	read_prox(int *buf, int num_samples);
};

#endif