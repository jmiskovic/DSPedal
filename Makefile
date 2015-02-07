PROJECT = DSPedal

# comment following line for verbose building
Q := @

GCCPATH = ../../gcc-arm-none-eabi-4_8-2014q3
FAUSTPATH = ../../faustcore/
BUILD_DIR = out

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
GDB = arm-none-eabi-gdb
SIZE = arm-none-eabi-size
FAUST = $(FAUSTPATH)/compiler/faust

LINKERSCRIPT_M4 = linking.ld
LINKERSCRIPT_M0 = linking_M0.ld

LINKERSCRIPT_FLASH = memory_layout.ld
LINKERSCRIPT_RAM   = memory_layout_RAM.ld

CORELIB = ../Core

BUILD_FOR_DUALCORE = 0

## Compiler flags
CFLAGS = -mthumb -ffunction-sections -fmessage-length=0  -fno-stack-protector -fdata-sections -fsingle-precision-constant -fno-common
CFLAGS+= -D__USE_LPCOPEN -D__GNU_ARM
CFLAGS+= -MD -std=c99 -Wall #-pedantic
CFLAGS+= -O0 -g3 #-DDEBUG -DDEBUG_ENABLE
CFLAGS+= --specs=nano.specs --specs=rdimon.specs

CLIBS = -lc -lm

CFLAGS_M4 = -DCORE_M4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -DBOOT_M0 #-DDEBUG_SEMIHOSTING
CFLAGS_M0 = -DCORE_M0 -mcpu=cortex-m0 #-DDEBUG_SHAREDMEM

CFLAGS_DSP = -g0 -O3 -funroll-loops --param max-unroll-times=200

## Assembler flags
ASFLAGS = -c -x assembler-with-cpp

INCLUDES = -I $(CORELIB)/Device/NXP/LPC43xx/Include
INCLUDES+= -I $(CORELIB)/CMSIS/Include
INCLUDES+= -I src
INCLUDES+= -I faust_dsp
INCLUDES+= -I lpcopen

INCLUDES_M4 = -I lpcopen/lpc_chip_43xx_M4/inc
INCLUDES_M4+= -I lpcopen/lpc_chip_43xx_M4/inc/usbd

INCLUDES_M0 = -I lpcopen/lpc_chip_43xx_M0/inc
INCLUDES_M0+= -I lpcopen/lpc_chip_43xx_M0/inc/usbd

OC_RENAMES = --redefine-sym __vectors_start__=__vectors_start_m0 --keep-symbol __vectors_start_m0 --keep-symbol __bss_section_table_M0 --keep-symbol __bss_section_table_end_M0

OBJECTS = 	$(BUILD_DIR)/startup.o \
			$(BUILD_DIR)/sysinit.o \
			$(BUILD_DIR)/board.o \
			$(BUILD_DIR)/board_sysinit.o \
			$(BUILD_DIR)/uda1380.o \
			$(BUILD_DIR)/sysinit_18xx_43xx.o \
			$(BUILD_DIR)/chip_18xx_43xx.o \
			$(BUILD_DIR)/clock_18xx_43xx.o \
			$(BUILD_DIR)/gpio_18xx_43xx.o \
			$(BUILD_DIR)/rgu_18xx_43xx.o \
			$(BUILD_DIR)/emc_18xx_43xx.o \
			$(BUILD_DIR)/i2c_18xx_43xx.o \
			$(BUILD_DIR)/i2s_18xx_43xx.o \
			$(BUILD_DIR)/cdc_vcom.o \
			$(BUILD_DIR)/cdc_desc.o \
			$(BUILD_DIR)/fpu_init.o \
			$(BUILD_DIR)/gpdma_18xx_43xx.o \
			$(BUILD_DIR)/main.o \
			$(BUILD_DIR)/mem_tests.o \
			$(BUILD_DIR)/sound.o \
			$(BUILD_DIR)/mydsp_wrap.o \
#			$(BUILD_DIR)/uart_18xx_43xx.o \
#			$(BUILD_DIR)/i2s.o \

