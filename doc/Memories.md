# Physical memories

   FlashA      (rx): ORIGIN = 0x1a000000, LENGTH = 0x80000 512K bytes
   FlashB      (rx): ORIGIN = 0x1b000000, LENGTH = 0x80000 512K bytes
   Ram1       (rwx): ORIGIN = 0x10000000, LENGTH = 0x8000  32K bytes
   Ram2       (rwx): ORIGIN = 0x10080000, LENGTH = 0x8000  32K bytes
   Ram2Retain (rwx): ORIGIN = 0x10088000, LENGTH = 0x2000  8K bytes
   RamAHB1    (rwx): ORIGIN = 0x20000000, LENGTH = 0xc000  48K bytes
   RamAHB2ETB (rwx): ORIGIN = 0x2000c000, LENGTH = 0x4000  16K bytes
   SDRAM      (rwx): ORIGIN = 0x28000000, LENGTH = 0x2000000  4 Meg x 16 x 4 banks

# Linking

Toolchain supports two options for linking. The first links program to be loaded directly into RAM by GDB, while the second option first runs from flash until the program is copied into RAM.

Text section of M0 core goes into Ram1.
Text section of M4 core goes into Ram2.

Data section of M0 core goes into AHB1.
Shared data section goes into AHB1.
Data section of M4 core goes into AHB1 and AHB2.


# SPI Flash

SPI flash returns 0xFF if debugger interrupts SPI commands!

id = {mfgId = {0x1, 0x2, 0x19}, extCount = 0x2, extId = {0x4d, 0x1, 0x8, 0x10, 0x1, 0x0, 0x0, 0x0}}

S25FL256S	Device
64 kB		Primary Erase Block Size
01h			RDID Byte 0 Mfg ID
02h			RDID Byte 1 Device ID
19h			RDID Byte 2 Device ID
4Dh			RDID Byte 3 Extended Bytes
01h			RDID Byte 4 Extended Bytes
00h			RDID Byte 5 Reserved
00h			RDID Byte 6 Reserved 

# Patching

Patch editor supports adding, removing and editing effects in processing chain. Editor is based on menu list. For currently active effects, possible actions are remove and edit. Available effects can be added at the end of list. All loadable code should be PIC (position independant).

	Current effects
		OverDrive
		Echo
		Looper
	Available effects
		Echo
		Looper
		OverDrive
		Reverb
		Wah

Memories to manage:
	* DSP data - append into SDRAM, first to allocate because initialization code needs pointer to mydsp structure
	* M4 initialization code (instanceInitmydsp) - load, execute, unload
	* M0 initialization code (buildUserInterfacemydsp) - append to RAM2, cannot unload because effect interface needs rebuilding on each edit
	* M4 processing code (computemydsp) - append into RAM2

Limiters:
	* M4 dsp code size
	* M0 code size
	* dsp data (parameters) size
	* computation time

Extract data from object file with:
	arm-none-eabi-objcopy mydsp_wrap.o outfile --only-section=.text.computemydsp -O binary --gap-fill 0xff
