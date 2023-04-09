#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*这代码是用来开启WiFi模块连接我手机热点的一个类似脚本的命令程序，无法实现命令是否执行成功，也还有很多问题*/
/**/

void wifi_connect(void)
{
	char command[256];
	int result;
	
	//struct timespec req;
	//req.tv_sec  = 0;
	//req.tv_nsec = 20000000; /* 20ms */

	sprintf(command, "cp -f /root/70-persistent-net.rules /etc/udev/rules.d/");
    	result = system(command);
	
	sleep(1);
	
	/*modprobe xr829*/
    	sprintf(command, "modprobe xr829");
    	result = system(command);
	
	sleep(1);	

	/*配置WiFi信道名和密码*/
	//sprintf(command, "rm -rf /etc/wpa_supplicant.conf");
    	//result = system(command);
	//sprintf(command, "wpa_passphrase Ace2 123780807 >> /etc/wpa_supplicant.conf");
    	//result = system(command);
	
	/*wpa_supplicant 建立网络连接*/
	sprintf(command, "wpa_supplicant -B -iwlan1 -c /etc/wpa_supplicant.conf");
    	result = system(command);

	sleep(1);

	sprintf(command, "iw wlan1 link");
    	result = system(command);
	
	sleep(1);
	
	sprintf(command, "udhcpc -i wlan1");
    	result = system(command);
}

