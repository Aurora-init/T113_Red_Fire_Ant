

# 代码使用说明

[TOC]



## 代码介绍

![image-20230529224849411](image-20230529224849411.png)

（1）T113设备上运行的代码：(运行前提：1.curl及ffmpeg移植，2.将以下的四个可执行文件都放到设备/root目录下）

1.**_bluez_part**：可执行文件名为"**bt_app**"，目前只有一个读服务功能和一个写服务功能，

2.**_edp_part**：可执行文件名为"**edp**"，T113连接onenet的应用部分代码，用于完成onenet与T113设备的联合

3.**_fork_edp_and_bluez**：可执行文件名为"**main**"，创建进程"**bt_app**"和"**edp**"

4.**_photo_upload_part**：可执行文件名为"**photo_upload**"，将设备拍摄的照片上传至onenet平台



（2）手机APP

**_uniapp_part**：uniapp开发的安卓APP，目前功能有：1.登录界面(账号密码都为1)、2.下拉onenet的api控制设备、3.用户退出界面、



（3）ZIGBEE设备上运行的代码：

**_zigbee_part**：zigbee代码，这份代码包含协调器，路由器，终端设备的代码，用于路由器或终端设备采集温湿度光照数据发送回协调器，协调器再通过串口发送到T113开发板。具体怎么用得你们自己了解如何烧录代码了



（4）PC上(Linux)运行的代码：

**_photo_download_part**：可执行文件名为"**photo**"，用于将onenet平台上的图片下载到PC端并恢复成图片

## 0.技术框图![技术框图](D:\桌面\buildroot_project\22_Red_Ant\技术框图.png)

## 1._bluez_part

![image-20230505150632163](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505150632163.png)

这部分代码，主要是实现了蓝牙模块的驱动，蓝牙模块主要实现了如下功能：

1.读服务，我是直接读取/root/GPS.txt的内容，后续可以想一下如何改进

![image-20230505151755292](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505151755292.png)

1.写服务，更改WiFi密码的格式为**"WiFi名称,WiFi密码"**，例如:"Ace2,123780807"，Ace2为WiFi名称，123780807为WiFi密码

![image-20230505151919334](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505151919334.png)

## 2._edp_part

![image-20230505152130386](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505152130386.png)

这部分代码，主要是设备与云平台(OneNet)的交互中间层代码，主要实现了如下功能：

### 2.1.主线程

1.串口接收ZIGBEE协调器设备串口数据，协调器设备数据主要包含路由器设备及终端设备通过Mesh网传输过来的**温湿度**、**光照强度**数据

![image-20230505154441735](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154441735.png)

2.串口接收GPS数据，GPS数据将被解析并存放至**gngga_buf**结构体中，结构体成员详见**gps_analyse.c**

![image-20230505154322344](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154322344.png)

3.根据时间及光照强度，在17:00开始进行5分钟的拍照

![image-20230505154545633](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154545633.png)

4.将温度、湿度、光照强度、经纬度数据上传至OneNet

![image-20230505154613892](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154613892.png)

### 2.1.副线程

1.根据OneNet云平台下发的数据，实现补光灯的远程控制，命令格式为**{"LEDSET":"1"}**/***{"LEDSET":"0"}***

![image-20230505154646987](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154646987.png)

2.根据OneNet云平台下发的数据，实现相机的远程控制，命令格式为**{"CAMERA":"1"}**/**{"CAMERA":"0"}**

![image-20230505154703353](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230505154703353.png)
