#include "iGlass_mic.h"

static 		PDMClass	INT_MIC_PDM(PIN_PDM_DIN, PIN_PDM_CLK, PIN_PDM_PWR);
static 		PDMClass  	EXT_MIC_PDM(D8, D7, D6);

int16_t * 		_buf 			= NULL;
volatile int 	_samples_read 	= 1;
PDMClass 		PDM 			= EXT_MIC_PDM;
volatile int 	_mic_int		= 0;

void _onPDMdata();

/*
	Function: 	Begin and setup current instance of mic
	Input: 		None
	Ret Val: 	None
*/
void iGlass_mic::init(){
	switch(_mic_location) {
		case INT_MIC:
			PDM = INT_MIC_PDM;
			break;

		case EXT_MIC:
			PDM = EXT_MIC_PDM;
			break;
	}

	_buf = new int16_t[_buf_size];

	// Configure the data receive callback
	PDM.onReceive(_onPDMdata);
	PDM.setBufferSize(_buf_size);

	if (!PDM.begin(_channels, _frequency)){
		#if DEBUG
			Serial.println("Failed to start PDM!");
		#endif
		while (1);
	}
}

/*
	Function:	Reads sensor data to buffer
	Input:		buf - ptr to data buffer
				num_samples number of samples to read, should be the same as _samples_read
	Ret Val: 	num of sensor data pts read
*/
int iGlass_mic::read(int16_t * buf, int num_samples){
	if (_samples_read == num_samples) {
		memcpy(buf, _buf, num_samples * sizeof(int16_t));
		return _samples_read;
	}
	return 0;
}

/*
	Function:	Prints sensor data
	Input:		None
	Ret Val: 	None
*/
void iGlass_mic::print() {
	for (int i = 0; i < _samples_read; i++) {
		if (_channels == 2) {
			#if DEBUG
				Serial.print(String(_buf[i]) + " ");
			#endif
		}
		#if DEBUG
			Serial.println(String(_buf[++i]));
		#endif
	}
}

/*
	Function:	Gets the number of samples read
	Input:		None
	Ret Val: 	number of samples read
*/
int iGlass_mic::get_samples_read() {
	return _samples_read;
}

/*
	Function:	Gets the pointer to the internal data buffer
	Input:		None
	Ret Val: 	data buffer
*/
int16_t * iGlass_mic::get_buf() {
	return _buf;
}

/*
	Function:	Ends the current microphone class, deletes pointers as needed
	Input:		None
	Ret Val: 	data buffer
*/
void iGlass_mic::end(){
	PDM.end();
	delete(_buf);
}

/*
	Function:	Interrupt handler for the microphone, reads out data to internal buffer
	Input:		None
	Ret Val: 	None
*/
void _onPDMdata() {
	// Query the number of available bytes
	int bytesAvailable = PDM.available();

	// Read into the sample buffer
	PDM.read(_buf, bytesAvailable);

	// 16-bit, 2 bytes per sample
	_samples_read = bytesAvailable / 2;

	_mic_int 	= 1;
}