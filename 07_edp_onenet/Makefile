all: edp

CC=arm-linux-gnueabi-gcc
CFLAGS= -I. -D_LINUX -lm  -pthread
CLIENT_OBJ = cJSON.o EdpKit.o Main.o ConnectOneNet.o

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
