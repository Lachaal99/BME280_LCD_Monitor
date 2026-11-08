/*
 * lcd.c
 *
 *  Created on: 2 Jul 2026
 *      Author: LACHAAL
 */
#include"stm32f446xx.h"

#define BIT_MASK(n)			(~(0x03<<2*(n))) // generate a mask for the specified control bits
#define BIT_PUSH(bin,n)		((bin)<< 2*(n)) // push the needed control sequence to the position
#define CALIBRATION_MS      (180000)// for a clock cycle of 180 MHZ

// pin definition you can change these  pins
#define RS    5
#define RW    6
#define E     7
#define D7    9
#define D6    8
#define D5    12
#define D4    11

static void delay(uint16_t clock_cycles) {
	while (clock_cycles--)
		__asm__ volatile("nop");
}

void delay_ms(uint32_t time_ms){
	uint32_t cycles = time_ms * CALIBRATION_MS;
	while(cycles--){
		__asm__ volatile("nop");
	}
}

static void LCD_PulseEnable() {
	GPIOA->BSRR = (1 << E);
	delay(100);
	GPIOA->BSRR = (1 << (E + 16));
	delay(100);
}


static void LCD_ReadBusy() {
	// change the data pins to input mode
	//D7 - D4
	GPIOA->MODER &= (BIT_MASK(D7)) & (BIT_MASK(D6))
			& (BIT_MASK(D5)) & (BIT_MASK(D4));
	uint8_t state = 1;
	// set rs and rw to read
	GPIOA->BSRR = (1 << (RS + 16)) | (1 << RW);
	delay(10);

	while (state) {
		GPIOA->BSRR = (1 << E);
		delay(100);
		state = !!(GPIOA->IDR & 1 << D7);
		GPIOA->BSRR = (1 << (E + 16));
		delay(100);
		//max_ticks--;
		//perform the other read enable without storing anything just to clear things
		LCD_PulseEnable();
	}
	//reset the data pins to output
	//the MODER register contains 00 for each pin
	GPIOA->MODER |= (0x01 << (D7 * 2)) | (0x01 << (D6 * 2))
			| (0x01 << (D5 * 2)) | (0x01 << (D4 * 2));
}


static void LCD_SendNibble(uint8_t cmd) {
	// set all bits to 0
	GPIOA->BSRR = (1 << (D7 + 16)) | (1 << (D6 + 16))
			| (1 << (D5 + 16)) | (1 << (D4 + 16));
	uint32_t set_mask = 0;
	if (cmd & 0x08)
		set_mask |= (1 << D7);
	if (cmd & 0x04)
		set_mask |= (1 << D6);
	if (cmd & 0x02)
		set_mask |= (1 << D5);
	if (cmd & 0x01)
		set_mask |= (1 << D4);
	GPIOA->BSRR = set_mask;
	LCD_PulseEnable();
}

void LCD_SendCmd(uint8_t cmd) {
	// RS=0 and RW= 0
	GPIOA->BSRR = (1 <<(16 + RW)) | (1 << (16 + RS));
	uint8_t first_nibble = (cmd >> 4 & 0x0F);
	uint8_t second_nibble = (0x0f & cmd);
	LCD_SendNibble(first_nibble);
	LCD_SendNibble(second_nibble);
	LCD_ReadBusy();

}

void LCD_SendData( uint8_t cmd){
	// RS=1 and RW= 0
	GPIOA->BSRR = (1 <<(16 + RW)) | (1 <<  RS);
	uint8_t first_nibble = (cmd >> 4 & 0x0F);
	uint8_t second_nibble = (0x0f & cmd);
	LCD_SendNibble(first_nibble);
	LCD_SendNibble(second_nibble);
	LCD_ReadBusy();
}

void LCD_print(const char * string){
	while( *string != '\0'){
		LCD_SendData(*string);
		string++;
	}
}
void LCD_println(uint8_t line, const char * string){
	if(line == 0 ){
		// set the cursor to the first line
		LCD_SendCmd(0x80);
		while( *string != '\0'){
			LCD_SendData(*string);
			string++;
		}
	}
	else {
		// set the cursor to the first line
		LCD_SendCmd(0xC0);
		while( *string != '\0'){
			LCD_SendData(*string);
			string++;
		}

	}
}

void LCD_Init() {
    // GPIO settings 
    RCC->AHB1ENR |= (1U<< 0);
    GPIOA->MODER |= (1U<<(RS*2)) | (1U<<(RW*2)) | (1U<<(E*2)) |  (1U<<(D7*2)) | (1U<<(D6*2)) | (1U<<(D5*2)) | (1U<<(D4*2));
	// initialization sequence
	// set the rs and rw to zeor always
	GPIOA->BSRR = (1 << (RS + 16)) | (1 << (RW + 16));
	
    
    // force 8bit mode reset
	//1. first hard reset
	LCD_SendNibble(0x03);
	delay_ms(5);
	// 2. second hard reset
	LCD_SendNibble(0x03);
	delay_ms(1);
	//3. third reset
	LCD_SendNibble(0x03);
	delay_ms(1);
	// set in 4 bit mode
	LCD_SendNibble(0x02);
	delay_ms(1);

	// Function Set: 4-bit mode, 2-line display, 5x8 font character map
	LCD_SendCmd(0x28);
	LCD_SendCmd(0x08);
	LCD_SendCmd(0x01); //clear screen
	LCD_SendCmd(0x06);//set automatic right shift
	//Display Control: Display ON, Cursor OFF, Blink OFF
	LCD_SendCmd(0x0E);

	delay_ms(1);
	LCD_println(0, "Init success !!");
	LCD_println(1, "waiting for orders");



}

void LCD_clear(){
	LCD_SendCmd(0x01);
}