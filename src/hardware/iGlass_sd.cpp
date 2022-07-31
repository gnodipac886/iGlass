#include "iGlass_sd.h"


/*
	Function: 	setup iGlass_sd - detect SD card, initialize SD, reset/setup iGlass directory in SD card
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::init() {
    if (sd_setup_flag == 1)
        return;

    #if DEBUG
        if(!Serial){
            Serial.begin(115200);
            while (!Serial);
        }
	#endif

    pinMode(CHIP_SELECT, OUTPUT);
	pinMode(CHIP_DETECT, INPUT);

    while (!digitalRead(CHIP_DETECT)) {
		#if DEBUG
		    Serial.println("No SD card detected");
		#endif
		delay(1000);
	}

    card_present = digitalRead(CHIP_DETECT);
    attachInterrupt(digitalPinToInterrupt(CHIP_DETECT), update_card_detect, CHANGE);    //.................somehow not working.......?


    if (!SD.begin(SD_CONFIG)) {
        #if DEBUG
            Serial.println("initialization failed!");
        #endif
        return;//while (1);
    }

    //remove possibly existing iGlass data from previous use
    if(SD.exists(iGlass_dirname)) {
        iGlass_dir = SD.open(iGlass_dirname);    
        clearDirectory(iGlass_dir);
    } else {
        SD.mkdir(iGlass_dirname);
        iGlass_dir = SD.open(iGlass_dirname); 
    }

    sd_setup_flag = 1;
}


/*
	Function: 	clears/empties directory of SD card
	Input: 		dir - the ExFile directory to clear
	Ret Val: 	None
*/
void iGlass_sd::clearDirectory(ExFile dir) {
    while (true) {
        ExFile entry = dir.openNextFile();
        char entry_name[max_filename_len];//* entry_name;  //....added
        if (!entry) {
            //no more files
            break;
        }
        if (entry.isDirectory()) {
            clearDirectory(entry);
        } else {
            if (entry.getName(entry_name,max_filename_len)) {   //.....raondom size number, still not sure what it means
                string iGlass_fpath = string(iGlass_dirname) + "/" + string(entry_name);
                SD.remove(const_cast<char*>(iGlass_fpath.c_str()));
            }
        }
    }
}

/*
	Function: 	Returns sd_setup_flag
	Input: 		None
	Ret Val: 	1 - init() was successfully run
                0 - otherwise (init() was not called, init() was unsuccessful)
*/
bool iGlass_sd::isSetup() {                      
    return sd_setup_flag;
}

/*
	Function: 	Returns card_present
	Input: 		None
	Ret Val: 	1 - SD card is detected (in SD card slot)
                0 - otherwise
*/
bool iGlass_sd::available() {
    return card_present;       
}

