KDIR ?= /lib/modules/`uname -r`/build
obj-m  := ideapad-hacks.o

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	rm -rf *.o *.ko .*.cmd .tmp_versions Module.symvers modules.order
