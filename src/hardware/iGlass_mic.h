#ifndef iGLASS_MIC_H
#define iGLASS_MIC_H

#include "arduino.h"
#include <PDM.h>
// #include "../deps/PDM/src/PDM.h"

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define LOW_FREQ		16000
#define HIGH_FREQ		41667	// default number of output channels; default PCM output frequency, 41667

#define INT_MIC 		0
#define	EXT_MIC 		1

#define PDM_BUF_SIZE 	1024 * 16

#define DEBUG 			1


//----------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// iGlass mic class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_mic {
	public:
		iGlass_mic(int buf_size = PDM_BUF_SIZE, int channels = 2, int frequency = HIGH_FREQ, int mic_location = EXT_MIC): 
		_channels(channels), 
		_frequency(frequency),
		_mic_location(mic_location),
		_buf_size(buf_size)
		{}
		void 				init();
		void 				end();
		int 				read(int16_t * buf, int num_samples);
		int 				get_samples_read();
		void 				print();
		int16_t * 			get_buf();

	private:
		int 		_buf_size;
		int 		_mic_location;
		char 		_channels;
		int 		_frequency;
};

#endif