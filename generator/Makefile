PRG            = main
OBJ            = main.o dac.o function.o
MODEL          = 644
FREQ           = 20000000UL
MCU_TARGET     = atmega$(MODEL)
OPTIMIZE       = -O3 -mcall-prologues
DEFS           =
LIBS           =

CC             = avr-g++
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x -DF_CPU=$(FREQ)
CFLAGS         = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -DF_CPU=$(FREQ)
LDFLAGS        = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

AVRDUDE	       = avrdude
PROG           = avr910
DEV            = /dev/ttyACM0

LFUSE	       = 0xf7
HFUSE	       = 0xd1
EFUSE	       = 0xff

FUNCTION_FILE  = function

all: $(PRG).elf lst text

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.bak *.a
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf $(PRG).hex

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

fuse:
	$(AVRDUDE) -p m$(MODEL) -c $(PROG) -P $(DEV) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

text: hex

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

install: hex
	$(AVRDUDE) -p m$(MODEL) -c $(PROG) -P $(DEV) -U flash:w:$(PRG).hex

function:
	g++ -o get_values get_values.cpp
	./get_values c > $(FUNCTION_FILE).c
	./get_values h > $(FUNCTION_FILE).h
