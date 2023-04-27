

# 代码使用说明



## 代码介绍

![image-20230426224241411](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230426224241411.png)

一共有5部分代码，其中

_bluez_part为bluz代码，可执行文件名为"bt_app"

_edp_part为T113连接onenet的应用部分代码，可执行文件名为"edp"

_fork_edp_and_bluez为进程创建代码，用于创建两个进程分别执行bt_app及edp两个可执行文件

_uniapp_part为uniapp开发的安卓APP，目前功能有登录界面(账号密码都为1)，下拉onenet的api控制设备，用户退出界面等。

_zigbee_part为zigbee代码，这份代码包含协调器，路由器，终端设备的代码，用于路由器或终端设备采集温湿度光照数据发送回协调器，协调器再通过串口发送到T113开发板。



## 技术框图![技术框图](D:\桌面\buildroot_project\22_Red_Ant\技术框图.png)
