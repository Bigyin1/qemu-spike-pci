OUTPUT_FORMAT("elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x40000000;
    
    .text : {
        *(.text)
    }
    
    . = ALIGN(8);
    .data : {
        *(.data)
    }
    
    .bss : { 
        *(.bss) 
    }

    . = ALIGN(16);
    _stack_bottom = .;
    . += 4K;
    _stack_top = .;

    . = ALIGN(8);
    tohost = .;
    fromhost = . + 8;
}
