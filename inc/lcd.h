/*
 * lcd.h
 *
 *  Created on: 2 Jul 2026
 *      Author: LACHAAL
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include<stdint.h>


void LCD_Init();
void LCD_clear();
void LCD_SendData(uint8_t cmd);
void LCD_SendCmd( uint8_t cmd);
void LCD_println(uint8_t line, const char * string);
void delay_ms(uint32_t time_ms);

#endif /* INC_LCD_H_ */