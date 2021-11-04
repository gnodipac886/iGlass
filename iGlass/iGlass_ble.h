#include <ArduinoBLE.h>

#ifndef iGLASS_SD_H
#define iGLASS_SD_H

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

#define HUMAN_SPEECH_FREQ             1500
#define DOWNSAMPLE_RATE               16000/(HUMAN_SPEECH_FREQ*2)
#define BLE_MAX_SEND_SIZE             244

#define BLE_UUID_TEST_SERVICE         "9A48ECBA-2E92-082F-C079-9E75AAE428B1"
#define BLE_UUID_IMU                  "2713"
#define BLE_UUID_MIC                  "2714"
#define BLE_UUID_TMP                  "2715"
#define BLE_UUID_CLR                  "2716"

#define BLE_IMU_BUF_SIZE              60 * 1 //60 samples * 1 4-byte value //20 samples* 3 float values// 6 * 9 nine items
#define TMP_BUF_SIZE                  1 * 3
#define CLR_BUF_SIZE                  2 * 5
#define MIC_BUF_SIZE                  (BLE_MAX_SEND_SIZE / 2) * DOWNSAMPLE_RATE

//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------

BLEService testService( BLE_UUID_TEST_SERVICE );
BLECharacteristic IMUCharacteristic( BLE_UUID_IMU, BLERead | BLENotify , BLE_IMU_BUF_SIZE * sizeof(float), sizeof(float));
BLECharacteristic TMPCharacteristic( BLE_UUID_TMP, BLERead | BLENotify , TMP_BUF_SIZE * sizeof(float), sizeof(float));
BLECharacteristic CLRCharacteristic( BLE_UUID_CLR, BLERead | BLENotify , CLR_BUF_SIZE * sizeof(int), sizeof(int));
BLECharacteristic MICCharacteristic(BLE_UUID_MIC, BLERead | BLENotify, BLE_MAX_SEND_SIZE);

//----------------------------------------------------------------------------------------------------------------------
// Sensor variables
//----------------------------------------------------------------------------------------------------------------------

float ble_ax, ble_ay, ble_az, ble_wx, ble_wy, ble_wz, ble_mx, ble_my, ble_mz;
float barometricPressure, temperature, humidity;
int proximity, gesture, colorR, colorG, colorB;
int CENTRAL_FLAG = 0;

//----------------------------------------------------------------------------------------------------------------------
// Sensor buffers
//----------------------------------------------------------------------------------------------------------------------
float   ble_IMU_buf[BLE_IMU_BUF_SIZE];
short   ble_MIC_buf[MIC_BUF_SIZE];
short   ble_MIC_TEMP_BUF[MIC_BUF_SIZE / DOWNSAMPLE_RATE];
float   TMP_buf[TMP_BUF_SIZE];
int     CLR_buf[TMP_BUF_SIZE];

//---------------------------------------------------------------------------------------------------------------------
// Others
//----------------------------------------------------------------------------------------------------------------------

const int BLE_LED_PIN = LED_BUILTIN;
const int RSSI_LED_PIN = LED_PWR;
BLEDevice central;
int ble_imu_buf_idx = 0;

#endif