/*
	Function: 	Ends current instance of iGlass_sd
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::end() {
	if (sd_setup_flag == 0)
		return;

    closeFiles();
    iGlass_dir.close();
	detachInterrupt(digitalPinToInterrupt(CHIP_DETECT));

	sd_setup_flag = 0;
}

/*
	Function: 	close files in iGlass_sd directory (files added through the iGlass_sd instance)
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::closeFiles() {
    for (int file_idx = 0; file_idx != sd_num_files; file_idx++) {
        file_des[file_idx]->file_ptr->close();
        delete file_des[file_idx]->file_ptr;
        file_des[file_idx]->file_ptr = nullptr;
        delete file_des[file_idx];
        file_des[file_idx] = nullptr;
    }
    sd_num_files = 0;
}

/*
	Function: 	updates card_present upon changes in card detection, and then initializes SD if card_present = 1 meaning it is re-inserted
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::update_card_detect() {         
    card_present = digitalRead(CHIP_DETECT);
    if (card_present) {
        if (!SD.begin(SD_CONFIG)) {
            #if DEBUG
                Serial.println("re-initialization failed!");
            #endif
            while (1);
        }
    }
}

/*
	Function: 	add new file to iGlass_sd directory in SD card, with filename fname
	Input: 		fname - filename of new file to be added
	Ret Val:    successfully-added file's file_idx for the file descriptor array; -1 if unsuccessful
*/
int iGlass_sd::addNewFile(char * fname) { 
    //checks
    if (!isSetup()) {
        #if DEBUG
            Serial.println("init() has not been called yet");
        #endif
        return -1;  
    }
    if (!available()) {
        #if DEBUG
            Serial.println("No SD card detected");
        #endif
        return -1;  
    }
    if (fname == nullptr) {
        #if DEBUG
            Serial.println("nullptr given for file name");
        #endif
        return -1;  
    }

    string iGlass_fpath_str = string(iGlass_dirname) + "/" + string(fname);
    char * iGlass_fpath = const_cast<char*>(iGlass_fpath_str.c_str());

    if (SD.exists(iGlass_fpath)) {
        #if DEBUG
            Serial.print(String(fname));
            Serial.println(" file(name) already exists!");   //all existing files in iGlass_sd directory should be added through this function
        #endif
        return -1;
    }
    if (sd_num_files >= SET_NUM_FILES_LIMIT) {  
        #if DEBUG
            Serial.println("(chosen) Num File limit reached");
        #endif
        return -1;
    }

    file_des[sd_num_files] = new dentry;
    file_des[sd_num_files]->filename = fname;
    file_des[sd_num_files]->file_ptr = new ExFile(SD.open(iGlass_fpath, O_RDWR | O_CREAT | O_APPEND));
    file_des[sd_num_files]->read_position = 0;

    if (!(*(file_des[sd_num_files]->file_ptr))) { 
        #if DEBUG
            Serial.println("the file did not open succesfully; file not added");
        #endif
        delete file_des[sd_num_files]->file_ptr;//......do i need
        file_des[sd_num_files]->file_ptr = nullptr;//.......do i need
        delete file_des[sd_num_files];
        file_des[sd_num_files] = nullptr;
        return -1; 
    } 

    return sd_num_files++;
}

/*
	Function: 	write to given file (indicated by file's idx to file descriptor array)
	Input: 		f_num - file des array idx of the given file to write to
                buf - ptr to buffer of data to copy from into the given file
                buf_size - number of bytes to copy from given buffer
	Ret Val:    number of bytes successfully written to given iGlass_sd file
*/
int iGlass_sd::write(int f_num, byte * buf, int buf_size) {
    //checks
    if (!isSetup()) {
        #if DEBUG
            Serial.println("init() has not been called yet");
        #endif
        return 0;  
    } 
    if (!available()) {
        #if DEBUG
            Serial.println("No SD card detected");
        #endif
        return 0;  
    } 
    if (f_num < 0 && f_num >= sd_num_files) {
        #if DEBUG
            Serial.println("the given file number does not exist");
        #endif
        return 0; 
    }
    if (file_des[f_num] == nullptr) {
        #if DEBUG
            Serial.println("the given file is not added");
        #endif
        return 0; 
    }
    if (buf == nullptr) {
        #if DEBUG
            Serial.println("nullptr given for data to be written");
        #endif
        return 0; 
    }
    if (buf_size <= 0) {
        #if DEBUG
            Serial.println("invalid buffer size given for data to be written");
        #endif
        return 0; 
    }

    int total_bytes_written = 0;
    /*Just in case there is a buffer limit to SD write capacity*/
    // int bytes_left_to_write = buf_size;
    // int bytes_to_write;
    // while (bytes_left_to_write > 0) {
    //     bytes_to_write = min(bytes_left_to_write, SD_DATA_BUFFER_BYTE_LIMIT);
    //     bytes_written = file_des[f_num]->file_ptr->write((byte *) buf + (buf_size-bytes_left_to_write), (size_t) bytes_to_write);
    //     total_bytes_written += bytes_written;
    //     file_des[f_num]->file_ptr->flush();
    //     bytes_left_to_write -= bytes_written; 
    // }
    total_bytes_written += file_des[f_num]->file_ptr->write((byte *) buf, (size_t) buf_size);
    file_des[f_num]->file_ptr->flush();

    return total_bytes_written;
}

