#include "stm32f446xx.h"

int main(void) {
    // Test 1: Peripheral register access via stm32f446xx.h
    // Enable GPIOA Clock (AHB1 Bus)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // Set PA5 (LD2 Built-in LED on NUCLEO-F446RE) as Output
    GPIOA->MODER &= ~(3U << (5 * 2)); // Clear mode bits
    GPIOA->MODER |=  (1U << (5 * 2)); // Set mode to 01 (Output)

    // Test 2: Cortex-M4 Core Register access via core_cm4.h
    // Configure SysTick for 1ms interrupts at 180 MHz SYSCLK
    SysTick->LOAD = (180000000U / 1000U) - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    while (1) {
        // Toggle PA5 using Atomic Bit Set/Reset Register
        GPIOA->BSRR = (1<<5);
        for (volatile int i = 0; i < 1000000; i++);
        
        GPIOA->BSRR = ((1<<5) << 16);
        for (volatile int i = 0; i < 1000000; i++);
    }

    return 0;
}