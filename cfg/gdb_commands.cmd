define print_memmap
    p/x ((LPC_CREG_T *) 0x40043000)->MXMEMMAP
end

define connect_openocd
    if $argc == 0
        target remote localhost:3333
    end
        target remote localhost:$arg0
end

define reset_to_flash
    monitor reset
    monitor halt
    set $pc = ResetISR
    p/x LPC_CREG->MXMEMMAP = 0x1a000000
end

define unprotect_flash
    monitor flash protect 0 0 last off
    monitor flash protect 1 0 last off
end

define wipe_flash
    monitor flash erase_sector 0 0 last
    monitor flash erase_sector 1 0 last
end