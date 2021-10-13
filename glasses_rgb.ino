#include <Time.h>

int RED_PIN = 22;
int GREEN_PIN = 23;
int BLUE_PIN = 24;

int RED_VAL;
int GREEN_VAL;
int BLUE_VAL;

int SCALE_TIME = 15;
int scale_pos;
int scale_upwards;

void setup_rgb() {

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
}

void update_rgb()
{
    showColor(int(RED_VAL*scale_pos/SCALE_TIME),int(GREEN_VAL*scale_pos/SCALE_TIME),int(BLUE_VAL*scale_pos/SCALE_TIME));
    
    if (scale_pos == SCALE_TIME) {
        scale_upwards = 0;
    } else if (scale_pos == 0) {
        scale_upwards = 1;
    }
    
    if(scale_upwards) {
        scale_pos += 1;
    } else {
        scale_pos -= 1;
    }
}

//Helper function for update_rgb
void showColor(int red, int green, int blue) {
    analogWrite(RED_PIN, 255-red);
    analogWrite(GREEN_PIN, 255-green);
    analogWrite(BLUE_PIN, 255-blue);
}

void rgb_setColor(int red, int green, int blue)
{
    RED_VAL = red;
    GREEN_VAL = green;
    BLUE_VAL = blue;

    scale_pos = 0;
    scale_upwards = 1;

    //setTime(0);
    //rgb_timer.resume();
    rgb_timer = millis();
}

void rgb_off() {
    showColor(0,0,0);
}
