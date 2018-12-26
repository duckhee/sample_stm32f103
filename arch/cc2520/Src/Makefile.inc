###########################################################
# GCC template makefile
###########################################################


STM32F10X_CC25250_SRCS = 
STM32F10X_CC25250_SRCS += basic_rf_security.c
STM32F10X_CC25250_SRCS += basic_rf.c
STM32F10X_CC25250_SRCS += cc2520_mango_porting.c
STM32F10X_CC25250_SRCS += hal_cc2520.c
STM32F10X_CC25250_SRCS += hal_int.c
STM32F10X_CC25250_SRCS += hal_rf_security.c
STM32F10X_CC25250_SRCS += hal_rf.c
STM32F10X_CC25250_SRCS += util.c

C_SOURCES += $(STM32F10X_CC25250_SRCS)