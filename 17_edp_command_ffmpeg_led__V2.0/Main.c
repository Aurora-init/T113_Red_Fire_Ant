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
#include "wifi_connect.h"
#include "ConnectOneNet.h"
#include "gps_analyse.h"
#include "Uart.h"

//代码更改需求(从简到难)
//1.把代码功能模块化，封装while循环内的代码内容，创建wifi_connect.h
//2.把GPS数据也接入ONENET
//3.代码稳定运行，完善uniapp代码

/*a.数据流变量结构体*///温度、湿度、光照强度、经度/纬度、
typedef struct parmanent
{
    char temp;						//温度变量
	char humi;						//湿度变量
	char light;						//光照强度变量
	float test_value;
} onenet_parmanent;


void Hal_ONENET_Init(void);

/*******************************************START多线程1之接收线程START**************************************************/
void *OneNetReacv(void *arg)
{
	/*保存接收的字符串*/
	char RecvBuffer[100]; 
	
	/*Linux命令缓冲区*/
	char command[256];
	int ret;

	/*当接收到onenet平台的数据就打印出来，后面也可以用这个线程来实验一些远程控制的操作*/
	while(1)
	{
		memset(RecvBuffer ,0 ,sizeof(RecvBuffer));
		OneNet_RecvData((void*)&sockfd , RecvBuffer);
		printf("RecvBuffer:%s\n", RecvBuffer);

		/* 查找命令部分的起始位置和长度 */
		char* cmdStart = strstr(RecvBuffer, "{\"LEDSET\":\"");
		if (cmdStart == NULL) {
			/* 如果命令部分不存在，则执行默认操作 */
			continue;
		}

		cmdStart += strlen("{\"LEDSET\":\"");
		int cmdLen = strcspn(cmdStart, "\"}");

		/* 复制命令部分到一个新的缓冲区 */
		char cmdBuffer[20]; // 假设命令部分的最大长度为20
		strncpy(cmdBuffer, cmdStart, cmdLen);
		cmdBuffer[cmdLen] = '\0';

		/* 根据提取出的命令执行相应的操作 */
		if (strcmp(cmdBuffer, "0") == 0) {
			 /* 2.灭灯 */
			sprintf(command, "echo 0 > /sys/class/leds/red/brightness");
			ret = system(command);
		} else if (strcmp(cmdBuffer, "1") == 0) {
			/* 1.点灯 */
			sprintf(command, "echo 1 > /sys/class/leds/red/brightness");
			ret = system(command);
		}
	}
}


/*******************************************END多线程1之接收线程END**************************************************/

/*程序实现*/
/*
0.装载WiFi模块,连接热点,初始化Onenet
1.创建一个多线程用于接收从onenet云平台下发的数据
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

	/**************************Main函数变量（START）**************************/
	/*函数返回值变量*/
	int ret;
	
	/*传感器数据变量*/
	onenet_parmanent onenet_value;

	/*GPS信息数据变量*/
	GNGGA gngga_buf;
	
	
	/*onenet数据流缓冲区变量*/
	char buf[20] = {0};				//onenet数据流缓冲
	int onenet_send_state = 0;      //onenet数据发送状态位
	/**************************Main函数变量（END）**************************/




	/*0.装载WiFi模块,连接热点,初始化Onenet*/
	Hal_ONENET_Init();

	/*1.创建一个多线程用于接收从onenet云平台下发的数据*/
	pthread_t thread;				//副线程句柄	
	ret = pthread_create(&thread, NULL,OneNetReacv, NULL);
	if (ret){
		printf("pthread_create err!\n");
		return -1;
	}
	
	/**************************A.UART配置部分（START）**************************/
	/*串口0变量*/
	char uart_buf0[1024];	//串口数据缓冲区
	int fd_uart0 = 0;		//串口0设备文件描述符	
	/*串口1变量*/
	char uart_buf1[1024];	//串口数据缓冲区
	int fd_uart1 = 0;		//串口1设备文件描述符
	
	/*2.打开串口*/
	/*2.1.使用 open() 函数打开串口设备文件。*/
	/*O_RDWR | O_NOCTTY | O_NDELAY | O_CLOEXEC：1.打开文件时使用读写模式、
												2.不将打开的设备文件作为控制终端设备、
												3.非阻塞模式、
												4.打开文件时将文件描述符标记为“关闭时执行”*/

	fd_uart0 = open("/dev/ttyUSB0" , O_RDWR|O_NOCTTY);//串口0设置功能为：1.打开文件时使用读写模式、2.不将打开的设备文件作为控制终端设备、
	fd_uart1 = open("/dev/ttyUSB1" , O_RDWR|O_NOCTTY);//串口1设置功能为：1.打开文件时使用读写模式、2.不将打开的设备文件作为控制终端设备、（看情况设置：3.非阻塞模式、）
	if(fd_uart0 < 0 && fd_uart1 < 0)
	{
		perror("open /dev/ttyUSB0 or /dev/ttyUSB1 failed");
		return 0;
	}
	/*2.2.配置串口相关参数，包括波特率、数据位、停止位、校验位等*/
	set_serial_uart(fd_uart0, 115200, 8, 'N', 1);
	set_serial_uart(fd_uart1,  9600 , 8, 'N', 1);
	/**************************A.UART配置部分（END）**************************/

	while(1)
	{
		/*2.3.使用 read() 函数从串口接收数据。*/
		memset(uart_buf0, 0, sizeof(uart_buf0));//清空uart_buf0缓冲区
		ret = read(fd_uart0 , uart_buf0 , sizeof(uart_buf0)-1); // sizeof(uart_buf0)-1是留一个位置给'\0'
		if(ret < 0 ){	
			perror("Read failed");
			break;
		} else if (ret == 0) { //增加错误处理，当读取到文件结尾（EOF）时，应该退出循环并关闭文件。
			printf("End of file reached\n");
			close(fd_uart0);
			exit(EXIT_SUCCESS);
		} else { //read函数返回读取的字节数，这里是在读取到的数据末尾的位置添加一个字符串终止符（'\0'）
			uart_buf0[ret] = '\0';
		}
		
		memset(uart_buf1, 0, sizeof(uart_buf1));//清空uart_buf1缓冲区
		ret = read(fd_uart1 , uart_buf1 , sizeof(uart_buf1)-1); // sizeof(uart_buf1)-1是留一个位置给'\0'
		if(ret < 0 ){	
			perror("Read failed");
			break;
		} else if (ret == 0) { //增加错误处理，当读取到文件结尾（EOF）时，应该退出循环并关闭文件。
			printf("End of file reached\n");
			close(fd_uart1);
			exit(EXIT_SUCCESS);
		} else { //read函数返回读取的字节数，这里是在读取到的数据末尾的位置添加一个字符串终止符（'\0'）
			uart_buf1[ret] = '\0';
		}
		
		/* 验证 uart_buf0 中的数据是否符合预期格式 */		
		if (strlen(uart_buf0) > 6 ) { //这里是只要了前6个数，但也出现了一种情况，就是串口发送的数据总长度小于6，也被筛出去了，所以得改成>6
    		printf("Error: Invalid data length.\n");
    		continue; // 数据长度不符合预期，跳过当前循环
		}
		if (!isdigit(uart_buf0[0]) || !isdigit(uart_buf0[1]) || !isdigit(uart_buf0[2]) || !isdigit(uart_buf0[3]) || !isdigit(uart_buf0[4]) ) { //这里只检测前5位是否为数字，如果第6位不为数字就说明光照强度很低，要单独拉出来处理
			printf("Error: Invalid data format.\n");
			continue; // 数据格式不符合预期，跳过当前循环
		}

		/*3.把串口数据处理后放到buf数据缓冲区中上传到onenet*/
		/*3.1.准备要传到onenet上的数据*/
		/*3.1.1.准备串口0获得的温度（temp）、湿度（humi）、光照强度数据（light）*/
		onenet_value.temp = ((uart_buf0[0] - '0') * 10) + (uart_buf0[1] - '0');
		onenet_value.humi = ((uart_buf0[2] - '0') * 10) + (uart_buf0[3] - '0');
		if (!isdigit(uart_buf0[5])) { //如果第6位数据不为数字
			onenet_value.light = (uart_buf0[4] - '0');
		}else{
			onenet_value.light = ((uart_buf0[4] - '0') * 10) + (uart_buf0[5] - '0');
		}

		/*3.1.2.准备串口1获得的GPS经度、维度数据*/
		memset(&gngga_buf, 0, sizeof(gngga_buf));	//清空gngga_buf结构体
		gps_analyse(uart_buf1, &gngga_buf); 		//将数据放进gngga_buf结构体，这个结构里暂时存储了解析后的GPS数据

		
		/*3.2.清空原本的onenet数据流缓冲区，把新数据写进去，发送到onenet平台*/
		switch(onenet_send_state)
		{
		case 0://状态0发送TEMP数据
			memset(buf ,0 ,sizeof(buf));
			OneNet_SendData(buf,"Temp",onenet_value.temp);
			sleep(1);
			printf("Send Temp: %d \n", onenet_value.temp); // 打印
			onenet_send_state = 1;
		case 1://状态1发送HUMI数据
			memset(buf ,0 ,sizeof(buf));
			OneNet_SendData(buf,"Humi",onenet_value.humi);
			sleep(1);
			printf("Send Humi: %d \n", onenet_value.humi); // 打印
			onenet_send_state = 2;
		case 2://状态2发送LIGH数据
			memset(buf ,0 ,sizeof(buf));
			OneNet_SendData(buf,"ligh",onenet_value.light);
			sleep(1);
			printf("Send LIGH: %d \n", onenet_value.light); // 打印
			onenet_send_state = 3;
		case 3://状态3发送数据分割线
			print_gps(&gngga_buf);         						 //打印GPS数据
			printf("*************Send UART End*************\n"); // 打印
			printf("*************Send UART End*************\n"); // 打印
			onenet_send_state = 0;
		}

	}

	/*关闭socket连接*/
	TcpClientClose();
	
	/* 等待线程退出 */
    pthread_join(thread,NULL);
	
	return 0;
}



void Hal_ONENET_Init(void)
{
	/*0.完成安装WIFI驱动,连接热点等一系列操作*/
	wifi_connect();

	/*1.onenet初始化*/

	/*onenet初始化*/
	OneNet_Init();
	printf("OneNet Init Success!\n");	
}

