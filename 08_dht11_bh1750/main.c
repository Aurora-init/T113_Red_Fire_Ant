#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <stdint.h>

/*设备名宏定义*/
#define DEV_NAME "/dev/dht11"

/*结构体变量————DHT11*/
typedef struct dht11
{
    float temperature;
    float humidity;
} DHT11;

/*结构体变量————BH1750*/
typedef struct bh1750
{
    float flight;
} BH1750;

/*
函数名：void sleep_ms(unsigned int ms)
入口参数：ms(延时毫秒数)
返回值：无
*/
void sleep_ms(unsigned int ms)
{
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;
    select(0, NULL, NULL, NULL, &delay);
}
/*DHT11本体函数*/
/*
函数名：int getDht11(DHT11 *dht11_data)
入口参数：DHT11 *dht11_data(dht11的结构体变量，用于储存dht11温湿度数据)
返回值：异常值或正常运行
*/
int getDht11(DHT11 *dht11_data)
{
    int fd;
    int ret;

    /* 2. 打开文件 */
    fd = open(DEV_NAME, O_RDONLY); // | O_NONBLOCK

    if (fd < 0)
    {
        printf("[failed] can not open file %s, %d\n", DEV_NAME, fd);
        return -1;
    }

    uint8_t dht11_temp_data[5];
    int timeout = 5;
    while (timeout)
    {
        ret = read(fd, dht11_temp_data, sizeof(dht11_temp_data)) == sizeof(dht11_temp_data);
        if (ret)
        {
            sleep_ms(500);
            ret = read(fd, dht11_temp_data, sizeof(dht11_temp_data)) == sizeof(dht11_temp_data);
            if (ret)
            {
                dht11_data->temperature = dht11_temp_data[2] + (float)dht11_temp_data[3] / 10.00;
                dht11_data->humidity = dht11_temp_data[0] + dht11_temp_data[1] / 10.00;
                printf("[success] temperture %d.%d  humi %d.%d\r\n", dht11_temp_data[2],
                       dht11_temp_data[3], dht11_temp_data[0], dht11_temp_data[1]);
                close(fd);
                return 0;
            }
            else
                continue;
        }
        else
        {
            printf("[failed] tempget temp err %d\n", ret);
            timeout--;
        }
        sleep_ms(500);
    }
    close(fd);
    return -1;
}

int getBH1750(BH1750 *bh1750_data)
{
	int fd;
	char val;
	unsigned char buf[3];
	
	/* 2. 打开文件 */
	fd = open("/dev/gy_sensor", O_RDWR);

	if (fd < 0)
	{
		printf("can't open /dev/gy_sensor\n");
		return -1;
	}
	sleep_ms(200);
	if(read(fd,&buf,3))
	{
		bh1750_data->flight = (buf[0]*256+buf[1])*0.5/1.2;
		//printf("light: %6.2flx\r\n",bh1750_data->flight);
	}
	else
	{
		printf("read err!\r\n");
	}
	sleep(4);
	close(fd);
    return -1;
}

int main(int argc, char **argv)
{
    DHT11 dht11_data;
	BH1750 bh1750_data;
    if (!getDht11(&dht11_data)&&!getBH1750(&bh1750_data))
    {
        printf("!!! temp %.1f  humi %.1f\n", dht11_data.temperature, dht11_data.humidity);
		printf("light: %6.2flx\r\n",bh1750_data.flight);
    }
    else
        printf("read dht11 error\n");
    return 0;
}

