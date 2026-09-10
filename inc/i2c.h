#ifndef __I2C_H
#define __I2C_H

#include<stdint.h>


void i2c_setup(void);
void i2c_stop(void);
void i2c_start(void);
void i2c_send_addr(uint8_t);
void i2c_send_byte(uint8_t);
void i2c_read_bytes(uint8_t*, uint8_t);
uint8_t i2c_read_byte();
void i2c_write_byte_to_reg(uint8_t, uint8_t);
#endif