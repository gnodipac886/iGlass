#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>
#include <PDM.h>

//----------------------------------------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------------------------------------

#define BLE_UUID_TEST_SERVICE         "9A48ECBA-2E92-082F-C079-9E75AAE428B1"
#define BLE_UUID_IMU                  "2713"
#define BLE_UUID_MIC                  "2714"
#define BLE_UUID_TMP                  "2715"
#define BLE_UUID_CLR                  "2716"

#define IMU_BUF_SIZE                  1 * 9
#define TMP_BUF_SIZE                  1 * 3
#define CLR_BUF_SIZE                  2 * 5
#define MIC_BUF_SIZE 				  122

//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------

BLEService testService( BLE_UUID_TEST_SERVICE );
BLECharacteristic IMUCharacteristic( BLE_UUID_IMU, BLERead | BLENotify , IMU_BUF_SIZE * sizeof(float), sizeof(float));
BLECharacteristic TMPCharacteristic( BLE_UUID_TMP, BLERead | BLENotify , TMP_BUF_SIZE * sizeof(float), sizeof(float));
BLECharacteristic CLRCharacteristic( BLE_UUID_CLR, BLERead | BLENotify , CLR_BUF_SIZE * sizeof(int), sizeof(int));
BLECharacteristic MICCharacteristic( BLE_UUID_MIC, BLERead | BLENotify , MIC_BUF_SIZE * sizeof(short), sizeof(short));

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
float   IMU_buf[IMU_BUF_SIZE];
float   TMP_buf[TMP_BUF_SIZE];
int   CLR_buf[TMP_BUF_SIZE];

//----------------------------------------------------------------------------------------------------------------------
// PDM
//----------------------------------------------------------------------------------------------------------------------

// static const char channels = 1;
// static const int frequency = 16000;
// short sampleBuffer[MIC_BUF_SIZE];
// volatile int samplesRead;
// PDMClass PDM(D8, D7, D6);

//---------------------------------------------------------------------------------------------------------------------
// Others
//----------------------------------------------------------------------------------------------------------------------

const int BLE_LED_PIN = LED_BUILTIN;
const int RSSI_LED_PIN = LED_PWR;
BLEDevice central;

//---------------------------------------------------------------------------------------------------------------------
// Setup Functions
//----------------------------------------------------------------------------------------------------------------------
bool setupBleMode(){
  if ( !BLE.begin() ){
    return false;
  }

  // set advertised local name and service UUID:
  BLE.setDeviceName( "NANO_IMU" );
  BLE.setLocalName( "NANO_IMU" );
  BLE.setAdvertisedService( testService );

  // BLE add characteristics
  testService.addCharacteristic( IMUCharacteristic );
  testService.addCharacteristic( MICCharacteristic );
  testService.addCharacteristic( TMPCharacteristic );
  testService.addCharacteristic( CLRCharacteristic );

  // add service
  BLE.addService( testService );

  // set the initial value for the characeristic:
  IMUCharacteristic.writeValue( (int32_t)0.0 );
  MICCharacteristic.writeValue( (int16_t)0 );
  TMPCharacteristic.writeValue( (int32_t)0.0 );
  CLRCharacteristic.writeValue( (int32_t)0);

  BLE.advertise();

  return true;
}

// void setup_IMU(){
//   if ( !IMU.begin() ){
//     Serial.println( "Failed to initialize IMU!" );
//     while ( 1 );
//   }
// }

// void setup_MIC(){
//   PDM.onReceive(onPDMdata);
//   PDM.setBufferSize(MIC_BUF_SIZE * sizeof(short));
//   if (!PDM.begin(channels, frequency)) {
//     Serial.println("Failed to start PDM!");
//     while (1);
//   }
// }

void setup_CLR(){
  APDS.setGestureSensitivity(50);
  if (!APDS.begin()){
    Serial.println("Error initializing APDS9960 sensor.");
    while(1);
  }
  /* As per Arduino_APDS9960.h, 0=100%, 1=150%, 2=200%, 3=300%. Obviously more
   * boost results in more power consumption. */
  APDS.setLEDBoost(0);
}

void setup_TMP(){
  if (!BARO.begin()){
    Serial.println("Failed to initialize barometricPressure sensor!");
    while (1);
  }

  if (!HTS.begin()){
    Serial.println("Failed to initialize humidity temperature sensor!");
    while(1);
  }
}

