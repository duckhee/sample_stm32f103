###########################################################
# GCC template makefile
###########################################################


FREERTOS_CORE_SOURCES = 
FREERTOS_CORE_SOURCES += croutine.c
FREERTOS_CORE_SOURCES += event_groups.c
FREERTOS_CORE_SOURCES += list.c
FREERTOS_CORE_SOURCES += queue.c
FREERTOS_CORE_SOURCES += stream_buffer.c
FREERTOS_CORE_SOURCES += tasks.c
FREERTOS_CORE_SOURCES += timers.c

ifeq ($(USE_FREERTOS),y)


SUB_ARCH := \
kernel/freeRTOS/Common/Minimal \
kernel/freeRTOS/MemMang \
kernel/freeRTOS/portable/ARM_CM3 \
kernel/freeRTOS \

C_SOURCES += $(FREERTOS_CORE_SOURCES)

SUB_ARCH  := $(addprefix $(TOP)/kernel/freeRTOS/,$(SUB_ARCH))

vpath %.c $(SUB_ARCH)
vpath %.s $(SUB_ARCH)

sinclude $(addsuffix /Makefile.inc, $(SUB_ARCH))

EXTRAINCDIRS += \
$(TOP)/kernel/freeRTOS/include \
$(TOP)/kernel/freeRTOS/portable/ARM_CM3 \

endif
