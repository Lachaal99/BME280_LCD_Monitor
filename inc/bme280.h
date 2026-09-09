#ifndef __BME280_H
#define __BME280_H
#include<stdint.h>

void bme280_setup(uint8_t osrs_t, uint8_t osrs_h, uint8_t osrs_p , uint8_t mode, uint8_t t_sb , uint8_t filter);
float* read_sensor_values(void);

#endif