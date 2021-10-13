const String WRITE_SD = "FB";
const String STOP_WRITE_SD = "BF";
const String SLEEP = "UD";
const String WAKE = "DU";

void stop_write_sd_handler() {
    if (SD_WRITE_FLAG == 0) {return;}
    SD_WRITE_FLAG = 0;
    mic_flush_counter = 0;
    mic_file.flush();
    mic_file.close();
    imu_file.close();
    #if DEBUG
    Serial.println("done saving data to sd file");
    #endif
    //rgb
    rgb_off();
    go_to_sleep_handler();
}

void write_sd_handler() {

    if (SD_WRITE_FLAG == 1) {return;}
    #if DEBUG
    Serial.println("Starting SD recording");
    #endif
    wake_from_sleep_handler();
    SD_WRITE_FLAG = 1;
    setup_sd_card();  
}

void wake_from_sleep_handler() {
    #if DEBUG
    Serial.println("Waking up");
    #endif
    setup_imu();
    setup_mic();
}


void go_to_sleep_handler() {
    #if DEBUG
    Serial.println("Going to sleep");
    #endif
    end_mic();
    Serial.println("End mic");
    end_imu();
    Serial.println("End imu");
    //rgb
    rgb_off();
}

void ble_start_handler() {
    #if DEBUG
    Serial.println("Starting BLE...");
    #endif
    ble_setup();
    #if DEBUG
    Serial.println("BLE setup complete");
    #endif
}

void ble_end_handler() {
    #if DEBUG
    Serial.println("Ending BLE...");
    #endif
    ble_end();
    #if DEBUG
    Serial.println("BLE ended");
    #endif
}

int get_ges_num(String gesture) {
    int value = 0;
    #if DEBUG
    Serial.println("Gesture num: " + gesture);
    #endif
    for (int i = 0; i < gesture.length(); i++) {
        if (gesture.charAt(i) == 'F') {
            value *= 10;
            value += 0;
        } else if (gesture.charAt(i) == 'B') {
            value *= 10;
            value += 1;
        } else if (gesture.charAt(i) == 'U') {
            value *= 10;
            value += 2;
        } else if (gesture.charAt(i) == 'D') {
            value *= 10;
            value += 3;
        }
    }
    return value;
}
