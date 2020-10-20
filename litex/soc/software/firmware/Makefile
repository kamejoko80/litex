include ../include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

ifeq ($(CPU),blackparrot)
BP_LIBS =  -L$(BP_EXTERNAL_DIR)/lib/gcc/riscv64-unknown-elf/8.3.0
BP_FLAGS = -lgcc
endif
# Permit TFTP_SERVER_PORT override from shell environment / command line
ifdef TFTP_SERVER_PORT
CFLAGS += -DTFTP_SERVER_PORT=$(TFTP_SERVER_PORT)
endif

# Distinguish platform name.
CPUFLAGS += -D$(PLATFORM)

OBJECTS = ads126x.o isr.o sdcard.o main.o boot-helper-$(CPU).o

all: firmware.bin

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
ifneq ($(OS),Windows_NT)
	chmod -x $@
endif
ifeq ($(CPUENDIANNESS),little)
	$(PYTHON) -m litex.soc.software.mkmscimg $@ --little
else
	$(PYTHON) -m litex.soc.software.mkmscimg $@
endif

firmware.elf: $(FIRMWARE_DIRECTORY)/linker.ld $(OBJECTS)


%.elf: ../libbase/crt0-$(CPU)-ctr.o ../libnet/libnet.a ../libbase/libbase-nofloat.a ../libcompiler_rt/libcompiler_rt.a
	$(LD) $(LDFLAGS) -T $(FIRMWARE_DIRECTORY)/linker.ld -Map firmware.map -N -o $@ \
		../libbase/crt0-$(CPU)-ctr.o \
		$(OBJECTS) \
		-L../libnet \
		-L../libbase \
		-L../libcompiler_rt \
		$(BP_LIBS) \
		-lnet -lbase-nofloat -lcompiler_rt \
		$(BP_FLAGS)

ifneq ($(OS),Windows_NT)
	chmod -x $@
endif

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.o: $(FIRMWARE_DIRECTORY)/%.c
	$(compile)

%.o: $(FIRMWARE_DIRECTORY)/%.S
	$(assemble)

clean:
	$(RM) $(OBJECTS) firmware.elf firmware.bin .*~ *~

.PHONY: all clean
