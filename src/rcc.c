#include "stm32f446xx.h"

void SystemInit(void) {
    // 1. Enable HSE (High-Speed External Crystal)
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Enable Power Controller clock & set Voltage Scale 1 (required for 180 MHz)
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    // 3. Configure Flash prefetch, instruction/data cache, and 5 wait states (WS)
    FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    // 4. Configure Bus Prescalers: HCLK = SYSCLK/1 (180MHz), PCLK1 = HCLK/4 (45MHz max), PCLK2 = HCLK/2 (90MHz max)
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;

    // 5. Configure Main PLL: VCO_in = 8MHz / M(8) = 1MHz; VCO_out = 1MHz * N(360) = 360MHz
    // SYSCLK = 360MHz / P(2) = 180MHz; USB/SDIO = 360MHz / Q(7) = 51.4MHz
    RCC->PLLCFGR = (8 << RCC_PLLCFGR_PLLM_Pos)      |
                   (360 << RCC_PLLCFGR_PLLN_Pos)    |
                   (0 << RCC_PLLCFGR_PLLP_Pos)      | // 00: PLLP = 2
                   RCC_PLLCFGR_PLLSRC_HSE           |
                   (7 << RCC_PLLCFGR_PLLQ_Pos);

    // 6. Enable PLL and wait for lock
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 7. Enable Over-Drive Mode (Required for speeds > 168 MHz on STM32F446)
    PWR->CR |= PWR_CR_ODEN;
    while (!(PWR->CSR & PWR_CSR_ODRDY));
    PWR->CR |= PWR_CR_ODSWEN;
    while (!(PWR->CSR & PWR_CSR_ODSWRDY));

    // 8. Select PLL as System Clock Source and wait for switch confirmation
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);
}