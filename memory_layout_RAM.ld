/* Physical memories: */
/*    FlashA      (rx): ORIGIN = 0x1a000000, LENGTH = 0x80000 512K bytes */
/*    FlashB      (rx): ORIGIN = 0x1b000000, LENGTH = 0x80000 512K bytes */
/*    Ram1       (rwx): ORIGIN = 0x10000000, LENGTH = 0x8000  32K bytes */
/*    Ram2       (rwx): ORIGIN = 0x10080000, LENGTH = 0x8000  32K bytes */
/*    Ram2Retain (rwx): ORIGIN = 0x10088000, LENGTH = 0x2000  8K bytes */
/*
/*    RamAHB1    (rwx): ORIGIN = 0x20000000, LENGTH = 0xc000  48K bytes */
/*    RamAHB2ETB (rwx): ORIGIN = 0x2000c000, LENGTH = 0x4000  16K bytes */
/*    SDRAM      (rwx): ORIGIN = 0x28000000, LENGTH = 0x2000000  4 Meg x 16 x 4 banks */

MEMORY
{
    VirtTextM4    (rx): ORIGIN = 0x10080000, LENGTH = 0xA000 /* 40k -> Shadow Mem */
    VirtTextM0    (rx): ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32k -> Shadow Mem */

    LoadTextM4    (rx): ORIGIN = 0x10080000, LENGTH = 0xA000 /* 40k -> FlashA */
    LoadTextM0    (rx): ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32k -> FlashB */

    VirtDataM4    (rw): ORIGIN = 0x20008000, LENGTH = 0x8000 /* 32k -> RAM AHB */
    VirtDataM0    (rw): ORIGIN = 0x20000000, LENGTH = 0x7000 /* 28k -> RAM AHB */

    LoadDataM4    (rw): ORIGIN = 0x20008000, LENGTH = 0x8000 /* 32k -> FlashA */
    LoadDataM0    (rw): ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k -> FlashB */

    SharedData    (rw): ORIGIN = 0x20007000, LENGTH = 0x1000 /* 4k -> RAM AHB */

    SDRAM        (rwx): ORIGIN = 0x28000000, LENGTH = 0x2000000 /* 32M */
}

/*    CopyTextM4  (!rwx): ORIGIN = 0x10080000, LENGTH = 0xA000  40k -> SRAM 2 */
/*    CopyTextM0  (!rwx): ORIGIN = 0x10000000, LENGTH = 0x8000  32k -> SRAM 1 */

__CopyTextM4 = 0x10080000;
__CopyTextM0 = 0x10000000;