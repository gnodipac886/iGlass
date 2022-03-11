#include "iGlass_rgb.h"

static RGBLed 	_rgb_led = RGBLed(iGLASS_RED_PIN, iGLASS_GREEN_PIN, iGLASS_BLUE_PIN, RGBLed::COMMON_ANODE);

void iGlass_rgb::init(){
    write(0, 0, 0);
}

void iGlass_rgb::end(){
    _rgb_led.off();
}

int iGlass_rgb::read(int buf[3]){
    buf[0] = _red;
    buf[1] = _green;
    buf[2] = _blue;
}

int iGlass_rgb::write(int buf[3]){
    _rgb_led.setColor(buf);
}

int iGlass_rgb::write(int red, int green, int blue){
    _rgb_led.setColor(red, green, blue);
}

RGBLed iGlass_rgb::get_RGBLed(){
    return _rgb_led;
}