#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define SSID_NAME "Ace2" 			//热点名称
#define WIFI_PASSWORD "123780807"	//热点密码


/*这代码是用来开启WiFi模块连接我手机热点的一个类似脚本的命令程序，无法实现命令是否执行成功，也还有很多问题*/

void wifi_connect(void)
{
	struct timespec req;
	req.tv_sec  = 0;		/* 0s */
	req.tv_nsec = 20000000; /* 20000000ns = 20ms */
	
	char command[256];
	int ret;

	/*1.配置WiFi信道名和密码*/
	sprintf(command, "rm -rf /etc/wpa_supplicant.conf");
    ret = system(command);
	nanosleep(&ts, NULL);
	sprintf(command, "wpa_passphrase %s %s >> /etc/wpa_supplicant.conf", SSID_NAME,WIFI_PASSWORD);
    ret = system(command);
	nanosleep(&ts, NULL);

	/*2.每次执行modprobe xr829命令都会生成一个网卡端口的配置文件，我们查找该配置文件，来查询是否执行过modprobe xr829命令*/
	sprintf(command, "find /etc/udev/rules.d/ -name 70-persistent-net.rules");
    ret = system(command);
	if(ret == 0){  //如果找到了，说明装载过WiFi芯片的驱动，需要把新生成的配置文件覆盖掉
		/*因为每次装载xr829 wifi 驱动模块都会重命名Wlan网口名，所以把配置文件复制过来*/
		sprintf(command, "cp -f /root/70-persistent-net.rules /etc/udev/rules.d/");
    	ret = system(command);
		nanosleep(&ts, NULL);
		#define WANLX "wlan1"				//网卡端口
	}else { //没装过
		#define WANLX "wlan0"				//网卡端口
	}
	nanosleep(&ts, NULL);
	
	/*3.安装 xr829 wifi 驱动模块*/
    sprintf(command, "modprobe xr829");
    ret = system(command);
	sleep(1);	
	
	/*wpa_supplicant 建立网络连接*/
	sprintf(command, "wpa_supplicant -B -i%s -c /etc/wpa_supplicant.conf",WANLX);
    ret = system(command);
	sleep(1);

	/*查看连接状态*/
	sprintf(command, "iw %s link",WANLX);
    ret = system(command);
	sleep(1);

	/*获取 WiFi 网卡的 IP 地址*/
	sprintf(command, "udhcpc -i %s",WANLX);
    ret = system(command);
	sleep(1);

	/*关掉内核打印信息*/
	sprintf(command, "dmesg -n 1");
    ret = system(command);
}