OBJECTS_M0 = 	$(BUILD_DIR)/main_M0.o \
				$(BUILD_DIR)/startup_M0.o \
				$(BUILD_DIR)/chip_18xx_43xx_M0.o \
				$(BUILD_DIR)/clock_18xx_43xx_M0.o \
				$(BUILD_DIR)/gpio_18xx_43xx_M0.o \
				$(BUILD_DIR)/i2c_18xx_43xx_M0.o \
				$(BUILD_DIR)/adc_18xx_43xx_M0.o \
				$(BUILD_DIR)/board_M0.o \
				$(BUILD_DIR)/mydsp_wrap_M0.o \
				$(BUILD_DIR)/interface_M0.o \
				$(BUILD_DIR)/pots_M0.o \
				$(BUILD_DIR)/adxl345_M0.o \


all: $(BUILD_DIR)/RAM_$(PROJECT).axf

# Faust and assembly compiling rules

faust_dsp/mydsp.c: faust_dsp/audio_effect.dsp
	@-echo FAUST src: $<
	$(Q) $(FAUST) -I $(FAUSTPATH)/architecture -lang c -o $@ $<

$(BUILD_DIR)/%.o: src/%.s
	@-echo AS src: $@
	$(Q) $(AS) -c $(ASFLAGS) -o $@ $<

# Cortex M0 c compiling rules

$(BUILD_DIR)/mydsp_wrap_M0.o: faust_dsp/mydsp.c faust_dsp/mydsp_wrap.c
	@-echo CCm0 faust-generated: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M0) $(INCLUDES) $(INCLUDES_M0) -o $@ faust_dsp/mydsp_wrap.c

$(BUILD_DIR)/%_M0.o: src/%_M0.c
	@-echo CCm0 src: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M0) $(INCLUDES) $(INCLUDES_M0) -o $@ $<

$(BUILD_DIR)/%_M0.o: lpcopen/%.c
	@-echo CCm0 lpcopen: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M0) $(INCLUDES) $(INCLUDES_M0) -o $@ $<

$(BUILD_DIR)/%_M0.o: lpcopen/lpc_chip_43xx_M0/src/%.c
	@-echo CCm0 lpcopen chip: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M0) $(INCLUDES) $(INCLUDES_M0) -o $@ $<

# Cortex M4 c compiling rules

$(BUILD_DIR)/mydsp_wrap.o: faust_dsp/mydsp.c faust_dsp/mydsp_wrap.c
	@-echo CC faust-generated: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M4) $(CFLAGS_DSP) $(INCLUDES) $(INCLUDES_M4) -o $@ faust_dsp/mydsp_wrap.c

$(BUILD_DIR)/%.o: src/%.c
	@-echo CC src: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M4) $(INCLUDES) $(INCLUDES_M4) -o $@ $<

$(BUILD_DIR)/%.o: lpcopen/%.c
	@-echo CC lpcopen: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M4) $(INCLUDES) $(INCLUDES_M4) -o $@ $<

$(BUILD_DIR)/%.o: lpcopen/lpc_chip_43xx_M4/src/%.c
	@-echo CC lpcopen chip: $@
	$(Q) $(CC) -c $(CFLAGS) $(CFLAGS_M4) $(INCLUDES) $(INCLUDES_M4) -o $@ $<

# Linking and transforming binary image for FLASH

$(BUILD_DIR)/$(PROJECT)_M0.axf: $(OBJECTS_M0)
	@-echo 'LD OBJECTS, LIBS -> $@ @M0'
	$(Q) $(LD) $(CFLAGS) $(CFLAGS_M0) -Wl,--gc-sections -T $(LINKERSCRIPT_FLASH) -T $(LINKERSCRIPT_M0) $(OBJECTS_M0) -Wl,-Map=$(BUILD_DIR)/$(PROJECT)_M0.map -o $@

$(BUILD_DIR)/$(PROJECT)_M0.o: $(BUILD_DIR)/$(PROJECT)_M0.axf
	@-echo 'OC $< -> $@ @M0'
	$(Q) $(OC) --target elf32-littlearm --verbose --strip-all $(OC_RENAMES) $< $@

