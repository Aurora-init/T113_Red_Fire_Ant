# 以下是指定编译器
CC = $(CROSS_COMPILE)gcc
# 以下是指定编译需要的头文件
CFLAGS = -g -Wall -O0 -I./ \
		      -I/home/book/buildroot-100ask_t113-pro/buildroot/output/build/bluez5_utils-5.63/
# 以下是源⽂件
SRCS =  main.c  ./attach.c  ./efivars.c  ./gap.c  ./gatt.c  ./log.c 
# 以下是指定需要的库文件.o
LIBS = -L/home/book/buildroot-100ask_t113-pro/buildroot/output/build/bluez5_utils-5.63/lib/.libs/ \
	-lbluetooth-internal -lbluetooth 

# 以下是指 当所有.c文件变成.o文件
OBJS = $(SRCS:.c=.o)  

# 以下是生成可执行文件
EXECUTABLE = bt_app

O_DIR =  /home/book/buildroot-100ask_t113-pro/buildroot/output/build/bluez5_utils-5.63/src/shared/.libs/
MY_O =      $(O_DIR)libshared_glib_la-hci.o 		$(O_DIR)libshared_glib_la-gap.o 			\
	   $(O_DIR)libshared_glib_la-queue.o 		$(O_DIR)libshared_mainloop_la-mainloop.o 		$(O_DIR)libshared_glib_la-gatt-helpers.o 		\
	   $(O_DIR)libshared_mainloop_la-io-mainloop.o 	$(O_DIR)libshared_glib_la-hci-crypto.o 			$(O_DIR)libshared_glib_la-ecc.o					\
	   $(O_DIR)libshared_glib_la-crypto.o		$(O_DIR)libshared_mainloop_la-pcap.o 			$(O_DIR)libshared_glib_la-att.o					\
	   $(O_DIR)libshared_glib_la-gatt-client.o 	$(O_DIR)libshared_mainloop_la-uhid.o 			$(O_DIR)libshared_glib_la-gatt-server.o			\
	   $(O_DIR)libshared_glib_la-hfp.o 		$(O_DIR)libshared_glib_la-gatt-db.o  			$(O_DIR)libshared_glib_la-mgmt.o				\
	   $(O_DIR)libshared_glib_la-ad.o 		$(O_DIR)libshared_mainloop_la-btsnoop.o 		$(O_DIR)libshared_mainloop_la-ringbuf.o			\
	   $(O_DIR)libshared_mainloop_la-util.o 		$(O_DIR)libshared_mainloop_la-timeout-mainloop.o 	$(O_DIR)libshared_mainloop_la-mainloop-notify.o	

	  

# make all 执行生成可执行文件
# 1编译器 2编译选项 3输出 4⽣成的可执行文件 5需要的源文件 6需要的库文件
all:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SRCS) $(LIBS) $(MY_O)

clean:
	rm bt_app

