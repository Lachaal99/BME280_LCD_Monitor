#ifndef TIMEBASE_H
#define TIMEBASE_H
#include<stdint.h>

typedef enum{
    I2C_STATUS_OK = 0,
    I2C_STATUS_TIMEOUT,
    I2C_STATUS_BUS_ERROR,
    I2C_STATUS_ARBITRATION_LOSS,
    I2C_STATUS_OVERRUN
} I2C_status_t;



int timebase_init(void);
uint32_t timebase_get_ms(void);
bool timebase_timeout_expired(uint32_t start_time, uint32_t timeout);


#endif