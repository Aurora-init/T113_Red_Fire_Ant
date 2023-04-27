#include "Uart.h"

int set_serial_uart(int ser_fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios new_cfg,old_cfg;
		
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if	(tcgetattr(ser_fd, &old_cfg) != 0)
	{
		perror("tcgetattr");
		return -1;
	}
	
	bzero( &new_cfg, sizeof(new_cfg));
	
	new_cfg = old_cfg; 		/* 设置字符大小*/
	cfmakeraw(&new_cfg); 	/* 配置为原始模式 */ 

    /* 设置波特率 */
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&new_cfg, B2400);
        cfsetospeed(&new_cfg, B2400);
        break;
    case 4800:
        cfsetispeed(&new_cfg, B4800);
        cfsetospeed(&new_cfg, B4800);
        break;
    case 9600:
        cfsetispeed(&new_cfg, B9600);
        cfsetospeed(&new_cfg, B9600);
        break;
    case 115200:
        cfsetispeed(&new_cfg, B115200);
        cfsetospeed(&new_cfg, B115200);
        break;
    default:
        cfsetispeed(&new_cfg, B9600);
        cfsetospeed(&new_cfg, B9600);
        break;
    }
	
	//用于本地连接和接受使能
	new_cfg.c_cflag |= CLOCAL | CREAD;

	/* 数据位选择 */
	new_cfg.c_cflag &= ~CSIZE;    
    switch (nBits)
    {
    case 7:		/*7位数据位*/
        new_cfg.c_cflag |= CS7;
        break;
    case 8:		/*8位数据位*/
        new_cfg.c_cflag |= CS8;
        break;
    }

	/* 设置奇偶校验位 */
	switch (nEvent)
	{
	case '0':								  /* 奇校验 */
		new_cfg.c_cflag |= PARENB; 		  /* 开启奇偶校验 */
		new_cfg.c_iflag |= (INPCK | ISTRIP); /*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
		new_cfg.c_cflag |= PARODD; 		  /*启用奇校验(默认为偶校验)*/
		break;
	case 'E':								  /*偶校验*/
		new_cfg.c_cflag |= PARENB; 		  /*开启奇偶校验	*/
		new_cfg.c_iflag |= (INPCK | ISTRIP); /*打开输入奇偶校验并去除字符第八个比特*/
		new_cfg.c_cflag &= ~PARODD;		  /*启用偶校验*/
		break;
	case 'N': /*无奇偶校验*/
		new_cfg.c_cflag &= ~PARENB;
		break;
	}

    /*设置停止位*/
    if (nStop == 1) /* 设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则**CSTOPB */
    {
        new_cfg.c_cflag &= ~CSTOPB; /*默认为一位停止位； */
    }
    else if (nStop == 2)
    {
        new_cfg.c_cflag |= CSTOPB; /* CSTOPB表示送两位停止位 */
    }
	
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	new_cfg.c_cc[VTIME] = 0; //等待时间
	new_cfg.c_cc[VMIN] = 1;  //最少接收字节数
	tcflush ( ser_fd, TCIOFLUSH);

    /*串口设置使能*/
    if ((tcsetattr(ser_fd, TCSANOW, &new_cfg)) != 0)
    {
        perror("com set error");
        exit(1);
    }
}

