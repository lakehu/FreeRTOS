#
# Copyright 2022 NXP.
#

#Path to Tool Directory (NXP GCC 9.2 ARM64)
CrossToolDir ?= C:/NXP/S32DS.3.4/S32DS/build_tools/gcc_v9.2/gcc-9.2-arm64-eabi
#Path to FreeRtos Directory
OsPath ?= ../..
OutDir := Output


AS := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
CC := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
LD := $(CrossToolDir)/bin/aarch64-none-elf-gcc.exe
CP := $(CrossToolDir)/bin/aarch64-none-elf-objcopy.exe
OD := $(CrossToolDir)/bin/aarch64-none-elf-objdump.exe


OSCFLAGS := -std=c99 -Os -funsigned-char -fstack-usage -fdump-ipa-all -fomit-frame-pointer \
			-ggdb3 -pedantic -Wall -Wextra -c -fno-short-enums -funsigned-bitfields        \
			-fno-common -Wunused -Wstrict-prototypes -Wsign-compare                        \
			-Werror=implicit-function-declaration -Wundef -mlittle-endian                  \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib" -mstrict-align                \
			-mcpu=cortex-a53 -specs=nosys.specs  -I"include" -I"$(OsPath)/Source/include"  \
			-I"$(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE"


OSASFLAGS := -c -x assembler-with-cpp -mlittle-endian -mcpu=cortex-a53 -specs=nosys.specs  \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib"


LDFLAGS  := -nostartfiles --entry=Reset_Handler -ggdb3 -T "./image.ld"                       \
			-Wl,-Map,$(OutDir)/image.map -mlittle-endian -mcpu=cortex-a53 -specs=nosys.specs \
			--sysroot="$(CrossToolDir)/aarch64-none-elf/lib"

OBJFILES_OS :=  $(OutDir)/tasks.o        \
				$(OutDir)/timers.o       \
				$(OutDir)/event_groups.o \
				$(OutDir)/queue.o        \
				$(OutDir)/list.o         \
				$(OutDir)/croutine.o     \
				$(OutDir)/port.o         \
				$(OutDir)/portASM.o      \
				$(OutDir)/heap_4.o

$(OutDir)/%.o: $(OsPath)/Source/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

$(OutDir)/%.o: $(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

$(OutDir)/%.o: $(OsPath)/Source/portable/GCC/ARM_CA53_64_BIT_SRE/%.S
	$(AS) $(OSASFLAGS) -c -o $@ $<

$(OutDir)/%.o: $(OsPath)/Source/portable/MemMang/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

OBJFILES_APP := $(OutDir)/main_blinky.o      \
				$(OutDir)/main_regtest.o     \
				$(OutDir)/reg_test.o         \
				$(OutDir)/core_utility.o     \
				$(OutDir)/exceptions_stack.o \
				$(OutDir)/exceptions.o       \
				$(OutDir)/gic.o              \
				$(OutDir)/main.o             \
				$(OutDir)/startup.o          \
				$(OutDir)/system.o           \
				$(OutDir)/tick_config.o      \
				$(OutDir)/Vector_Table.o

$(OutDir)/%.o: source/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

$(OutDir)/%.o: source/Blinky_Demo/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

$(OutDir)/%.o: source/RegTest_Demo/%.c
	$(CC) $(OSCFLAGS) -c -o $@ $<

$(OutDir)/%.o: source/%.s
	$(AS) $(OSASFLAGS) -c -o $@ $<

$(OutDir)/%.o: source/%.S
	$(AS) $(OSASFLAGS) -c -o $@ $<

$(OutDir)/%.o: source/RegTest_Demo/%.S
	$(AS) $(OSASFLAGS) -c -o $@ $<


OBJFILES := $(OBJFILES_OS) $(OBJFILES_APP)

# default goal
all: clean make_dir $(OutDir)/image.elf

clean:
	rm -f $(OutDir)/*

make_dir:
	mkdir -p $(OutDir)
# Linking
$(OutDir)/image.elf: $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJFILES)
	$(CP) -S -I elf32-littlearm -O binary $(OutDir)/image.elf $(OutDir)/image.bin
	$(OD) -m arm -Mforce-thumb --target=binary -D $(OutDir)/image.bin > $(OutDir)/image.asm
	$(OD)  -D $(OutDir)/image.elf > $(OutDir)/image.S
