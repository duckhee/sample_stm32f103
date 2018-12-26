#####################################################################
# GCC template makefile inc.mk
# 
# On command line : 
#
#
#
#
#
#
#
#
#
#
#
#####################################################################

#model & sub model
MODEL = CMSIS
SUBMODEL = version1

#MANGO_Z1 = y

REMOVE_CMD = rm

PREFIX = arm-none-eabi-

ComSpec = arm-none-eabi

SHELL = sh

REMOVE_CMD = rm

CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
AR = $(PREFIX)ar
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size
NM = $(PREFIX)nm
OBJDUMP = $(PREFIX)objdump
REMOVE = $(REMOVE_CMD) -rf


#####################################################################
# CFLAGS
#####################################################################

#cpu
CPU = -mcpu=cortex-m3

FPU = 

FLOAT-ABI = 

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

## Output format. (can be ihex or binary)
## (binary i.e. for openocd and SAM-BA, hex i.e. for lpc21isp and uVision)
#FORMAT = ihex
FORMAT = binary

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = g

# macrros for gcc 
# As defines
AS_DEF = 

# Debugging format.
# Native formats for GCC's -g are stabs [default], or dwarf-2.
#DEBUG = stabs
DEBUG = dwarf-2

# Place -D or -U options for C here
#CDEFS =  -D$(RUN_MODE)
ifeq ($(MANGO_Z1),y)
CDEFS = \
-D USE_STDPERIPH_DRIVER \
-D BOARD_DEF_MANGO_Z1
else
CDEFS = \
-D USE_STDPERIPH_DRIVER \
-D BOARD_DEF_MANGO_M32
endif
# Place -D or -U options for ASM here
#ADEFS =  -D$(RUN_MODE)
ADEFS = 

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
#Assem C common flags
COMFLAGS = -Wall -fdata-sections -ffunction-sections 
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(C_INCLUDES)
CFLAGS += -O$(OPT)
CFLAGS +=  -Wcast-align -Wimplicit
CFLAGS += -Wpointer-arith -Wswitch
CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += $(COMFLAGS)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
ASFLAGS = $(COMFLAGS)

#flags only for C
CONLYFLAGS += -Wnested-externs

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CONLYFLAGS += -std=gnu99


#warings with
CFLAGS += -Wcast-qual
CONLYFLAGS += -Wmissing-prototypes
CONLYFLAGS += -Wstrict-prototypes
CONLYFLAGS += -Wmissing-declarations

#output directory
LID_DIR := $(TOP)/lib.$(SUBMODEL)
OUT_DIR := $(TOP)/obj.$(SUBMODEL)
PRJ_LIBS :=
