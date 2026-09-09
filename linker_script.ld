MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{

    .isr_vector :
    {
            . = ALIGN(4);
            KEEP(*(.isr_vector))
    } > FLASH
    
    .text :
    {
        *(.text)
        *(.text*)
    } > FLASH

    .rodata :
    {
        *(.rodata)
        *(.rodata*)
    } > FLASH

    .data :
    {   
        . = ALIGN(4);
        _sdata = . ;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = . ;

    } > SRAM AT> FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > SRAM

    _estack = ORIGIN(SRAM) + LENGTH(SRAM);
}