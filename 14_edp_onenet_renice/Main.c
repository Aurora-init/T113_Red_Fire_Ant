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
#include "Uart.h"

#define UART_BUF_LEN 1024

/*定义于wifi_connect.c中*/
void wifi_connect(void);

/*******************************************START多线程1之接收线程START**************************************************/
void *OneNetReacv(void *arg)
{
	/*保存接收的字符串*/
	char RecvBuffer[10] = {0};  

	/*当接收到onenet平台的数据就打印出来，后面也可以用这个线程来实验一些远程控制的操作*/
	while(1)
	{
		memset(RecvBuffer ,0 ,sizeof(RecvBuffer));
		OneNet_RecvData((void*)&sockfd , RecvBuffer);
		printf("RecvBuffer:%s\n", RecvBuffer);
	}
}

/*******************************************END多线程1之接收线程END**************************************************/

/*程序实现*/
/*
0.完成安装WIFI驱动,连接热点等一系列操作
1.初始化onenet、创建一个多线程用于接收从onenet云平台下发的数据
2.打开串口、在这里串口只接收不发送，所以只用到read
	串口使用：
	2.1.使用 open() 函数打开串口设备文件。
	2.2.配置串口相关参数，包括波特率、数据位、停止位、校验位等
	2.3.使用 read() 函数从串口接收数据。
	2.4.使用 close() 函数关闭串口文件描述符，释放资源。
3.把串口数据处理后放到buf数据缓冲区中上传到onenet
*/

int main(void)
{	
	/*返回值变量*/
	int ret;
	
	/*串口变量*/
	char uart_buf[UART_BUF_LEN];	//串口数据缓冲区
	int fd = 0;						//串口设备文件描述符
							
	/*温度变量及onenet数据流缓冲区变量*/
	char temp;						//温度变量
	char buf[20] = {0};				//onenet数据流缓冲

	/*副线程句柄*/
	pthread_t thread;				//副线程

	/*0.完成安装WIFI驱动,连接热点等一系列操作*/
	wifi_connect();

	/*1.onenet初始化、创建一个多线程用于接收从onenet云平台下发的数据*/

	/*onenet初始化*/
	OneNet_Init();			
	printf("HardWare Init Success!\n");	
	
	/*开启接收命令线程*/
	ret = pthread_create(&thread, NULL,OneNetReacv, NULL);
	if (ret){
		printf("pthread_create err!\n");
		return -1;
	}

	/*2.打开串口、在这里串口只接收不发送，所以只用到read*/
	/*2.1.使用 open() 函数打开串口设备文件。*/
		/*O_RDWR | O_NOCTTY | O_NDELAY | O_CLOEXEC：打开文件时使用读写模式、不将打开的设备文件作为控制终端设备、非阻塞模式、打开文件时将文件描述符标记为“关闭时执行”、*/
	fd = open("/dev/ttyUSB0" , O_RDWR|O_NOCTTY);
	if(fd < 0)
	{
		perror("open /dev/ttyUSB0 failed");
		return 0;
	}
	
	/*2.2.配置串口相关参数，包括波特率、数据位、停止位、校验位等*/
	set_serial_uart(fd, 115200, 8, 'N', 1);

	/*while循环*/
	while(1)
	{
		/*2.3.使用 read() 函数从串口接收数据。*/
		memset(uart_buf, 0, sizeof(uart_buf));//清空uart_buf缓冲区
		ret = read(fd , uart_buf , sizeof(uart_buf)-1); // sizeof(uart_buf)-1是留一个位置给'\0'
		if(ret < 0 ){	
			perror("Read failed");
			break;
		} else if (ret == 0) { //增加错误处理，当读取到文件结尾（EOF）时，应该退出循环并关闭文件。
			printf("End of file reached\n");
			close(fd);
			exit(EXIT_SUCCESS);
		} else { //read函数返回读取的字节数，这里是在读取到的数据末尾的位置添加一个字符串终止符（'\0'）
			uart_buf[ret] = '\0';
		}
		
		/* 验证 uart_buf 中的数据是否符合预期格式 */		
		if (strlen(uart_buf) != 2) { //只要前2个，因为温度一般也就两位数，后面得想想如何把温度、湿度、光照强度等数据也加上
    		printf("Error: Invalid data length.\n");
    		continue; // 数据长度不符合预期，跳过当前循环
		}
		if (!isdigit(uart_buf[0]) || !isdigit(uart_buf[1])) {
			printf("Error: Invalid data format.\n");
			continue; // 数据格式不符合预期，跳过当前循环
		}

		/*3.把串口数据处理后放到buf数据缓冲区中上传到onenet*/
		/*3.1.准备要传到onenet上的数据*/
		temp = ((uart_buf[0] - '0') * 10) + (uart_buf[1] - '0');
		/*3.2.清空原本的onenet数据流缓冲区，把新数据写进去*/
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"Temp",temp);
		sleep(1);
		printf("Send : %d \n", temp); // 打印
	}

	/*关闭socket连接*/
	TcpClientClose();
	/* 等待线程退出 */
    pthread_join(thread,NULL);

	return 0;
}
