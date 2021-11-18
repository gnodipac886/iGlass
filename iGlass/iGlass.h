#ifndef iGLASS_H
#define iGLASS_H

#include "iGlass_imu.h"
#include "iGlass_ble.h"
#include "iGlass_unittest.h"

#define DEBUG 							0

#define BLE_IMU_BUF_SIZE              	122 * 1 //122 samples * 1 2-byte value //20 samples* 3 float values// 6 * 9 nine items
#define BLE_TMP_BUF_SIZE                1 * 3
#define BLE_CLR_BUF_SIZE                2 * 5
#define BLE_MIC_BUF_SIZE                (BLE_MAX_SEND_SIZE / 2) * DOWNSAMPLE_RATE

//----------------------------------------------------------------------------------------------------------------------
// STRUCTS
//----------------------------------------------------------------------------------------------------------------------
// typedef struct sensor{
// 	int char_idx;

// } sensor_t




#endif