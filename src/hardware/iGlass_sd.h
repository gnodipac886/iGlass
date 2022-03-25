#ifndef iGLASS_SD_H
#define iGLASS_SD_H

#include "SdFat.h"
#include "sdios.h"
#include "arduino.h"
#include <iostream>
using namespace std;
#define DEBUG 1//..........................................................
//----------------------------------------------------------------------------------------------------------------------
// defines for sd_fat class
//----------------------------------------------------------------------------------------------------------------------
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif // HAS_SDIO_CLASSs

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------
#define SET_NUM_FILES_LIMIT         10      //..................
#define SD_DATA_BUFFER_BYTE_LIMIT   512     //Actually 512
#define MIN_SDSC_CAPACITY           999967  //1 MB
#define CHIP_DETECT                 D9  
#define CHIP_SELECT                 D10

//----------------------------------------------------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------------------------------------------------
struct dentry {
        char * filename;
        ExFile * file_ptr;
        int read_position;
};

//----------------------------------------------------------------------------------------------------------------------
// iGlass sd class
//----------------------------------------------------------------------------------------------------------------------
class iGlass_sd {
    public:
        iGlass_sd(int sd_capacity = MIN_SDSC_CAPACITY): sd_capacity(sd_capacity) {}
        void 		init();    
        void 		end();   
		int 		write(int f_idx, byte * buf, int buf_size); 
        int         read(int f_idx, byte * buf, int buf_size);   
		int 		addNewFile(char * fname); 
		bool 		available();  
		bool 		isSetup();    

    private:
        ExFile      iGlass_dir; 
        char *      iGlass_dirname = "iGlass_data"; 
        unsigned long long  sd_capacity; // in bytes
        inline static SdExFat     SD; 
        dentry *    file_des[SET_NUM_FILES_LIMIT];
        int 		sd_num_files = 0;
        inline static void        update_card_detect();
        inline static int         card_present = 0;
        int 		sd_setup_flag = 0;
        int         max_filename_len = 20;  // randomly chosen; in bytes
        void        clearDirectory(ExFile dir);
        void        closeFiles();
        //bool        spaceAvailable(int num_bytes);    // can add to write func
};

#endif