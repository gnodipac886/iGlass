#ifndef iGLASS_SD_H
#define iGLASS_SD_H

#include "SdFat.h"
#include "sdios.h"
#include "arduino.h"
#include <iostream>
#include <math.h>
#include "iGlass_macros.h"
using namespace std;//...........double check if needed in the future

//----------------------------------------------------------------------------------------------------------------------
// defines for sd_fat class...............double check in the future
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
#define SD_DATA_BUFFER_BYTE_LIMIT   512    //for reads and writes???? or for writes only.....check in the future 
#define MIN_SDSC_CAPACITY           999967  //1 MB...........double check, and check what happens if we write to a full SD card in the future...
#define CHIP_DETECT                 D9  
#define CHIP_SELECT                 D10
#define NUM_FILES_LIMIT				10		// randomly chosen......may change in the future		
#define MAX_FILENAME_LEN			20     	// randomly chosen; in bytes......may change in the future

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
        iGlass_sd(int sd_capacity = 32000000000): sd_capacity(sd_capacity) {}
        int 		init();    
        void 		end();   
		int 		write(int f_idx, byte * buf, int buf_size); 
        int         read(int f_idx, byte * buf, int buf_size);   
		int 		addNewFile(char * fname); 

    private:
        ExFile      			iGlass_dir; 
        char *      			iGlass_dirname = "iGlass_data"; 
        inline static SdExFat	SD; 
        int 					sd_setup_flag = 0;
        int					  	sd_capacity; // in bytes........maybe try to use this in some way in the future
        dentry *    			file_des[10/*randomly set num files limit (for now).....may change in the future*/];
        int 					sd_num_files = 0;
        inline static void     	update_card_detect();
        inline static bool     	card_present = 0;       // 1 when SD card is detected (in SD card slot)
        void        			clearDirectory();
        void        			closeFiles();
        int                     openAddedFile(int f_num);
        //bool        spaceAvailable(int num_bytes);    // can add to write func.......in the future
};

#endif