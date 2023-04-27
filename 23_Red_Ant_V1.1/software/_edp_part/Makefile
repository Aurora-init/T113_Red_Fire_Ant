CC=arm-linux-gnueabi-gcc
CFLAGS= -I. -D_LINUX -lm  -pthread
CLIENT_OBJ = cJSON.o ConnectOneNet.o EdpKit.o wifi_connect.o Uart.o gps_analyse.o Main.o  
KERN_DIR = /home/book/buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master

all: edp
	make -C $(KERN_DIR) M=`pwd` modules 

# 如果需要加密功能且系统内已经安装openssl，
# 取消以下两行注释
#CFLAGS+=-D_ENCRYPT -lcrypto
#CLIENT_OBJ += Openssl.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

edp: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f edp $(CLIENT_OBJ)
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
