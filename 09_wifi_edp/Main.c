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

/*定义于wifi_connect.c中*/
void wifi_connect(void);

int set_serial_uart(int ser_fd)
{
	struct termios new_cfg,old_cfg;
		
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if	(tcgetattr(ser_fd, &old_cfg) != 0)
	{
		perror("tcgetattr");
		return -1;
	}
	
	bzero( &new_cfg, sizeof(new_cfg));
	/*原始模式*/
	/* 设置字符大小*/
	new_cfg = old_cfg; 
	cfmakeraw(&new_cfg); /* 配置为原始模式 */ 

	/*波特率为115200*/
	cfsetispeed(&new_cfg, B115200); 
	cfsetospeed(&new_cfg, B115200);
	
	//用于本地连接和接受使能
	new_cfg.c_cflag |= CLOCAL | CREAD;
	
	/*8位数据位*/
	new_cfg.c_cflag &= ~CSIZE;
	new_cfg.c_cflag |= CS8;

	/*无奇偶校验位*/
	new_cfg.c_cflag &= ~PARENB;

	/*1位停止位*/
	new_cfg.c_cflag &= ~CSTOPB;
	
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	new_cfg.c_cc[VTIME] = 0; //等待时间
	new_cfg.c_cc[VMIN] = 1; //最少接收字节数
	tcflush ( ser_fd, TCIOFLUSH);

	/*串口设置使能*/
	tcsetattr( ser_fd ,TCSANOW,&new_cfg);
}

/*接收线程*/
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


/*
0.连接WiFi热点
1.初始化onenet，串口等硬件，创建一个多线程
2.把串口数据处理后放到buf数据缓冲区中上传到onenet
*/

int main(void)
{	
	/*0.完成安装WIFI驱动,连接热点等一系列操作*/
	wifi_connect();
	
	/*1.初始化onenet，串口等硬件，创建一个多线程*/
	/*串口变量*/
	char bufx[10];
	int ret;

	/*定义温度及onenet数据流缓冲区*/
	char temp;
	char buf[20] = {0};
	char num = 0;		

	pthread_t thread;

	/************************ 硬件初始化 **************************/
	/*onenet初始化*/
	OneNet_Init();			
	printf("HardWare Init Success!\n");	
	
	/*开启多线程接收命令*/
	ret = pthread_create(&thread, NULL,OneNetReacv, NULL);
	if (ret)
	{
		printf("pthread_create err!\n");
		return -1;
	}
	
	/*开启串口USB0*/
	/*O_RDWR|O_NOCTTY：以读写方式打开设备并忽略终端控制*/
	int fd = open("/dev/ttyUSB0" , O_RDWR|O_NOCTTY);
	if(fd < 0)
	{
		perror("open /dev/ttyUSB0 failed");
		return 0;
	}
	set_serial_uart(fd);

	/*while循环*/
	while(1)
	{
		memset(bufx, 0, sizeof(bufx));//将bufx前10个字节设置为0
		ret = read(fd , bufx , sizeof(bufx)-1); // 留一个位置给‘\0'
		if(ret < 0 ){
			perror("Read failed");
			break;
		} else if (ret == 0) {	//增加错误处理，当读取到文件结尾（EOF）时，应该退出循环并关闭文件。
			printf("End of file reached\n");
			close(fd);
			exit(EXIT_SUCCESS);
		} else {
			bufx[ret] = '\0';
		}
		
		/* 验证 bufx 中的数据是否符合预期格式 */		
		if (strlen(bufx) != 2) {
    		printf("Error: Invalid data length.\n");
    		continue; // 数据长度不符合预期，跳过当前循环
		}
		if (!isdigit(bufx[0]) || !isdigit(bufx[1])) {
			printf("Error: Invalid data format.\n");
			continue; // 数据格式不符合预期，跳过当前循环
		}

		/* 将 bufx 中的数据转换为温度 */
		temp = ((bufx[0] - '0') * 10) + (bufx[1] - '0');
		/*清空原本的onenet数据流缓冲区，把新数据写进去*/
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"Temp",temp);
		sleep(1);
		printf("SEND : %s \n", bufx); // 打印
	}

	/*关闭socket连接*/
	TcpClientClose();
	/* 等待线程退出 */
    pthread_join(thread,NULL);

	return 0;
}
