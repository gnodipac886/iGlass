#ifndef iGLASS_MIC_H
#define iGLASS_MIC_H

#include "arduino.h"
#include <PDM.h>

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define LOW_FREQ		16000	// for MONO
#define HIGH_FREQ		41667	// for STEREO; default number of output channels; default PCM output frequency, 41667

#define INT_MIC 		0		// Arduino Nano 33 Ble Sense Mic
#define	EXT_MIC 		1

#define PDM_BUF_SIZE 	1024 	//PDM's buffer size (in bytes)

// #define DEBUG 			1


//----------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// iGlass mic class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_mic {
	public:
		iGlass_mic(int buf_size = PDM_BUF_SIZE, int channels = 2, int frequency = HIGH_FREQ, int mic_location = EXT_MIC);
		void 				init();
		void 				end();
		int 				read(int16_t * buf, int num_samples);
		int 				write();
		volatile int		num_samples_read();
		void 				print();
		volatile int16_t * 	get_buf();

	private:
		int 		_buf_size;
		char 		_channels;
		int 		_frequency;
		int 		_mic_location;
};

#endif