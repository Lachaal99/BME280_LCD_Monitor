#include "i2c.h"

uint8_t address = 0x77;
static int32_t t_fine;

int32_t pRaw, hRaw, tRaw;
// calibration values
uint16_t dig_T1, dig_P1;
int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4,
	dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2, dig_H4, dig_H5;
uint8_t dig_H1, dig_H3;
int8_t dig_H6;

static void read_trim(void)
{
	uint8_t trimdata[32];
	// read the first 24 consequtive data
	i2c_start();
	i2c_send_addr(address << 1);
	i2c_send_byte(0x88);
	i2c_read_bytes(trimdata, 24);

	// read the 0xA1 data
	i2c_start();
	i2c_send_addr(address << 1);
	i2c_send_byte(0xA1);
	trimdata[24] = i2c_read_byte();
	// read the rest of bytes
	i2c_start();
	i2c_send_addr(address << 1);
	i2c_send_byte(0xE1);
	i2c_read_bytes(trimdata + 25, 7);
	// ==================extarcting coefficients========================
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
	dig_H4 = (int16_t)(((int16_t)(int8_t)trimdata[28] << 4) | (trimdata[29] & 0x0F));
	dig_H5 = (int16_t)(((int16_t)(int8_t)trimdata[30] << 4) | (trimdata[29] >> 4));

	dig_H6 = (int8_t)trimdata[31];
}

void bme280_setup(uint8_t osrs_t, uint8_t osrs_h, uint8_t osrs_p, uint8_t mode, uint8_t t_sb, uint8_t filter)
{	
	read_trim();
	uint8_t datatosend;
	// sending the osrs_h 0xf2
	datatosend = osrs_h;
	i2c_write_byte_to_reg(0xF2, datatosend);
	// sending osrs_t, osrs_p, mode
	datatosend = (osrs_t << 5) | (osrs_p << 2) | mode;
	i2c_write_byte_to_reg(0xF4, datatosend);
	// sending t_sb and filter
	datatosend = (t_sb << 5) | (filter << 2);
	i2c_write_byte_to_reg(0xF5, datatosend);
}

static void bme280_read_raw(void)
{
	uint8_t rawData[8];
	// read data in burst
	i2c_start();
	i2c_send_addr(address << 1);
	// start of the raw data address
	i2c_send_byte(0xF7);
	i2c_read_bytes(rawData, 8);

	pRaw = (rawData[0] << 12) | (rawData[1] << 4) | (rawData[2] >> 4);
	tRaw = (rawData[3] << 12) | (rawData[4] << 4) | (rawData[5] >> 4);
	hRaw = (rawData[6] << 8) | (rawData[7]);
}

static int32_t BME280_compensate_T_int32(int32_t adc_T)
{
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *((int32_t)dig_T3)) >>14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

static uint32_t BME280_compensate_P_int32(int32_t adc_P)
{
	int32_t var1, var2;
	uint32_t p;
	var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)dig_P6);
	var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
	var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
	var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)dig_P2) *
																	  var1) >>
																	 1)) >>
		18;
	var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}

	p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;

	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
	var2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}

static uint32_t bme280_compensate_H_int32(int32_t adc_H)
{
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
																																	 ((int32_t)dig_H6)) >>
																																	10) *
																																   (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
																																	((int32_t)32768))) >>
																																  10) +
																																 ((int32_t)2097152)) *
																																	((int32_t)dig_H2) +
																																8192) >>
																															   14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
							   ((int32_t)dig_H1)) >>
							  4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r >> 12);
}

void bme280_read_values(float* temperature, float* pressure, float* humidity)
{

	bme280_read_raw();

	*temperature = (BME280_compensate_T_int32(tRaw))/100.0f;
	*pressure = (BME280_compensate_P_int32(pRaw))/100.0f;
	*humidity = (bme280_compensate_H_int32(hRaw))/1024.0f;
}

static uint8_t bme280_single_read(uint8_t addr){
	volatile uint8_t data;
	
	i2c_start();
	i2c_send_addr(address << 1);
	i2c_send_byte(addr);
	data = i2c_read_byte();
	return data;
}


void bme280_helper(){
	volatile uint8_t array[26];
	for (uint8_t  i=0; i<26; i++){
		array[i] = bme280_single_read(0xD0);
	}
}