$(BUILD_DIR)/$(PROJECT).axf: $(OBJECTS) $(BUILD_DIR)/$(PROJECT)_M0.o
	@-echo 'LD OBJECTS, LIBS, M0 image -> $@'
	$(Q) $(LD) $(CFLAGS) $(CFLAGS_M4) -Wl,--gc-sections -T $(LINKERSCRIPT_FLASH) -T $(LINKERSCRIPT_M4) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map $(OBJECTS) $(BUILD_DIR)/$(PROJECT)_M0.o $(CLIBS) -o $@
	$(Q) $(SIZE) $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).axf
	@-echo 'OC $< -> $@'
	$(Q) $(OC) -O binary $< $@

$(BUILD_DIR)/$(PROJECT).bin.hdr: $(BUILD_DIR)/$(PROJECT).bin
#	$(Q)cp $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/_tmp.dfu
#	$(Q)dfu-suffix --vid=0x1fc9 --pid=0x000c --did=0x0 -a _tmp.dfu
	@-echo 'appending header $< -> $@'
	$(Q) python -c "import os.path; import struct; print('0000000: 1a 3f ' + ' '.join(map(lambda s: '%02x' % ord(s), struct.pack('<H', os.path.getsize('$(BUILD_DIR)/$(PROJECT).bin') / 512 + 1))) + '00 00 00 00 00 00 00 00 ff ff ff ff')" | xxd -g1 -r > $(BUILD_DIR)/header.bin
	$(Q) cat $(BUILD_DIR)/header.bin $(BUILD_DIR)/$(PROJECT).bin > $(BUILD_DIR)/$(PROJECT).bin.hdr

# Linking and transforming binary image for RAM

$(BUILD_DIR)/RAM_$(PROJECT)_M0.axf: $(OBJECTS_M0)
	@-echo 'LD OBJECTS, LIBS -> $@ @M0'
	$(Q) $(LD) $(CFLAGS) $(CFLAGS_M0) -Wl,--gc-sections -T $(LINKERSCRIPT_RAM) -T $(LINKERSCRIPT_M0) $(OBJECTS_M0) -Wl,-Map=$(BUILD_DIR)/$(PROJECT)_M0.map -o $@

$(BUILD_DIR)/RAM_$(PROJECT)_M0.o: $(BUILD_DIR)/RAM_$(PROJECT)_M0.axf
	@-echo 'OC $< -> $@ @M0'
	$(Q) $(OC) --target elf32-littlearm --verbose --strip-all $(OC_RENAMES) $< $@

$(BUILD_DIR)/RAM_$(PROJECT).axf: $(OBJECTS) $(BUILD_DIR)/RAM_$(PROJECT)_M0.o
	@-echo 'LD OBJECTS, LIBS, M0 image -> $@'
	$(Q) $(LD) $(CFLAGS) $(CFLAGS_M4) -Wl,--gc-sections -T $(LINKERSCRIPT_RAM) -T $(LINKERSCRIPT_M4) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map $(OBJECTS) $(BUILD_DIR)/RAM_$(PROJECT)_M0.o $(CLIBS) -o $@
	$(Q) $(SIZE) $@

# Executing and cleaning

ram: $(BUILD_DIR)/$(PROJECT).bin.hdr
	@-echo downloading firmware to RAM
	$(Q) dfu-util -d 1fc9:000c -D $(BUILD_DIR)/$(PROJECT).bin.hdr

gdb: $(BUILD_DIR)/RAM_$(PROJECT).axf
	$(GDB) $< -q -ex 'target remote localhost:3333' -ex "mon targets lpc43xx.m4" -ex "mon arm semihosting enable"

gdb_M4: $(BUILD_DIR)/RAM_$(PROJECT).axf
	$(GDB) $< -q -ex 'target remote localhost:3334' -ex "mon targets lpc43xx.m4" -ex "mon arm semihosting enable"

gdb_M0: $(BUILD_DIR)/RAM_$(PROJECT)_M0.axf
	$(GDB) $< -q -ex 'target remote localhost:3333' -ex "mon targets lpc43xx.m0"

clean:
	@-echo cleaning
	$(Q) find $(BUILD_DIR) -type f -exec rm {} \;
	$(Q) rm -f ./faust_dsp/mydsp.c
