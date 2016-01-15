MCU ?= attiny88
AVRDUDE_PROGRAMMER ?= usbasp

AVRCC ?= avr-gcc
AVRCXX ?= avr-g++
AVRFLASH ?= avrdude
AVRNM ?= avr-nm
AVROBJCOPY ?= avr-objcopy
AVROBJDUMP ?= avr-objdump

MCU_FLAGS = -mmcu=attiny88 -DF_CPU=8000000UL

SHARED_FLAGS = ${MCU_FLAGS} -I. -Os -Wall -Wextra -pedantic
SHARED_FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
SHARED_FLAGS += -fwhole-program -flto -mstrict-X

CFLAGS += ${SHARED_FLAGS} -std=c11
CXXFLAGS += ${SHARED_FLAGS} -std=c++11 -fno-rtti -fno-exceptions

ASFLAGS += ${MCU_FLAGS} -wA,--warn
LDFLAGS += -Wl,--gc-sections

AVRFLAGS += -U lfuse:w:0xee:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
AVRFLAGS += -U flash:w:main.hex
#AVRFLAGS += -U eeprom:w:main.eep

HEADERS  = $(wildcard *.h)
ASFILES  = $(wildcard *.S)
CFILES   = $(wildcard *.c)
CXXFILES = $(wildcard *.cc)
OBJECTS  = ${CFILES:.c=.o} ${CXXFILES:.cc=.o} ${ASFILES:.S=.o}

all: main.elf

%.hex: %.elf
	${AVROBJCOPY} -O ihex -R .eeprom $< $@

%.eep: %.elf
	${AVROBJCOPY} -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@

%.o: %.cc ${HEADERS}
	${AVRCXX} ${CXXFLAGS} -o $@ $< -c -Wl,-Map=main.map,--cref

%.o: %.c ${HEADERS}
	${AVRCC} ${CFLAGS} -o $@ $< -c -Wl,-Map=main.map,--cref

main.elf: ${OBJECTS}
	${AVRCXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}
	@echo
	@avr-size --format=avr --mcu=${MCU} $@

program: main.hex #main.eep
	${AVRFLASH} -p ${MCU} -c ${AVRDUDE_PROGRAMMER} ${AVRFLAGS}

secsize: main.elf
	${AVROBJDUMP} -hw -j.text -j.bss -j.data main.elf

funsize: main.elf
	${AVRNM} --print-size --size-sort main.elf

.PHONY: all program secsize funsize
