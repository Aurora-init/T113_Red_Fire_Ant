KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master


all:
	arm-linux-gnueabi-gcc -o photo_upload main.c -I/home/book/T113/buildroot-100ask_t113-pro/buildroot/output/build/host-cmake-3.16.9/Utilities/cmcurl/include -L/home/book/Desktop/curl-8.0.1/output/lib -lcurl

clean:
	rm -f photo_upload 

obj-m	+= main.o
