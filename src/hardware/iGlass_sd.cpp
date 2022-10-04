#include "iGlass_sd.h"


/*
	Function: 	Begin and setup current instance of iGlass_sd - detect SD card, begin iGlass_sd's instance of SD, reset/setup iGlass directory in SD card
	Input: 		None
	Ret Val: 	EXECUTION_SUCCESS
                EXECUTION_FAILURE - failure in beginning instance of SD..........maybe include failure in setting up in the future
*/
int iGlass_sd::init() {
    if (sd_setup_flag == 1)
        return EXECUTION_SUCCESS;

    #if DEBUG_SD
        if(!Serial){
            Serial.begin(115200);
            while (!Serial);
        }
	#endif

    pinMode(CHIP_SELECT, OUTPUT);
	pinMode(CHIP_DETECT, INPUT);

    while (!digitalRead(CHIP_DETECT)) {                                             //.................somehow not working.......?
		#if DEBUG_SD
		    Serial.println("No SD card detected");
		#endif
		delay(1000);
	}

    card_present = digitalRead(CHIP_DETECT);
    attachInterrupt(digitalPinToInterrupt(CHIP_DETECT), update_card_detect, CHANGE);    //.................^somehow not working.......?


    if (!SD.begin(SD_CONFIG)) {
        #if DEBUG_SD
            Serial.println("Failed to initialize SD!");
        #endif
        return EXECUTION_FAILURE;
    }

    //remove possibly existing iGlass data from previous use
    if(SD.exists(iGlass_dirname)) {
        iGlass_dir = SD.open(iGlass_dirname);    
        clearDirectory();
    } else {
        SD.mkdir(iGlass_dirname);
        iGlass_dir = SD.open(iGlass_dirname); 
    }

    sd_setup_flag = 1;

    return EXECUTION_SUCCESS;
}


/*
	Function: 	clears/empties iGlass directory; helper function to init
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::clearDirectory() { 
    ExFile entry;
    char entry_name[MAX_FILENAME_LEN];
    while (true) {
        entry = iGlass_dir.openNextFile();
        if (!entry) {
            //no more files
            break;
        }
        if (entry.getName(entry_name,MAX_FILENAME_LEN)) {   //.....random size number, still not sure what it means but needs to be more than 13 bytes maybe
            string iGlass_fpath = string(iGlass_dirname) + "/" + string(entry_name);
            SD.remove(const_cast<char*>(iGlass_fpath.c_str()));
        }
    }
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
	Function: 	closes files in the iGlass directory; helper function to end()
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
	Function: 	updates card_present upon changes in card detection, and then begins iGlass_sd's instance of SD if card_present = 1 (meaning the SD card is re-inserted)
	Input: 		None
	Ret Val: 	None
*/
void iGlass_sd::update_card_detect() {         
    card_present = digitalRead(CHIP_DETECT);
    #if DEBUG_SD
        Serial.println(card_present ? "SD card inserted" : "SD card removed");
    #endif
    if (card_present) {
        if (!SD.begin(SD_CONFIG)) {
            #if DEBUG_SD
                Serial.println("re-initialization failed!");
            #endif
            while (1);
        }
        //do i also need to reopen directory/files...................................?
    }
     //do i also need to close directory/files (needa do after each r/w access then)........do i also need to detach interrupts for chip detection...........................?
}   //might have SD filesystem corruption if we are in middle of read/write though (since file not closed yet)......................deal with this in the future

