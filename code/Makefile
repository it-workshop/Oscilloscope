PRG            = main
OBJ            = main.o ffft.o menu.o lcd.o uart.o draw.o buttons.o settings.o
MODEL          = 644
FREQ           = 20000000UL
MCU_TARGET     = atmega$(MODEL)
OPTIMIZE       = -Os -mcall-prologues
DEFS           =
LIBS           =

CC             = avr-gcc
ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS) -DF_CPU=$(FREQ)
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

fonts:
	$(AVRDUDE) -p m$(MODEL) -c $(PROG) -P $(DEV) -U eeprom:w:eeprom_fonts

text: hex

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

install: hex
	$(AVRDUDE) -p m$(MODEL) -c $(PROG) -P $(DEV) -U flash:w:$(PRG).hex
