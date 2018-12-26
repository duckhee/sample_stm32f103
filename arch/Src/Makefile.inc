###########################################################
# GCC template makefile
###########################################################

MAIN_SOURCE = 
MAIN_SOURCE += debug.c
MAIN_SOURCE += Hw_Config.c
MAIN_SOURCE += stm32f10x_it.c
MAIN_SOURCE += syscalls.c
MAIN_SOURCE += Main.c

C_SOURCES += $(MAIN_SOURCE)