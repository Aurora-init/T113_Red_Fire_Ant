#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



int main(int argc, char **argv)
{
	int fd;
	char val;
	unsigned char buf[3];
	float flight;
	fd = open("/dev/gy_sensor", O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/gy_sensor\n");
		return -1;
	}
 
	usleep(200000);
	while(1)
		{
	     if(read(fd,&buf,3)){
	         flight=(buf[0]*256+buf[1])*0.5/1.2;
	         printf("light: %6.2flx\r\n",flight);
	     }
	     else{
	         printf("read err!\r\n");
	     }
	    sleep(4);
	}
 
	return 0;
}