/*
	Function: 	add new file to iGlass directory, with filename fname
	Input: 		fname - filename of new file to be added
	Ret Val:    added file's file_idx for the file descriptor array
                EXECUTION_FAILURE - iGlass_sd instance not setup; 
                                    invalid argument; 
                                    file already added; 
                                    num files limit already reached; 
                                    unsuccessful in opening new file (not added)
*/
int iGlass_sd::addNewFile(char * fname) { 
    //checks
    if (sd_setup_flag == 0) {
        #if DEBUG_SD
            Serial.println("iGlass_sd instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
    }
    if (!card_present) {
        #if DEBUG_SD
            Serial.println("No SD card detected");
        #endif
        return EXECUTION_FAILURE;  
    }
    if (fname == nullptr) {
        #if DEBUG_SD
            Serial.println("Invalid fname argument (nullptr)!");
        #endif
        return EXECUTION_FAILURE;  
    }

    string iGlass_fpath_str = string(iGlass_dirname) + "/" + string(fname);
    char * iGlass_fpath = const_cast<char*>(iGlass_fpath_str.c_str());

    if (SD.exists(iGlass_fpath)) {
        #if DEBUG_SD
            Serial.print(String(fname));
            Serial.println(" file(name) already exists!");   //all existing files in iGlass directory should have been added through this function
        #endif
        return EXECUTION_FAILURE;
    }
    if (sd_num_files >= NUM_FILES_LIMIT) {  
        #if DEBUG_SD
            Serial.println("(chosen) num files limit reached");
        #endif
        return EXECUTION_FAILURE;
    }

    file_des[sd_num_files] = new dentry;
    file_des[sd_num_files]->filename = fname;
    file_des[sd_num_files]->file_ptr = new ExFile(SD.open(iGlass_fpath, O_RDWR | O_CREAT | O_APPEND));
    file_des[sd_num_files]->read_position = 0;

    if (!(*(file_des[sd_num_files]->file_ptr))) { 
        #if DEBUG_SD
            Serial.println("the file did not open succesfully; file not added");
        #endif
        delete file_des[sd_num_files]->file_ptr;//......do i need
        file_des[sd_num_files]->file_ptr = nullptr;//.......do i need
        delete file_des[sd_num_files];
        file_des[sd_num_files] = nullptr;
        return EXECUTION_FAILURE; 
    } 

    return sd_num_files++;
}

/*
	Function: 	write to given file (indicated by file's idx to file descriptor array)
	Input: 		f_num - file des array idx of the given file to write to
                buf - ptr to buffer of data to copy from into the given file
                buf_size - number of bytes to copy from given buffer
	Ret Val:    number of bytes successfully written to given iGlass_sd file (buf_size)
                EXECUTION_FAILURE - iGlass_sd instance not setup; invalid argument(s); failure in opening (closed) file; failure to write all buf_size bytes of data during execution -> file content still changed
*/
int iGlass_sd::write(int f_num, byte * buf, int buf_size) {
    //checks
    if (sd_setup_flag == 0) {
        #if DEBUG_SD
            Serial.println("iGlass_sd instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
    }
    if (!card_present) {
        #if DEBUG_SD
            Serial.println("No SD card detected");
        #endif
        return EXECUTION_FAILURE;  
    } 
    if (f_num < 0 || f_num >= sd_num_files) {
        #if DEBUG_SD
            Serial.println("Invalid f_num argument (< 0 || >= sd_num_files)!");
        #endif
        return EXECUTION_FAILURE; 
    }
    if (buf == nullptr) {
        #if DEBUG_SD
            Serial.println("Invalid buf argument (nullptr)!");
        #endif
        return EXECUTION_FAILURE; 
    }
    if (buf_size <= 0) {
        #if DEBUG_SD
            Serial.println("Invalid buf_size argument (<= 0)!");
        #endif
        return EXECUTION_FAILURE; 
    }
    if (file_des[f_num]->file_ptr == nullptr) {
        if (openAddedFile(f_num) == EXECUTION_FAILURE) {
            return EXECUTION_FAILURE;
        }
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

    if (total_bytes_written != buf_size) {
        #if DEBUG_SD
            Serial.println("execution failure in writing all buf_size bytes of data");
        #endif
        return EXECUTION_FAILURE;
    }

    return buf_size;        //............as with imu, shouldn't this be total_bytes_written......? check in future
}

/*
	Function: 	read from given file (indicated by file's idx to file descriptor array)
	Input: 		f_num - file des array idx of the given file to read from
                buf - ptr to buffer to copy the given file's data into                
                buf_size - number of bytes to read from given file
	Ret Val:    number of bytes successfully read from given iGlass_sd file (total_bytes_read)
                EXECUTION_FAILURE - iGlass_sd instance not setup; invalid argument(s); failure in opening (closed) file; failure to read available bytes of data during execution -> buffer values still changed
*/
int iGlass_sd::read(int f_num, byte * buf, int buf_size) { 
    //checks
    if (sd_setup_flag == 0) {
        #if DEBUG_SD
            Serial.println("iGlass_sd instance has not been setup");
        #endif
        return EXECUTION_FAILURE;  
    }
    if (!card_present) {
        #if DEBUG_SD
            Serial.println("No SD card detected");
        #endif
        return EXECUTION_FAILURE;  
    } 
    if (f_num < 0 || f_num >= sd_num_files) {
        #if DEBUG_SD
            Serial.println("Invalid f_num argument (< 0 || >= sd_num_files)!");
        #endif
        return EXECUTION_FAILURE; 
    }
    if (buf == nullptr) {
        #if DEBUG_SD
            Serial.println("Invalid buf argument (nullptr)!");
        #endif
        return EXECUTION_FAILURE; 
    }
    if (buf_size <= 0) {
        #if DEBUG_SD
            Serial.println("Invalid buf_size argument (<= 0)!");
        #endif
        return EXECUTION_FAILURE; 
    }

    //need to close and reopen file in order to have non-zero file.available() (when file.size() != 0)
    if (file_des[f_num]->file_ptr != nullptr) {
        file_des[f_num]->file_ptr->close();
        delete file_des[f_num]->file_ptr;
        file_des[f_num]->file_ptr = nullptr;
    } 
    if (openAddedFile(f_num) == EXECUTION_FAILURE) {
        return EXECUTION_FAILURE;
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
    // while (bytes_left_to_read > 0) {
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

    if (bytes_left_to_read > 0) {
        #if DEBUG_SD
            Serial.println("execution failure in reading available bytes of data");
        #endif
        return EXECUTION_FAILURE;
    }

	return total_bytes_read;
}

/*
	Function: 	open given file (indicated by file's idx to file descriptor array); helper function to read and write 
	Input: 		f_num - file des array idx of the given file to open
	Ret Val:    EXECUTION_SUCCESS
                EXECUTION_FAILURE - failure in opening file
*/
int iGlass_sd::openAddedFile(int f_num) {
    string iGlass_fpath_str = string(iGlass_dirname) + "/" + string(file_des[f_num]->filename);
    file_des[f_num]->file_ptr = new ExFile(SD.open((const_cast<char*>(iGlass_fpath_str.c_str())), O_RDWR | O_CREAT | O_APPEND));

    if (!(*(file_des[f_num]->file_ptr))) { 
        #if DEBUG_SD
            Serial.println("the file did not open succesfully");
        #endif
        delete file_des[f_num]->file_ptr;//.......do i need
        file_des[f_num]->file_ptr = nullptr;//.........do i need
        return EXECUTION_FAILURE; 
    }
    return EXECUTION_SUCCESS;
}



