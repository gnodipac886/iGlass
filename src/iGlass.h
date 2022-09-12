#ifndef iGLASS_H
#define iGLASS_H
	// #include "api/iGlass_ble_rounded_angle.h"//...............
	// #include "api/iGlass_ble_send_imu.h" //....................
	// #include "hardware/iGlass_ble.h" //......................sanity checks (like incorporate available.....etc) + make get stuck in while loop for some
	// #include "hardware/iGlass_mic.h"	//.............double checking
	// #include "hardware/iGlass_rgb.h"	//..............double checking
	// #include "hardware/iGlass_ir.h" //.......print is weird+figure out source code w/ gesture, source code is weird.............
	#include "hardware/iGlass_sd.h"		//.........................double checking
	#include "hardware/iGlass_imu.h"
	// pressure sensor..........................................................
	//..........unify if not .begin() while(1) to something else like return before setting setup_flag to 1?.....?
#endif

//.................double check all ino files