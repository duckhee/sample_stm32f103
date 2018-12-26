###########################################################
# GCC template makefile rule.mk
#
# On command line:
#
# make all = Make software.
# make clean = Clean out built project files.
#
# To rebuild project do "make clean" then "make all".
###########################################################

ifndef PREFIX
$(error PREFIX is not defined, Please include 'ink.mk' or PREFIX checking first)
endif

ALL_ASMFLAGS = $(MCU) -I. -x assembler-with-cpp $(ASFLAGS)
# Add target processor to flags.
ALL_CFLAGS = $(MCU) -I. $(CFLAGS)

AOBJS = $(addprefix $(OUT_DIR)/,$(ASM_SOURCES:.s=.o))
COBJS = $(addprefix $(OUT_DIR)/,$(SRCS:.c=.o))
CPPOBJS = $(addprefix $(OUT_DIR)/,$(CPPSRCS:.cpp=.o))

HESIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf


sizeinfo:
	$(ELFSIZE)

gccversion:
	@$(CC) --version

createdirs:
	@echo create directory $(OUT_DIR)
	$(shell mkdir ${OUT_DIR})

# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) $< $@

# Create final output file (.bin) from ELF output file.
%.bin: %.elf
	$(OBJCOPY) -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY: $(TARGET).elf
.PRECIOUS: $(AOBJS) $(COBJS) $(CPPOBJS)

%.elf: $(AOBJS) $(COBJS) $(CPPOBJS) $(PRJ_LIBS)
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM/Thumb
$(COBJS): $(OUT_DIR)/%.o : %.c
	$(CC) -c $(ALL_CFLAGS) $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJS): $(OUT_DIR)/%.o : %.cpp
	$(CC) -c $(ALL_CFLAGS) $(CPPFLAGS) $< -o $@

# Compile: create assembler files from C source files. ARM/Thumb
## does not work - TODO - hints welcome
##$(COBJ) : %.S : %.c
##	$(CC) -S $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJS) : $(OUT_DIR)/%.o : %.s
	$(CC) -c $(ALL_ASMFLAGS) $< -o $@


clean:
	@echo Cleaning project:
	$(REMOVE) $(COBJS) $(CPPOBJS) $(AOBJS)
	$(REMOVE) .dep $(OUT_DIR) $(LIB_DIR) .doc
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss

# Include the dependency files.
ifeq ($(PREFIX),)
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
endif

# Listing of phony targets.
.PHONY : all finish end sizeinfo gccversion createdirs build elf hex bin lss sym clean

