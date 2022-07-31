#include "iGlass_mic.h"

static 		PDMClass	INT_MIC_PDM(PIN_PDM_DIN, PIN_PDM_CLK, PIN_PDM_PWR);
static 		PDMClass  	EXT_MIC_PDM(D8, D7, D6);

volatile int16_t * 	_buf 			= nullptr;
volatile int 		_samples_read 	= 0;
PDMClass 			PDM 			= EXT_MIC_PDM;

void _onPDMdata();

iGlass_mic::iGlass_mic(int buf_size, int channels, int frequency, int mic_location) {
	_buf_size = buf_size;
	_channels = channels;
	_frequency = frequency;
	_mic_location = mic_location;
	_samples_read = 0;
}
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

	_buf = new int16_t[(int)(_buf_size/sizeof(int16_t))];  

	// Configure the data receive callback
	PDM.onReceive(_onPDMdata);	
	PDM.setBufferSize(_buf_size);	//if not called, 512 bytes by default

	if (!PDM.begin(_channels, _frequency)){
		#if DEBUG
			Serial.println("Failed to start PDM!");
		#endif
		while (1);
	}
	
	delay(100);		//not sure why, but first _onPDMdata _buf update is all zeros
}

/*
	Function:	Reads sensor data to buffer
	Input:		buf - ptr to data buffer
				num_samples - number of samples to read, should be the same as _samples_read
	Ret Val: 	num of sensor data pts read
*/
int iGlass_mic::read(int16_t * buf, int num_samples){			//what if we edit _samples_read to subtract num_samples, and add a new varaible like mic_data_buf_cur_read_idx..............?
	if (num_samples <= 0) {
		#if DEBUG
			Serial.println("Invalid num_samples (<= 0)!");
		#endif
		return 0;
	}

	int samples_read = 0;		//................rewrite logic!!! interrups() and noInterrupts placement doesnt make sense
	noInterrupts();
	samples_read = (int)_samples_read;
	interrupts();

	if (samples_read >= num_samples) {
		noInterrupts();
		memcpy(buf, (int16_t*)_buf, (size_t)(num_samples * sizeof(int16_t)));
		_samples_read = 0;		//.........................
		interrupts();
		return num_samples;
	} else {
		#if DEBUG
			Serial.println("Not enough samples yet!");
		#endif
		return 0;
	}
}



/*
	Function:	Nothing, placeholder
	Input:		None
	Ret Val: 	1 for success
*/
int iGlass_mic::write(){
	return 1;
}

/*
	Function:	Prints sensor data
	Input:		None
	Ret Val: 	None
*/
void iGlass_mic::print() {
	#if DEBUG
		int16_t * buf = new int16_t[(int)(_buf_size/sizeof(int16_t))];
		int samples_read = 0;

		noInterrupts();
		samples_read = _samples_read;
		memcpy(buf, (int16_t*)_buf, (size_t)(samples_read * sizeof(int16_t)));
		interrupts();

		for (int i = 0; i < samples_read; i++) {
			Serial.print(buf[i]);
			if (_channels == 2) {
				Serial.print(" ");
				Serial.print(buf[++i]);
				Serial.print(", ");
			} else {
				Serial.print(", ");
			}
		}
		Serial.println();
	#endif
}

/*
	Function:	Returns the number of samples read
	Input:		None
	Ret Val: 	number of samples read
*/
volatile int iGlass_mic::num_samples_read() {
	return _samples_read;
}

/*
	Function:	Gets the pointer to the internal data buffer
	Input:		None
	Ret Val: 	data buffer
*/
volatile int16_t * iGlass_mic::get_buf() {
	return _buf;
}

/*
	Function:	Ends the current microphone class, deletes pointers as needed
	Input:		None
	Ret Val: 	data buffer
*/
void iGlass_mic::end(){

	PDM.end();
	PDM.onReceive(NULL);

	delete[] _buf;
	_buf = nullptr;
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
	PDM.read((int16_t*)_buf, bytesAvailable);

	// 16-bit, 2 bytes per sample
	_samples_read = bytesAvailable / 2;
}