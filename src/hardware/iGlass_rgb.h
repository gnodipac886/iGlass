#ifndef iGLASS_RGB_H
#define iGLASS_RGB_H

#include "arduino.h"
#include <RGBLed.h>

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define iGLASS_RED_PIN 			22
#define iGLASS_GREEN_PIN 		23
#define iGLASS_BLUE_PIN 		24

// const int iGLASS_RED_PIN = 			22;
// const int iGLASS_GREEN_PIN = 		23;
// const int iGLASS_BLUE_PIN = 		24;
// #define DEBUG 					1


//----------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// iGlass mic class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_rgb {
	public:
		void 				init();
		void 				end();
		int 				read(int buf[3]);
		int 				write(int buf[3]);
		int 				write(int red, int green, int blue);
		RGBLed 				get_RGBLed();

	private:
		int 				_red;
		int 				_green;
		int 				_blue;
};

#endif