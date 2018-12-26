###########################################################
# GCC template makefile
#
# On command line:
#
# make all = Make software.
# make clean = Clean out built project files.
#
# To rebuild project do "make clean" then "make all".
###########################################################

#TOP := $(shell pwd)
TOP := .

include $(TOP)/inc.mk


TARGET = sample_test

#USE_FREERTOS = y



#List C sources files here.
SRCS += $(C_SOURCES)

#List C++ sources files here.
CPPSRCS = 

#List Assmbler sources files here.
ASM_SOURCES = 

SUB_DIRS := 
SUB_DIRS += arch
SUB_DIRS += kernel

ifeq ($(USE_FREERTOS), y)
SUB_DIRS += kernel/freeRTOS
endif

ifeq ($(USE_FREERTOS), y)
C_DEFS += -D FREE_RTOS
endif


SUB_DIRS := $(addprefix $(TOP)/,$(SUB_DIRS))

vpath %.c $(SUB_DIRS)
vpath %.s $(SUB_DIRS)

#include all sub directories makefile
sinclude $(addsuffix /Makefile.inc,$(SUB_DIRS))

# List of include file path
#     Each directory must be seperated by a space.
EXTRA_LIBDIRS = 

# List of library file
#    Each library-name must be seperated by a space.
#    To add libxyz.a, libabc.a and libefsl.a:
#    EXTRA_LIBS = xyz abc efsl
EXTRA_LIBS = 

#LIBC_LIB = -L $(shell dirname `$(CC) $(ALL_CFLAGS) -print-file-name=libc.a`) -lc
#MATH_LIB = -L $(shell dirname `$(CC) $(ALL_CFLAGS) -print-file-name=libm.a`) -lm
#LIBGCC_LIB += -L $(shell dirname `$(CC) $(ALL_CFLAGS) -print-libgcc-file-name`) -lgcc
# CPLUSPLUS_LIB = -lstdc++

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
#LDFLAGS = -Map=$(TARGET).map --cref
LDSCRIPT = STM32F103RBTx_FLASH.ld
LD_LIBS = -lc -lm -lnosys -specs=nano.specs -u _printf_float -u _scanf_float -static
LDFLAGS = $(MCU) $(LD_LIBS) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections

LDFLAGS += $(patsubst %,-L%, $(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-L%, $(EXTRA_LIBS))

ifeq ($(ROM_MODE), y)
LDFLAGS += -TSTM32F103RBTx_ROM.ld
else
LDFLAGS += -TSTM32F103RBTx_FLASH.ld
endif

all: gccversion createdirs build sizeinfo

ifeq ($(FORMAT),ihex)
build: elf hex lss sym
hex: $(TARGET).hex
IMGEXT=hex
else
ifeq ($(FORMAT),binary)
build: elf bin lss sym
lss: $(TARGET).lss
bin: $(TARGET).bin
sym: $(TARGET).sym
IMGEXT=bin
else
$(error "please check output-format $(FORMAT)")
endif
endif

doc:
	doxygen doc.mk

.PHONY: all build bin doc

include $(TOP)/rule.mk