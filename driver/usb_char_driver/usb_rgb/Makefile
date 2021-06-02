KDIR=/lib/modules/$(shell uname -r)/build
MOD_NAME=usb_rgb
obj-m := $(MOD_NAME).o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -f $(obj-m)
	rm -f $(MOD_NAME).ko
	rm -f $(MOD_NAME).mod.o
	rm -f $(MOD_NAME).mod.c
