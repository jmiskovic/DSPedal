_vStackTop = ORIGIN(VirtDataM4) + LENGTH(VirtDataM4) - 1;
_vShadowMapM4 = __CopyTextM4;
_vShadowMapM0 = __CopyTextM0;

_ResetISR_load = ResetISR;

ENTRY(_ResetISR_load)

SECTIONS
{
    . = ORIGIN(VirtTextM4);

    .text_M4 : ALIGN(4)
    {
        FILL(0xff)
         __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        . = ALIGN(4) ;
        /* markers for initialising data and bss */
        __section_table_start = .;
        __text_section_table = .;
        LONG(LOADADDR(.text_M4));
        LONG(__CopyTextM4);
        LONG(  SIZEOF(.text_M4));
        LONG(LOADADDR(.text_M0));
        LONG(__CopyTextM0);
        LONG(  SIZEOF(.text_M0));
        __text_section_table_end = .;
        __data_section_table = .;
        LONG(LOADADDR(.data_M4));
        LONG(    ADDR(.data_M4));
        LONG(  SIZEOF(.data_M4));
        LONG(LOADADDR(.data_M0));
        LONG(    ADDR(.data_M0));
        LONG(  SIZEOF(.data_M0));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss_M4));
        LONG(  SIZEOF(.bss_M4));
        LONG(    ADDR(.bss_M0));
        LONG(  SIZEOF(.bss_M0));
        __bss_section_table_end = .;
        __section_table_end = . ;
        *(.after_vectors*)
        *(.text*)
    } > VirtTextM4 AT > LoadTextM4

    . = ORIGIN(VirtDataM4);

    .data_M4 : ALIGN(4)
    {
        *(.data*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > VirtDataM4 AT > LoadDataM4

    /* for exception handling/unwind - some Newlib functions (in common with C++ and STDC++) use this */
    .ARM.extab (NOLOAD) : ALIGN(4)
    {
      *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > VirtDataM4
    __exidx_start = .;

    .ARM.exidx (NOLOAD) : ALIGN(4)
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > VirtDataM4
    __exidx_end = .;

    .bss_M4 (NOLOAD) :  ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > VirtDataM4

    . = ORIGIN(VirtTextM0);

    .text_M0 : ALIGN(4)
    {
       FILL(0xff)
       __core_m0app_START__ = .; /* start of slave image */
       KEEP(*(.M0_text))
       __core_m0app_END__ = .; /* end of slave image */
       ASSERT(!(__core_m0app_START__ == __core_m0app_END__), "No slave code for _core_m0app");
       ASSERT( (ABSOLUTE(__core_m0app_START__) == __vectors_start_m0), "M0APP execute address differs from address provided in source image");
    } > VirtTextM0 AT > LoadTextM0

    . = ORIGIN(VirtDataM0);

    .data_M0 : ALIGN(4)
    {
        KEEP(*(.M0_data))
    } > VirtDataM0 AT > LoadDataM0

    .bss_M0 (NOLOAD): ALIGN(4)
    {
        KEEP(*(.M0_bss))
    } > VirtDataM0
}
