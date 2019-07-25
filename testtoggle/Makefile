#src=$(wildcard *.c)
SOURCEDIR=./
SOURCES := $(shell find $(SOURCEDIR) -name '*.c')

target=cdc_ecm

SIZE = arm-none-eabi-size
OBJ_COPY = arm-none-eabi-objcopy
CC:=arm-none-eabi-gcc
CFLAGS =-c  -Wall  -Os -std=gnu99 -g -O2 -Wall -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -Wfatal-errors
CFLAGS += -Iinclude

LDFLAGS = --specs=nosys.specs -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m0

OBJS = $(patsubst %.c,%.o,$(SOURCES))

all: cdc_ecm

$(target): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	$(SIZE) -tA $@ 
	$(OBJ_COPY) -O binary $@ $@.bin

burn:
	st-flash write $(target).bin 0x8000000	

#; \
#	$(STRIP) $@
	
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

distclean clean:
	rm -f *.o $(target) *.bin

