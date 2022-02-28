#include "iGlass_mic.h"

void iGlass_mic::init(){
	switch(_mic_location) {
		case INT_MIC:
			_PDM = new PDMClass(PIN_PDM_DIN, PIN_PDM_CLK, PIN_PDM_PWR);
			break;

		case EXT_MIC:
			_PDM = new PDMClass(D8, D7, D6);
			break;
	}

	_buf = new int16_t[_buf_size];

	// Configure the data receive callback
	_PDM->onReceive(_onPDMdata);
	_PDM->setBufferSize(_buf_size);

	if (!_PDM->begin(_channels, _frequency)){
		#if DEBUG
			Serial.println("Failed to start PDM!");
		#endif
		while (1);
	}
}

int iGlass_mic::read(int16_t * buf, int num_samples){
	if (_samples_read == num_samples) {
		memcpy(buf, _buf, num_samples * sizeof(int16_t));
		return _samples_read;
	}
	return 0;
}

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

int iGlass_mic::get_samples_read() {
	return _samples_read;
}

int16_t * iGlass_mic::get_buf() {
	return _buf;
}

void iGlass_mic::_onPDMdata() {
	// Query the number of available bytes
	int bytesAvailable = _PDM->available();

	// Read into the sample buffer
	_PDM->read(_buf, bytesAvailable);

	// 16-bit, 2 bytes per sample
	_samples_read = bytesAvailable / 2;
}

void iGlass_mic::end(){
	_PDM->end();
	delete(_PDM);
	delete(_buf);
}