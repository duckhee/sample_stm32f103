###########################################################
# GCC template makefile
###########################################################

HW_DRIVER_SOURCES = 
HW_DRIVER_SOURCES += Hw_Led.c
HW_DRIVER_SOURCES += Hw_Key.c
HW_DRIVER_SOURCES += Hw_Segment.c
HW_DRIVER_SOURCES += zigbee_test.c
HW_DRIVER_SOURCES += 

C_SOURCES += $(HW_DRIVER_SOURCES)