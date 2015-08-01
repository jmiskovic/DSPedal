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

define armex
  printf "EXEC_RETURN (LR):\n",
  info registers $lr
    if $lr & 0x4 == 0x4
    printf "Uses MSP 0x%x return.\n", $MSP
    set $armex_base = $MSP
    else
    printf "Uses PSP 0x%x return.\n", $PSP
    set $armex_base = $PSP
    end
  
    printf "xPSR            0x%x\n", *($armex_base+28)
    printf "ReturnAddress   0x%x\n", *($armex_base+24)
    printf "LR (R14)        0x%x\n", *($armex_base+20)
    printf "R12             0x%x\n", *($armex_base+16)
    printf "R3              0x%x\n", *($armex_base+12)
    printf "R2              0x%x\n", *($armex_base+8)
    printf "R1              0x%x\n", *($armex_base+4)
    printf "R0              0x%x\n", *($armex_base)
    printf "Return instruction:\n"
    x/i *($armex_base+24)
    printf "LR instruction:\n"
    x/i *($armex_base+20)
end
  
document armex
ARMv7 Exception entry behavior.
xPSR, ReturnAddress, LR (R14), R12, R3, R2, R1, and R0
end