void ble_setup(){
  if (ble_setup_flag == 1) {return;}
  
  pinMode( BLE_LED_PIN, OUTPUT );
  pinMode( RSSI_LED_PIN, OUTPUT );

  // for power savings we can turn sensors on only when a central connects
  setup_imu();
  // setup_mic();
  setup_CLR();
  setup_TMP();

  Serial.print( "Accelerometer sample rate = " );
  Serial.print( IMU.accelerationSampleRate() );
  Serial.println( " Hz" );

  if( setupBleMode() ){
    digitalWrite( BLE_LED_PIN, HIGH );
  }
  //rgb
  rgb_setColor(0,0,255);
  ble_setup_flag = 1;
}

//---------------------------------------------------------------------------------------------------------------------
// Update Sensor Functions
//----------------------------------------------------------------------------------------------------------------------
void ble_update_IMU() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ble_ax, ble_ay, ble_az);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(ble_wx, ble_wy, ble_wz);
  }

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(ble_mx, ble_my, ble_mz);
  }
}

// void onPDMdata() {
//   // Query the number of available bytes
//   int bytesAvailable = PDM.available();

//   // Read into the sample buffer
//   PDM.read(sampleBuffer, bytesAvailable);

//   // 16-bit, 2 bytes per sample
//   samplesRead = bytesAvailable / 2;
// }

void ble_update_TMP(){
  barometricPressure = BARO.readPressure();
  temperature = HTS.readTemperature();
  humidity = HTS.readHumidity();
}

void ble_update_CLR(){
  if (APDS.proximityAvailable()){
    proximity = APDS.readProximity();
  }

  if (APDS.colorAvailable()){
    APDS.readColor(colorR, colorG, colorB);
  }

  if (APDS.gestureAvailable()){
    gesture = APDS.readGesture();
  }
}

//---------------------------------------------------------------------------------------------------------------------
// Send Sensor Functions
//----------------------------------------------------------------------------------------------------------------------

void ble_send_IMU(){
  float _buf [] = {ble_ax, ble_ay, ble_az, ble_wx, ble_wy, ble_wz, ble_mx, ble_my, ble_mz};
  memcpy(IMU_buf, _buf, 9 * sizeof(float));

  IMUCharacteristic.writeValue(IMU_buf, IMU_BUF_SIZE * sizeof(float));
}

void ble_send_TMP(){
  float _buf [] = {barometricPressure, temperature, humidity};
  memcpy(TMP_buf, _buf, 3 * sizeof(float));
  
  TMPCharacteristic.writeValue(TMP_buf, TMP_BUF_SIZE * sizeof(float));
}

void ble_send_CLR(){
  float _buf [] = {proximity, gesture, colorR, colorG, colorB};
  memcpy(CLR_buf, _buf, 5 * sizeof(float));
  
  CLRCharacteristic.writeValue(CLR_buf, CLR_BUF_SIZE * sizeof(float));
}

// void ble_send_MIC(){
//   if(samplesRead){
//     MICCharacteristic.writeValue(sampleBuffer, int(samplesRead) * sizeof(short));
//   }
// }


//---------------------------------------------------------------------------------------------------------------------
// Main Loop
//---------------------------------------------------------------------------------------------------------------------

void update_ble(){
  static unsigned long counter = 0;
  static long previousMillis = 0;

//   // listen for BLE peripherals to connect:
//   BLEDevice central = BLE.central();

  // listen for BLE peripherals to connect:
  if(!CENTRAL_FLAG){
    central = BLE.central();
    if(central)
      CENTRAL_FLAG = 1;
  }

  if ( CENTRAL_FLAG ){
    // Serial.print( "Connected to central: " );
    // Serial.println( central.address() );

    while ( central.connected() ){
      ble_update_IMU();
      ble_update_TMP();
      ble_update_CLR();
      
      long interval = 100;
      unsigned long currentMillis = millis();
      if( currentMillis - previousMillis > interval && central.connected()){
        previousMillis = currentMillis;
        
          ble_send_IMU();
          ble_send_TMP();
          ble_send_CLR();
      }
	    break;
    }

    // Serial.print( F( "Disconnected from central: " ) );
    // Serial.println( central.address() );
  }
}

//---------------------------------------------------------------------------------------------------------------------
// Stop BLE
//---------------------------------------------------------------------------------------------------------------------

void ble_end() {
  if (ble_setup_flag == 0) {return;}
	BLE.end();
  //rgb
  rgb_off();
	ble_setup_flag = 0;
}
