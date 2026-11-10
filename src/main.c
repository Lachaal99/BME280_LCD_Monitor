#include "lcd.h"
#include "i2c.h"
#include "bme280.h"

void float_to_string_baremetal(float val, char* buffer) {
    char* ptr = buffer;

    // 1. Gestion du signe
    if (val < 0.0f) {
        *ptr++ = '-';
        val = -val;
    }

    // 2. Arrondi à 2 décimales avant extraction (évite les troncatures bizarres comme 24.999)
    val += 0.005f;

    // 3. Extraction de la partie entière
    uint32_t int_part = (uint32_t)val;

    // 4. Extraction de la partie décimale (2 chiffres après la virgule)
    float frac_float = val - (float)int_part;
    uint32_t frac_part = (uint32_t)(frac_float * 100.0f);

    // --- CONVERSION DE LA PARTIE ENTIÈRE EN ASCII ---
    // On écrit d'abord les chiffres à l'envers dans un tampon temporaire
    char temp_int[12];
    int i = 0;
    
    if (int_part == 0) {
        temp_int[i++] = '0';
    } else {
        while (int_part > 0) {
            temp_int[i++] = (int_part % 10) + '0';
            int_part /= 10;
        }
    }
    
    // On remet la partie entière à l'endroit dans le buffer final
    while (i > 0) {
        *ptr++ = temp_int[--i];
    }

    // --- AJOUT DU POINT DÉCIMAL ---
    *ptr++ = '.';

    // --- CONVERSION DE LA PARTIE DÉCIMALE EN ASCII ---
    // On doit forcer l'affichage de 2 chiffres (ex: si frac_part vaut 5, on veut ".05" et non ".5")
    *ptr++ = (frac_part / 10) + '0';  // Chiffre des dixièmes
    *ptr++ = (frac_part % 10) + '0';  // Chiffre des centièmes

    // Terminer la chaîne par le caractère nul
    *ptr = '\0';
}

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
       
       // temperature
       float_to_string_baremetal(temperature,buff);
       LCD_clear();
       LCD_println(0,"Temperature :");
       LCD_println(1,buff);
       LCD_SendData(0xdf);
       LCD_print("C");
       delay_ms(500);
       LCD_clear();
       
       //   pressure
       LCD_println(0,"Pressure:");
       float_to_string_baremetal(pressure,buff);
       LCD_println(1,buff);
       LCD_print(" bar");
       delay_ms(500);
       LCD_clear();

       // humidity
       LCD_println(0,"Humidity:");
       float_to_string_baremetal(humidity,buff);
       LCD_println(1,buff);
       LCD_print(" \%");
       delay_ms(500);

    }
    return 0;
}