/*
	Function: 	read from given file (indicated by file's idx to file descriptor array)
	Input: 		f_num - file des array idx of the given file to read from
                buf - ptr to buffer to copy the given file's data into                
                buf_size - number of bytes to read from given file
	Ret Val:    number of bytes successfully read from given iGlass_sd file
*/
int iGlass_sd::read(int f_num, byte * buf, int buf_size) { 
	//checks
	 if (!isSetup()) {
        #if DEBUG
            Serial.println("init() has not been called yet");
        #endif
        return 0;  
    } 
    if (!available()) {
        #if DEBUG
            Serial.println("No SD card detected");
        #endif
        return 0;  
    } 
    if (f_num < 0 && f_num >= sd_num_files) {
        #if DEBUG
            Serial.println("the given file number does not exist");
        #endif
        return 0; 
    }
    if (file_des[f_num] == nullptr) {
        #if DEBUG
            Serial.println("the given file is not added");
        #endif
        return 0; 
    }
    if (buf == nullptr) {
        #if DEBUG
            Serial.println("nullptr given for data to be written");
        #endif
        return 0; 
    }
    if (buf_size <= 0) {
        #if DEBUG
            Serial.println("invalid buffer size given for data to be written");
        #endif
        return 0; 
    }

    //need to close and reopen file in order to have non-zero file.available() (when file.size() != 0) 
    file_des[f_num]->file_ptr->close();
    delete file_des[f_num]->file_ptr;
    file_des[f_num]->file_ptr = nullptr;

    string iGlass_fpath_str = string(iGlass_dirname) + "/" + string(file_des[f_num]->filename);
    file_des[f_num]->file_ptr = new ExFile(SD.open((const_cast<char*>(iGlass_fpath_str.c_str())), O_RDWR | O_CREAT | O_APPEND));

    if (!(*(file_des[f_num]->file_ptr))) { 
        #if DEBUG
            Serial.println("the file did not open succesfully");
        #endif
        delete file_des[f_num]->file_ptr;//.......do i need
        file_des[f_num]->file_ptr = nullptr;//.........do i need
        return 0; 
    }
 
    if (file_des[f_num]->read_position >= file_des[f_num]->file_ptr->size()) {
        file_des[f_num]->read_position = 0;
    }
    file_des[f_num]->file_ptr->seek(file_des[f_num]->read_position);

    int total_bytes_read = 0;
    int bytes_read;
    int bytes_left_to_read = min((int)file_des[f_num]->file_ptr->available(), buf_size);
    
    /*Just in case there is a buffer limit to SD read capacity*/
    // int bytes_to_read;
    // while(bytes_left_to_read > 0) {
    //     bytes_to_read = min(bytes_left_to_read, SD_DATA_BUFFER_BYTE_LIMIT);
    //     file_des[f_num]->file_ptr->read(buf + total_bytes_read, (size_t) bytes_to_read);
    //     bytes_read = file_des[f_num]->file_ptr->position() - file_des[f_num]->read_position;
    //     total_bytes_read += bytes_read;
    //     file_des[f_num]->read_position = file_des[f_num]->file_ptr->position();
    //     bytes_left_to_read -= bytes_read;
    // }

	file_des[f_num]->file_ptr->read(buf,bytes_left_to_read);
    bytes_read = file_des[f_num]->file_ptr->position() - file_des[f_num]->read_position;
    total_bytes_read += bytes_read;
    file_des[f_num]->read_position = file_des[f_num]->file_ptr->position();
    bytes_left_to_read -= bytes_read;

	return total_bytes_read;
}



