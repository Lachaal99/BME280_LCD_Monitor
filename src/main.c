#include "stm32f446xx.h"
#include "lcd.h"
#include "i2c.h"
#include "bme280.h"
#include<stdlib.h>




float pressure, humidity,temperature;
char buff[10];
int main(void) {
    LCD_Init();
    delay_ms(100);
    LCD_println(0,"                 ");
    LCD_println(1,"starting .....  ");
    i2c_setup();
    bme280_setup(0x04,0x04,0x04,0x03,0x01,0x00);

    
    while (1) {
       bme280_read_values(&temperature, &pressure,&humidity);
       LCD_println(0,"Temperature :              ");
       float_to_string(temperature,buff,2);
       LCD_println(1,buff);

    }
    return 0;
}

void float_to_string(float number, char *buffer, uint8_t precision) {
    // 1. Handle negative numbers
    if (number < 0.0f) {
        *buffer++ = '-';
        number = -number;
    }

    // 2. Extract the integer portion
    uint32_t integer_part = (uint32_t)number;
    float fractional_part = number - (float)integer_part;

    // 3. Convert integer portion to string (reverse standard algorithm)
    char temp[12];
    int i = 0;
    
    if (integer_part == 0) {
        temp[i++] = '0';
    } else {
        while (integer_part > 0) {
            temp[i++] = (integer_part % 10) + '0';
            integer_part /= 10;
        }
    }
    
    // Copy reversed integer part into the actual buffer
    while (i > 0) {
        *buffer++ = temp[--i];
    }

    // 4. Handle decimal portion (if precision is requested)
    if (precision > 0) {
        *buffer++ = '.'; // Add decimal point
        
        // Scale the fraction up by 10 for each precision digit
        for (uint8_t p = 0; p < precision; p++) {
            fractional_part *= 10.0f;
            uint32_t digit = (uint32_t)fractional_part;
            *buffer++ = digit + '0';
            fractional_part -= (float)digit;
        }
    }

    // 5. Null-terminate the string
    *buffer = '\0';
}