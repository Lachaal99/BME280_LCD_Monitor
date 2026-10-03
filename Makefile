TARGET = app

CFLAGS = -mcpu=cortex-m4 -mthumb -Og -g3 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -Wextra
INC = -Iinc
LDFLAGS = -nostdlib -T linker_script.ld
PREFIX = arm-none-eabi-

CC = $(PREFIX)gcc
SRCS := $(wildcard src/*.c)
ASSM := startup/startup_stm32f446.s
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))
OBJS += $(patsubst startup/%.s, obj/%.o, $(ASSM))


all: build/$(TARGET).elf
	@echo "Size of the elf file:"
	$(PREFIX)size  $^

build/$(TARGET).elf: $(OBJS)
	@echo "linking .."
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	@echo "Compiling .."
	@mkdir -p $(@D)
	$(CC) -c $< $(INC) $(CFLAGS) -o $@

obj/%.o: $(ASSM)
	@mkdir -p $(@D)
	$(CC) -c $< $(INC) $(CFLAGS) -o $@

readelf: build/$(TARGET).elf
	$(PREFIX)readelf -s $<
	$(PREFIX)readelf -S $<

flash: build/$(TARGET).hex
	@echo "Flashing"
	sudo openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $^ verify reset exit"

%.hex: %.elf
	$(PREFIX)objcopy -O ihex $^ $@


clean :
	@echo "deleting build files .."
	@rm -f build/* obj/*


