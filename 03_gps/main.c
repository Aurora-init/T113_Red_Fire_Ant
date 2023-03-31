#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "gps_analyse.h"
#include "Uart.h"

#define GPS_LEN 1024

int main(int argc, char **argv)
{
    int fd = 0;
    int n = 0;
	
    GNGGA gngga;
	
    char buff[GPS_LEN];
	
	// 打开USB转串口设备
    char *dev_name = "/dev/ttyUSB0"; 
    if ((fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        perror("Can't Open the ttyUSB0 Serial Port");
        return -1;
    }
    // 设置波特率等数据
    set_serial_uart(fd, 9600, 8, 'N', 1);

    while (1)
    {
        sleep(2);
        if ((n = read(fd, buff, sizeof(buff))) < 0)
        {
            perror("read error");
            return -1;
        }
        printf("buff:%s\n", buff); //打印原始数据
        memset(&gngga, 0, sizeof(gngga));
        gps_analyse(buff, &gngga); //将数据放进结构体
        print_gps(&gngga);         //打印数据
    }
    close(fd);
    return 0;
}
