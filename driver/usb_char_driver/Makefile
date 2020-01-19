KDIR=/lib/modules/$(shell uname -r)/build
MOD_NAME=usb_rgb
obj-m := $(MOD_NAME).o

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules