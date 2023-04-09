#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "EdpKit.h"
#include "ConnectOneNet.h"

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

//接收线程
void *OneNetReacv(void *arg)
{
	char RecvBuffer[10] = {0};  //保存接收的字符串

	while(1)
	{
		memset(RecvBuffer ,0 ,sizeof(RecvBuffer));
		OneNet_RecvData((void*)&sockfd , RecvBuffer);
		printf("RecvBuffer:%s\n", RecvBuffer);
	}
}

int main(void)
{	
	//创建一个EDP缓冲区变量
	char num = 0;		char buf[20] = {0};
	//串口变量									//用于存储第一组的温湿度光照强度数据						//记录上一组数据,当数据不符合（传输错误）,则上传上一组数据。
	char bufx[7];		int ret;			char temp;char humi;char ligh;			char temp_o;char humi_o;char ligh_o;

	pthread_t thread;

	/************************ 硬件初始化 **************************/
	//onenet初始化并开启多线程接收命令
	OneNet_Init();			printf("HardWare Init Success!\n");				pthread_create(&thread, NULL,OneNetReacv, NULL);
	
	//打开串口 0
	int fd = open("/dev/ttyUSB0" , O_RDWR|O_NOCTTY);

	if(fd < 0)
	{
		perror ("open failed");
		return 0;
	}

	set_serial_uart(fd);

	//while循环
	while(1)
	{
		bzero(bufx, 10);
		ret = read(fd , bufx , 10-1); // 留一个位置给‘\0'
		if(ret <= 0 )
		{
			perror("write failed");
			break;
		}
		//发送数据给 OneNet 平台
		//串口传输的数据是ASCII码值，需要减48
		temp = ((bufx[0]-48)*10)+(bufx[1]-48);
		humi = ((bufx[2]-48)*10)+(bufx[3]-48);
		ligh = ((bufx[4]-48)*10)+(bufx[5]-48);

		//如果数据出现了不正常的情况，就清空串口缓冲区，并且用上一次的旧数据代替新数据
		if(temp>40||humi>70||ligh>100)
		{
			memset(bufx ,0 ,sizeof(bufx));
			temp = temp_o;
			humi = humi_o;
			ligh = ligh_o;
		}
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"Temp",temp);
		sleep(1);
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"Humi",humi);
		sleep(1);
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf,"ligh",ligh);
		sleep(1);

		//把正常的这一次记录下来
		temp_o = temp;
		humi_o = humi;
		ligh_o = ligh;
		
		printf("SEND : %s \n", bufx); // 打印
	}

	/*关闭socket连接*/
	TcpClientClose();
	/* 等待线程退出 */
    pthread_join(thread,NULL);

	return 0;
}
