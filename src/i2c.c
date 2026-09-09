#include"stm32f446xx.h"

extern uint8_t ADDRESS;

void i2c_setup(void){
    // clock enabling
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; //i2c1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // port b clock enable

    //=========GPIO config PB7 PB6===============
    //moder register AF 
    GPIOB->MODER &= ~(0b11<<(7*2));
    GPIOB->MODER |= 0b10<<(7*2) ;
    GPIOB->MODER &= ~(0b11<<(6*2));
    GPIOB->MODER |= 0b10<<(6*2);
    //otyper open drain 
    GPIOB->OTYPER |= GPIO_OTYPER_OT7 | GPIO_OTYPER_OT6 ;
    //ospeed high speed
    GPIOB->OSPEEDR |= (0b1111 << 12); //7 and 6 at the same time
    //pupdr both pull up
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD6 ); //clear
    GPIOB->PUPDR |= (0x1U <<12 | 0x1U <<14);
    //alternate function config for i2c 
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7); // clear
    GPIOB->AFR[0] |= (GPIO_AFRL_AFRL7_2 | GPIO_AFRL_AFRL6_2); // AF4
    //================== i2c reset ===================
    I2C1->CR1 |= I2C_CR1_SWRST; // put it into reset mode 
    I2C1->CR1 &= ~(I2C_CR1_SWRST);// set it again

    //======================== i2c clock configuration======================
    // disable the peripheral
    I2C1->CR1 &= ~(I2C_CR1_PE);
    // enter the peripheral clock frequency 45MHz
    I2C1->CR2 &= ~(I2C_CR2_FREQ);
    I2C1->CR2 |= 0x2D;
    //ccr register config
    I2C1->CCR =225;
    // TRISE
    I2C1->TRISE= 46;
    // =================== enable the i2c =============
    I2C1->CR1 |= I2C_CR1_PE;
}

//start condition generation
void i2c_start(void){
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
}
// stop confition generation
void i2c_stop(void){
    I2C1->CR1 |= I2C_CR1_STOP;
    while(!(I2C1->SR1 & I2C_SR1_STOPF));
}


// function for the address send handling
void i2c_send_addr(uint8_t address){
    I2C1->DR = address;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}



void i2c_send_byte(uint8_t byte){
    I2C1->DR = byte;
    while(!(I2C1->SR1 & I2C_SR1_BTF));
}

void i2c_read_bytes(uint8_t* buffer , uint8_t size){

    //set the acknowledgement bit
    I2C1->CR1 |= I2C_CR1_ACK;
    i2c_start();
    i2c_send_addr(ADDRESS | 0x01);
    uint8_t remaining = size;
    
    while(remaining> 1){
        while(!(I2C1->SR1 & SPI_SR_RXNE));
        buffer[size-remaining] = I2C1->DR;
        remaining--;
    }

    // turn off the ack
    I2C1->CR1 = ~(I2C_CR1_ACK);
    i2c_stop();

    while(!(I2C1->SR1 & SPI_SR_RXNE));
    buffer[size-remaining] = I2C1->DR;

}