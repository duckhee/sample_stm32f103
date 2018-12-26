###########################################################
# GCC template makefile
###########################################################


HW_USB_USER_SOURCES = 
HW_USB_USER_SOURCES += usb_desc.c
HW_USB_USER_SOURCES += usb_endp.c
HW_USB_USER_SOURCES += usb_prop.c
HW_USB_USER_SOURCES += usb_istr.c
HW_USB_USER_SOURCES += usb_pwr.c

C_SOURCES += $(HW_USB_USER_SOURCES)