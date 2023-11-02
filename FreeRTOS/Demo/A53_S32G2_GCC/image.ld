/*
 * Copyright 2022 NXP.
 */

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x00002000;

MEMORY
{
    OsInternalRAM (rwx)  : ORIGIN = 0x34000000, LENGTH = 0x00080000  /* 512K should be enough for sample */
    int_sram_stack_c0    : ORIGIN = 0x34080000, LENGTH = 0x00002000  /* 8KB  */
}

ENTRY(Reset_Handler)
SECTIONS
{
    .startup :
    {
        . = ALIGN(8);
        *(.startup) 
        . = ALIGN(8);
        *(.systeminit)
        . = ALIGN(8);
        *(.text.startup) 
        . = ALIGN(8);
    } > OsInternalRAM
    .interruptTable :
    {
        . = ALIGN(4096);
        KEEP(*(.intc_vector))
        . = ALIGN(2048);
        KEEP(*(.exc_vector))
        . = ALIGN(8);
    } > OsInternalRAM
    .text :
    {
        . = ALIGN(8);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(8);
        _etext = .;        /* define a global symbols at end of code */
    } > OsInternalRAM
    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data : 
    {
        . = ALIGN(8);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(8);
        _edata = .;        /* define a global symbol at data end */
      } > OsInternalRAM

    /* Uninitialized data section */
    . = ALIGN(8);
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(8);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } > OsInternalRAM

    /* heap section */        
    .heap (NOLOAD):
    {
        . += ALIGN(8);
        _end = .;
        end = .;
        _heap_start = .;
        . += HEAP_SIZE;
        _heap_end = .;
    } > OsInternalRAM

    __Stack_start_c0         = ORIGIN(int_sram_stack_c0) + LENGTH(int_sram_stack_c0);
}
