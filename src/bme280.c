#include"i2c.h"

uint8_t address = 0xFF;


float pRaw, hRaw, tRaw;
// calibration values
uint16_t dig_T1, dig_P1;
int16_t dig_T2, dig_T3 , dig_P2,dig_P3,dig_P4,
                dig_P5,dig_P6,dig_P7,dig_P8,dig_P9,dig_H2,dig_H4,dig_H5;
uint8_t dig_H1,dig_H3;
int8_t dig_H6;

static void read_trim(void){
    uint8_t trimdata[32];
    i2c_start();
    i2c_send_addr(address);
    i2c_send_byte(0x88);
    i2c_read_bytes(trimdata,24);



    
    // Temperature coefficients
	dig_T1 = (uint16_t)(trimdata[1] << 8 | trimdata[0]);
	dig_T2 = (int16_t)(trimdata[3] << 8 | trimdata[2]);
	dig_T3 = (int16_t)(trimdata[5] << 8 | trimdata[4]);

	// Pressure coefficients
	dig_P1 = (uint16_t)(trimdata[7] << 8 | trimdata[6]);
	dig_P2 = (int16_t)(trimdata[9] << 8 | trimdata[8]);
	dig_P3 = (int16_t)(trimdata[11] << 8 | trimdata[10]);
	dig_P4 = (int16_t)(trimdata[13] << 8 | trimdata[12]);
	dig_P5 = (int16_t)(trimdata[15] << 8 | trimdata[14]);
	dig_P6 = (int16_t)(trimdata[17] << 8 | trimdata[16]);
	dig_P7 = (int16_t)(trimdata[19] << 8 | trimdata[18]);
	dig_P8 = (int16_t)(trimdata[21] << 8 | trimdata[20]);
	dig_P9 = (int16_t)(trimdata[23] << 8 | trimdata[22]);

	// Humidity coefficients
	dig_H1 = trimdata[24];
	dig_H2 = (int16_t)(trimdata[26] << 8 | trimdata[25]);
	dig_H3 = trimdata[27];
	dig_H4 = (int16_t)((trimdata[28] << 4) | (trimdata[29] & 0x0F));
	dig_H5 = (int16_t)((trimdata[30] << 4) | (trimdata[29] >> 4));
	dig_H6 = (int8_t)trimdata[31];
}


void bme280_setup(uint8_t osrs_t, uint8_t osrs_h, uint8_t osrs_p , uint8_t mode, uint8_t t_sb , uint8_t filter){

}