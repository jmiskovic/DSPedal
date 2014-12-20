PROJECT = DSPedal

GCCPATH = ../../gcc-arm-none-eabi-4_8-2014q3
BUILD_DIR = out

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

LINKERSCRIPT = RAMboot.ld
#LINKERSCRIPT = FLASHboot.ld

#CLIBS = ./libs/libgcc.a \
#		./libs/libc.a \
#./libs/libcr_eabihelpers.a \
#		./libs/libcr_nohost.a \
#		$(GCCPATH)/arm-none-eabi/lib/libm.a \

CORELIB = ../Core

# TODO: -specs=nano.specs

## Compiler flags
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS+= -O0 -g3 -ffunction-sections -fmessage-length=0  -fno-stack-protector -fdata-sections -fsingle-precision-constant -fno-common
#CFLAGS+= -flto -fno-builtin -nostdlib
CFLAGS+= -MD -std=c99 -Wall #-pedantic
CFLAGS+= -DCORE_M4 -D__MULTICORE_NONE -D__GNU_ARM
CFLAGS+= -DDEBUG -D__USE_LPCOPEN -DDONT_RESET_ON_RESTART -DDEBUG_ENABLE
CFLAGS+= -lrdimon -lc --specs=nano.specs --specs=rdimon.specs

## Assembler flags
ASFLAGS = -c -x assembler-with-cpp -D__START=main

INCLUDES = -I $(CORELIB)/Device/NXP/LPC43xx/Include
INCLUDES+= -I $(CORELIB)/CMSIS/Include
INCLUDES+= -I src
INCLUDES+= -I lpcopen
INCLUDES+= -I lpcopen/lpc_chip_43xx/inc
INCLUDES+= -I lpcopen/lpc_chip_43xx/inc/usbd

#Q := @

## Linker flags
LDFLAGS = $(CFLAGS) -T $(LINKERSCRIPT) -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

OBJECTS = 	$(BUILD_DIR)/startup.o \
			$(BUILD_DIR)/sysinit.o \
			$(BUILD_DIR)/board.o \
			$(BUILD_DIR)/board_sysinit.o \
			$(BUILD_DIR)/uda1380.o \
			$(BUILD_DIR)/sysinit_18xx_43xx.o \
			$(BUILD_DIR)/chip_18xx_43xx.o \
			$(BUILD_DIR)/clock_18xx_43xx.o \
			$(BUILD_DIR)/gpio_18xx_43xx.o \
			$(BUILD_DIR)/emc_18xx_43xx.o \
			$(BUILD_DIR)/i2c_18xx_43xx.o \
			$(BUILD_DIR)/i2s_18xx_43xx.o \
			$(BUILD_DIR)/cdc_vcom.o \
			$(BUILD_DIR)/cdc_desc.o \
			$(BUILD_DIR)/fpu_init.o \
			$(BUILD_DIR)/gpdma_18xx_43xx.o \
			$(BUILD_DIR)/main.o \
#			$(BUILD_DIR)/sound.o \
#			$(BUILD_DIR)/faust.o \
#			$(BUILD_DIR)/uart_18xx_43xx.o \
#			$(BUILD_DIR)/i2s.o \


all: $(BUILD_DIR)/$(PROJECT).axf

$(BUILD_DIR)/%.o: src/%.s
	@-echo AS src: $@
	$(Q) $(CC) -c $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: src/%.c
	@-echo CC src: $@
	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

#$(BUILD_DIR)/%.o: exam/%.c
#	@-echo CC src: $@
#	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(BUILD_DIR)/%.o: lpcopen/%.c
	@-echo CC lpcopen: $@
	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(BUILD_DIR)/%.o: lpcopen/lpc_chip_43xx/src/%.c
	@-echo CC lpcopen chip: $@
	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

# $(BUILD_DIR)/%.o: $(DRIVERLIB)/source/%.c
# 	@-echo CC driverlib: $@
# 	$(Q) $(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(BUILD_DIR)/$(PROJECT).axf: $(OBJECTS)
	@-echo 'LD OBJECTS, LIBS -> $@'
	$(Q) $(LD) $(LDFLAGS) $(OBJECTS) $(CLIBS) -o $(BUILD_DIR)/$(PROJECT).axf
	$(Q) $(SIZE) $(BUILD_DIR)/$(PROJECT).axf

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).axf
	@-echo 'OC $< -> $@'
	$(Q) $(OC) -O binary $(BUILD_DIR)/$(PROJECT).axf $@

$(BUILD_DIR)/$(PROJECT).bin.hdr: $(BUILD_DIR)/$(PROJECT).bin
#	$(Q)cp $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/_tmp.dfu
#	$(Q)dfu-suffix --vid=0x1fc9 --pid=0x000c --did=0x0 -a _tmp.dfu
	@-echo 'appending header $< -> $@'
	$(Q) python -c "import os.path; import struct; print('0000000: 1a 3f ' + ' '.join(map(lambda s: '%02x' % ord(s), struct.pack('<H', os.path.getsize('$(BUILD_DIR)/$(PROJECT).bin') / 512 + 1))) + '00 00 00 00 00 00 00 00 ff ff ff ff')" | xxd -g1 -r > $(BUILD_DIR)/header.bin
	$(Q) cat $(BUILD_DIR)/header.bin $(BUILD_DIR)/$(PROJECT).bin > $(BUILD_DIR)/$(PROJECT).bin.hdr

ram: $(BUILD_DIR)/$(PROJECT).bin.hdr
	@-echo downloading firmware to RAM
	$(Q) dfu-util -d 1fc9:000c -D $(BUILD_DIR)/$(PROJECT).bin.hdr

gdb: $(BUILD_DIR)/$(PROJECT).axf
	arm-none-eabi-gdb $(BUILD_DIR)/$(PROJECT).axf -q -ex 'target remote localhost:3333' -ex "mon arm semihosting enable" -ex 'load'

clean:
	@-echo cleaning
	$(Q) rm -f ./$(BUILD_DIR)/*
