#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*这代码是用来开启WiFi模块连接我手机热点的一个类似脚本的命令程序，无法实现命令是否执行成功，也还有很多问题*/
/*驱动WIFI模块的命令(我也只学了命令的执行方法)
0.modprobe -r xr829 													//卸载驱动
1.modprobe xr829                                              			//安装 xr829 wifi 驱动模块
2.rm -rf /etc/wpa_supplicant.conf							  			//删除系统自带的配置文件
3.wpa_passphrase WiFi_SSID_Name password >> /etc/wpa_supplicant.conf    //设置 WiFi SSID 信道名和密码

//这句命令的运行就涉及到一点逻辑了
//因为modprobe xr829会在/etc/udev/rules.d创建一个70-persistent-net.rules，而每次对设备掉电都会导致xr829 wifi驱动模块被remove，所以每次上电都需要再执行modprobe xr829安装驱动
//而当/etc/udev/rules.d中有70-persistent-net.rules后，其内的内容会强制递进更换NAME属性

如原本内容：
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="dc:44:6d:ce:d3:77", ATTR{dev_id}=="0x0", ATTR{type}=="1", KERNEL=="wlan*", NAME="wlan0"

执行modprobe xr829后：

SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="dc:44:6d:ce:d3:77", ATTR{dev_id}=="0x0", ATTR{type}=="1", KERNEL=="wlan*", NAME="wlan0"
SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="dc:44:6d:7e:10:67", ATTR{dev_id}=="0x0", ATTR{type}=="1", KERNEL=="wlan*", NAME="wlan1"

这就意味着原本的命令变成了wpa_supplicant -B -iwlan1 -c /etc/wpa_supplicant.conf

那怎么解决这个问题呢？
方法1.备份原本的70-persistent-net.rules，每次执行modprobe xr829后都把这个原本的70-persistent-net.rules覆盖新生成的70-persistent-net.rules
(但这个方法有个问题，就是如果没执行过modprobe xr829，执行modprobe xr829后，后面对应的WALNname是wlan0，并且，如果没有备份70-persistent-net.rules也没办法用这个办法)

方法2.通过find /etc/udev/rules.d/ -name 70-persistent-net.rules | grep -c .命令来查找70-persistent-net.rules是否存在
	  如果存在，则返回值大于0，说明执行过modprobe xr829，那就需要把原本的文件覆盖掉，前面说一般别人执行都是没有备份文件的，那么就自己创建一个
4.wpa_supplicant -B -iwlan0 -c /etc/wpa_supplicant.conf 				

5.iw wlan0 link

6.udhcpc -i wlan0

7.dmesg -n 1
*/
/**/

//草稿命令（创建一个立马能用的70-persistent-net.rules）
//sh -c 'echo "SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"dc:44:6d:7e:10:67\", ATTR{dev_id}==\"0x0\", ATTR{type}==\"1\", KERNEL==\"wlan*\", NAME=\"wlan1\"" > ./70-persistent-net.rules'

void wifi_connect(void)
{
	char command[256];
	int result;

	char ssid_name[100];
	char wifi_password[100];

	// 定义要写入的固定内容
    char content[] = "SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"dc:44:6d:7e:10:67\", ATTR{dev_id}==\"0x0\", ATTR{type}==\"1\", KERNEL==\"wlan*\", NAME=\"wlan0\"";

	// 检查文件是否存在，如果不存在则创建文件并写入初始数据
    if (access("/root/wifi.txt", F_OK) == -1) {
        FILE *fp = fopen("/root/wifi.txt", "w");

        if (fp == NULL) {
            printf("无法创建文件wifi.txt\n");
        }

        fprintf(fp, "Ace2,123780807");

        fclose(fp);
    }
	// 打开wifi.txt文件
    FILE *fp = fopen("/root/wifi.txt", "r");

    if (fp == NULL) {
        printf("无法打开文件wifi.txt\n");
    }

    // 从文件中读取一行数据，格式为 "ssid_name,wifi_password"
    fscanf(fp, "%[^,],%s", ssid_name, wifi_password);

    // 关闭文件
    fclose(fp);

	/*3.把原本的70-persistent-net.rules删掉，把写好的内容扔进去*/
	sprintf(command, "rm -f /etc/udev/rules.d/70-persistent-net.rules");
	result = system(command);
	sprintf(command, "touch -f /etc/udev/rules.d/70-persistent-net.rules");
	result = system(command);
	sprintf(command, "echo '%s' | tee -a /etc/udev/rules.d/70-persistent-net.rules > /dev/null", content);
	result = system(command);
	sleep(1);

	/*1.modprobe xr829*/
	sprintf(command, "modprobe xr829");
	result = system(command);
	sleep(1);	

	/*2.配置WiFi信道名和密码*/
	sprintf(command, "rm -rf /etc/wpa_supplicant.conf");
    result = system(command);
	sprintf(command, "wpa_passphrase %s %s >> /etc/wpa_supplicant.conf", ssid_name,wifi_password);
    result = system(command);
	sleep(1);
	
	/*4.wpa_supplicant 建立网络连接*/
	sprintf(command, "wpa_supplicant -B -iwlan1 -c /etc/wpa_supplicant.conf");
    result = system(command);
	sleep(1);

	sprintf(command, "iw wlan1 link");
    result = system(command);
	sleep(1);
	
	sprintf(command, "udhcpc -i wlan1");
    result = system(command);
	sleep(1);

	/*关掉内核打印信息*/
	sprintf(command, "dmesg -n 1");
    result = system(command);
}


