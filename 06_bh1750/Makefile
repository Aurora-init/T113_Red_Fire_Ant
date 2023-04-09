KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master


all:
	make -C $(KERN_DIR) M=`pwd` modules 
	$(CROSS_COMPILE)gcc -o bh1750_drv_test bh1750_drv_test.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
	rm -f bh1750_drv_test

obj-m	+= bh1750_drv.o
