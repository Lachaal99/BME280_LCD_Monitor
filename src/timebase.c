#include<stdint.h>
#include<stdbool.h>
#include"system_stm32f4xx.h"
#include"stm32f446xx.h"

static volatile uint32_t counter_ms = 0;


int timebase_init(){
    uint32_t ticks=0;
    // set the reload value for for 1 ms tick ( 180 000-1)
    ticks = SystemCoreClock/1000UL;
    if ( (ticks == 0)|| (ticks > 0x00FFFFFF +1UL)) return (-1);
    
    if(SysTick_Config(ticks) != 0 ) return -1;
    counter_ms =0;
    return 0UL; 
}

uint32_t timebase_get_ms(){
    return counter_ms;
}

void SysTick_Handler(){
    counter_ms++; 
}

bool timebase_timeout_expired(uint32_t start_time, uint32_t timeout){

    uint32_t now = timebase_get_ms();

    return ((uint32_t)(now - start_time) >=timeout);
}