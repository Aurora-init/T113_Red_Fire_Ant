#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include "EdpKit.h"
#include "ConnectOneNet.h"
#include "gps_analyse.h"

#define GPS_LEN 1024

/*WiFi连接脚本函数,wifi_connect.c*/
void wifi_connect(void);

/*串口设置函数,uart1.c*/
int set_serial(int fd, int nSpeed, int nBits, char nEvent, int nStop);

/*******************************************START多线程1之接收线程START**************************************************/
/*静态分配缓冲区*/
#define BUFFER_SIZE 1024
char RecvBuffer[BUFFER_SIZE] = {0};

/*限制接收数据的大小*/
#define RECV_SIZE (BUFFER_SIZE - 1)

/*接收线程*/
void *OneNetRecv(void *arg)
{
    /*当接收到onenet平台的数据就打印出来，后面也可以用这个线程来实验一些远程控制的操作*/
    while(1)
    {
        /*清空缓冲区*/
        memset_s(RecvBuffer, BUFFER_SIZE, 0, BUFFER_SIZE);

        /*接收数据*/
        OneNet_RecvData((void*)&sockfd, RecvBuffer, RECV_SIZE);

        /*打印接收到的数据*/
        if(strlen(RecvBuffer) > 0)
        {
            printf_s("RecvBuffer:%s\n", RecvBuffer);
        }

        /*添加延时*/
        sleep(1);
    }
}
/*******************************************END多线程1之接收线程END**************************************************/


/*
0.连接WiFi热点
1.初始化onenet、初始化串口、创建一个多线程、
2.把串口数据处理后放到buf数据缓冲区中上传到onenet
*/

int main(void)
{	
	
	/*0.完成安装WIFI驱动,连接热点等一系列操作*/
	wifi_connect();
	
	/*串口变量*/
	char uart_buf[GPS_LEN];
	int fd = 0;
	int ret;

	/*经纬度数据变量*/
	GNGGA gngga;

	/*onenet数据流缓冲区变量*/
	char onenet_buf[20] = {0};

	pthread_t thread;
	
	/*1.初始化onenet、初始化串口、创建一个多线程、*/

	/************************ 硬件初始化 **************************/
	/*1.1.onenet初始化*/
	OneNet_Init();			
	printf("OneNet Init Success!\n");	

	/*1.2.开启串口USB0*/
	/*O_RDWR | O_NOCTTY | O_NDELAY | O_CLOEXEC：打开文件时使用读写模式，不将打开的设备文件作为控制终端设备，非阻塞模式，打开文件时将文件描述符标记为“关闭时执行”*/
	char *dev_name = "/dev/ttyUSB0";
	if ((fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY | O_CLOEXEC)) < 0){
		perror("Can't Open the ttyUSB0 Serial Port");
		return -1;
	}
	set_serial(fd, 9600, 8, 'N', 1);
	
	/*1.3.开启多线程接收命令*/
	ret = pthread_create(&thread, NULL,OneNetReacv, NULL);
	if (ret)
	{
		printf("pthread_create err!\n");
		return -1;
	}


	/*2.把串口数据处理后放到buf数据缓冲区中上传到onenet*/
	
	/*while循环*/
	while(1)
	{
		memset(uart_buf, 0, sizeof(uart_buf));//将bufx前10个字节设置为0
		ret = read(fd , uart_buf , sizeof(uart_buf)-1); // 留一个位置给‘\0'
		if(ret < 0 ){
			perror("Read failed");
			break;
		} else if (ret == 0) {	//增加错误处理，当读取到文件结尾（EOF）时，应该退出循环并关闭文件。
			printf("End of file reached\n");
			close(fd);
			exit(EXIT_SUCCESS);
		} else {
			uart_buf[ret] = '\0';
		}

		/*把存储GPS解析数据的结构体清空，把串口读到的数据扔到GPS分析函数中，返回解析好的GPS数据*/
		memset(&gngga, 0, sizeof(gngga));
		gps_analyse(uart_buf, &gngga); //将串口数据传入GPS分析函数中，并将处理好的GPS数据放进结构体
		
		/*清空原本的onenet数据流缓冲区，把新数据写进去*/
		memset(onenet_buf ,0 ,sizeof(onenet_buf));
		OneNet_SendData(onenet_buf,"latitude", gngga.latitude);
		sleep(1);
		printf("SEND : %s \n", gngga.latitude); // 打印
	}

	/*关闭socket连接*/
	TcpClientClose();
	/* 等待线程退出 */
    pthread_join(thread,NULL);

	return 0;
}

