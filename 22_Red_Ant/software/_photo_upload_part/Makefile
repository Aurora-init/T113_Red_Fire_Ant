KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master


all:
	$(CROSS_COMPILE)gcc -o photo_upload main.c -lcurl

clean:
	rm -f photo_upload 

obj-m	+= main.o
