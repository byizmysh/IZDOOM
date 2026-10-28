ENTRY(_start)
SECTIONS {
 . = 0x10000;
 .text : { KEEP(*(.text.entry)) *(.text .text.*) }
 .rodata : { *(.rodata .rodata.*) }
 .data : { *(.data .data.*) *(.sdata .sdata.*) }
 .bss : { *(.bss .bss.*) *(.sbss .sbss.*) *(COMMON) }
 _end = .;
 ASSERT(_end < 0x00100000, "Image overlaps WAD")
 /DISCARD/ : { *(.comment) *(.eh_frame*) *(.riscv.attributes) }
}
