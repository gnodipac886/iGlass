#include "SdFat.h"
#include "sdios.h"
#include "arduino.h"

#ifndef iGLASS_SD_H
#define iGLASS_SD_H

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
// sd card variables
//----------------------------------------------------------------------------------------------------------------------
int chip_select = D10;
int chip_detect = D9;

SdExFat SD;
ExFile mic_file;
ExFile imu_file;

String mic_fname = "mic_data";
String imu_fname = "imu_data";

volatile int card_present = 0;

#endif