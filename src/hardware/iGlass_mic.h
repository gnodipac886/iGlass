#ifndef iGLASS_MIC_H
#define iGLASS_MIC_H

#include "arduino.h"
#include <PDM.h>
#include "iGlass_macros.h"

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define LOW_FREQ				16000	// for MONO
#define HIGH_FREQ				41667	// for STEREO

#define INT_MIC 				0		// Arduino Nano 33 Ble Sense Mic
#define	EXT_MIC 				1

#define PDM_BUF_SAMPLE_CAPACITY	512		// interrupt handler void _onPDMdata() called when PDM's internal buffer fills up
#define MIC_SAMPLE_BYTE_SIZE 	sizeof(int16_t)


//----------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// iGlass mic class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_mic {
	public:
		iGlass_mic(int buf_capacity = PDM_BUF_SAMPLE_CAPACITY, int channels = 2, int frequency = HIGH_FREQ, int mic_location = EXT_MIC);
		int 				init();
		void 				end();	
		int 				read(int16_t * buf, int num_samples); //.............
		int 				write();
		volatile int		samplesAvailable();
		int	 				print();

	private:
		int 		mic_setup_flag = 0;
		int 		_buf_capacity;
		char 		_channels;
		int 		_frequency;
		int 		_mic_location;
};

#endif