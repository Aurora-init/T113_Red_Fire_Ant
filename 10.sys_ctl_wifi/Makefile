KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master


all:
	$(CROSS_COMPILE)gcc -o main main.c wifi_connect.c

clean:
	rm -f main 

obj-m	+= main.o
