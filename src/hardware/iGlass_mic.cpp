#include "iGlass_mic.h"

static 		PDMClass	INT_MIC_PDM(PIN_PDM_DIN, PIN_PDM_CLK, PIN_PDM_PWR);
static 		PDMClass  	EXT_MIC_PDM(D8, D7, D6);

volatile int16_t * 	_buf 			= nullptr;
volatile int 		_samples_available 	= 0;
PDMClass 			PDM 			= EXT_MIC_PDM;

void _onPDMdata();

iGlass_mic::iGlass_mic(int buf_capacity, int channels, int frequency, int mic_location) {
	_buf_capacity = buf_capacity;
	_channels = channels;
	_frequency = frequency;
	_mic_location = mic_location;
	_samples_available = 0;
}
/*
	Function: 	Begin and setup current instance of MIC
	Input: 		None
	Ret Val: 	EXECUTION_SUCCESS
				EXECUTION_FAILURE - failure in beginning instance of MIC...........maybe include failure in setting up instance of MIC in the future
*/
int iGlass_mic::init(){
	if (mic_setup_flag == 1)
        return EXECUTION_SUCCESS;

	switch(_mic_location) {
		case INT_MIC:
			PDM = INT_MIC_PDM;
			break;

		case EXT_MIC:
			PDM = EXT_MIC_PDM;
			break;
	}

	_buf = new int16_t[_buf_capacity];  

	// Configure the data receive callback
	PDM.onReceive(_onPDMdata);	
	PDM.setBufferSize(_buf_capacity * MIC_SAMPLE_BYTE_SIZE);	//if not called, 512 bytes by default

	if (!PDM.begin(_channels, _frequency)){
		#if DEBUG_MIC
			Serial.println("Failed to initialize PDM!");
		#endif
		return EXECUTION_FAILURE;
	}
	
	delay(100);		//not sure why, but first _onPDMdata _buf update is all zeros

	mic_setup_flag = 1;

    return EXECUTION_SUCCESS;
}

/*
	Function:	Reads sensor data to buffer
	Input:		buf - ptr to data buffer
				num_samples - requested number of samples to read, should be the same as _samples_available
	Ret Val: 	number of sensor samples read (num_samples_to_read)
				EXECUTION_SUCCESS - not enough sensor samples available; buffer values not changed
				EXECUTION_FAILURE - iGlass_mic instance not setup; invalid argument(s); buffer values not changed
*/
int iGlass_mic::read(int16_t * buf, int num_samples){			//what if we edit _samples_available to subtract num_samples in the future, and add a new varaible like mic_data_buf_cur_read_idx..............?
	//checks
	if (mic_setup_flag == 0) {
		#if DEBUG_MIC
            Serial.println("iGlass_mic instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	}
	if (buf == nullptr) {
		#if DEBUG_MIC
			Serial.println("Invalid MIC buf argument (nullptr)!");
		#endif
		return EXECUTION_FAILURE;
	}
	if (num_samples <= 0) {
		#if DEBUG_MIC
			Serial.println("Invalid MIC num_samples argument (<= 0)!");
		#endif
		return EXECUTION_FAILURE;
	}

	int num_samples_to_read = min(num_samples, _buf_capacity);

	noInterrupts();
	if ((int)_samples_available >= num_samples_to_read) {	
		memcpy(buf, (int16_t*)_buf, (size_t)(num_samples_to_read * MIC_SAMPLE_BYTE_SIZE));
		_samples_available = 0;
		interrupts();
		return num_samples_to_read;
	}
	interrupts();

	#if DEBUG_MIC
		Serial.println("Not enough MIC samples yet!");
	#endif
	return EXECUTION_SUCCESS;
}

/*
	Function:	Nothing, placeholder
	Input:		None
	Ret Val: 	EXECUTION_SUCCESS
				EXECUTION_FAILURE - iGlass_mic instance not setup
*/
int iGlass_mic::write(){
	if (mic_setup_flag == 0) {
		#if DEBUG_MIC
            Serial.println("iGlass_mic instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	return EXECUTION_SUCCESS;
}



/*
	Function:	Prints all available/unread samples (from _buf)
	Input:		None
	Ret Val: 	number of sensor samples printed (num_samples_to_print)
				EXECUTION_SUCCESS - not enough sensor samples available
				EXECUTION_FAILURE - iGlass_mic instance not setup
*/
int iGlass_mic::print() {
	if (mic_setup_flag == 0) {
		#if DEBUG_MIC
            Serial.println("iGlass_mic instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	#if DEBUG_MIC
		int16_t * buf = new int16_t[_buf_capacity];
		int num_samples_to_print = 0;

		noInterrupts();
		num_samples_to_print = _samples_available;		//..........could change in the future if read() logic changes
		memcpy(buf, (int16_t*)_buf, (size_t)(num_samples_to_print * MIC_SAMPLE_BYTE_SIZE));
		interrupts();

		if (num_samples_to_print == 0) {
			#if DEBUG_MIC
				Serial.println("Not enough MIC samples yet!");
			#endif
			return EXECUTION_SUCCESS;
		}

		for (int i = 0; i < num_samples_to_print; i++) {
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

		delete[] buf;
		buf = nullptr;
	#endif

	return num_samples_to_print;
}

/*
	Function:	Returns the number of samples read from PDM's internal buffer
	Input:		None
	Ret Val: 	number of (unread) samples available
				EXECUTION_FAILURE - iGlass_mic instance not setup............check if needed in the future, might always be _buf_capacity
*/
volatile int iGlass_mic::samplesAvailable() {
	if (mic_setup_flag == 0) {
		#if DEBUG_MIC
            Serial.println("iGlass_mic instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
	} 
	return _samples_available;
}


/*
	Function:	Ends the current instance of iGlass_mic
	Input:		None
	Ret Val: 	None
*/
void iGlass_mic::end(){
	if (mic_setup_flag == 0)
		return;

	PDM.end();					//..........in the future, edit the source code to return a value
	PDM.onReceive(NULL);		//..........in the future, edit the source code to return a value

	delete[] _buf;
	_buf = nullptr;

	mic_setup_flag = 0;
}

/*
	Function:	Callback function for full PDM internal buffer, reads out data to iGlass_mic's buffer
	Input:		None
	Ret Val: 	None
*/
void _onPDMdata() {
	// Query the number of available bytes
	int bytesAvailable = PDM.available();

	// Read into the sample buffer
	PDM.read((int16_t*)_buf, bytesAvailable);		//.........has a ret val, can do something with it in the future

	// 16-bit, 2 bytes per sample
	_samples_available = bytesAvailable / MIC_SAMPLE_BYTE_SIZE;
}