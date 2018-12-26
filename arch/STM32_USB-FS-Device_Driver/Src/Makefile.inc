###########################################################
# GCC template makefile
###########################################################

HW_USB_SOURCES = 
HW_USB_SOURCES += usb_core.c
HW_USB_SOURCES += usb_init.c
HW_USB_SOURCES += usb_int.c
HW_USB_SOURCES += usb_mem.c
HW_USB_SOURCES += usb_regs.c



C_SOURCES += $(HW_USB_SOURCES)