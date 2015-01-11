_vStackTop = ORIGIN(VirtDataM0) + LENGTH(VirtDataM0) - 1;

ENTRY(ResetISR)

SECTIONS
{
    .M0_text : ALIGN(4096)
    {
        FILL(0xff)
         __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        . = ALIGN(4) ;
        *(.after_vectors*)
        *(.text*)
    } > VirtTextM0 AT > LoadTextM0

    .M0_data : ALIGN(4)
    {
        *(.data*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > VirtDataM0 AT > LoadDataM0

    /* for exception handling/unwind - some Newlib functions (in common with C++ and STDC++) use this */
    .ARM.extab (NOLOAD) : ALIGN(4)
    {
      *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > LoadDataM0
    __exidx_start = .;

    .ARM.exidx (NOLOAD) : ALIGN(4)
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > LoadDataM0
    __exidx_end = .;

    .M0_bss (NOLOAD) :  ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > VirtDataM0
}
