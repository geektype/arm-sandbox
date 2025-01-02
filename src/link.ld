ENTRY(_start)
SECTIONS
{
    . = 0x40200000;
    .text . : { *(.text) *(.text.*)}
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    .data : {
        *(.data)
        *(.data.*)
    }
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    . = ALIGN(8);
    . = . + 0x1000;
    stack_top = .;
}
