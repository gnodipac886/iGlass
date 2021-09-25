const String WRITE_SD = "FB";
const String STOP_WRITE_SD = "BF";
const String SLEEP = "UD";
const String WAKE = "DU";

void stop_write_sd_handler() {
    SD_WRITE_FLAG = 0;
    mic_flush_counter = 0;
    mic_file.flush();
    mic_file.close();
    imu_file.close();
    Serial.println("done saving data to sd file");
    go_to_sleep_handler();
}

void write_sd_handler() {
    Serial.println("Starting SD recording");
    wake_from_sleep_handler();
    SD_WRITE_FLAG = 1;
    setup_sd_card();  
}

void wake_from_sleep_handler() {
    Serial.println("Waking up");
    setup_imu();
    setup_mic();
}


void go_to_sleep_handler() {
    Serial.println("Going to sleep");
    end_mic();
    end_imu();
}


int get_ges_num(String gesture) {
    int value = 0;
    Serial.println("Gesture num: " + gesture);
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
