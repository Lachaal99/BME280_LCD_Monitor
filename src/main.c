#include "stm32f446xx.h"
#include "i2c.h"
#include "bme280.h"


float pressure, humidity,temperature;
int main(void) {
    i2c_setup();
    bme280_setup(0x04,0x04,0x04,0x03,0x01,0x00);
    while (1) {
       bme280_helper();
    }
    return 0;
}