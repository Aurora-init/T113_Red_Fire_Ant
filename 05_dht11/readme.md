## DHT11驱动(单总线设备)程序编写





实现步骤：

1.确定使用的IO口并准备硬件

2.修改设备树

3.裁剪LINUX内核，编译并烧写

4.DHT11驱动编写



## 1.确定使用的IO口并准备硬件

在之前飞线LED的IO口时，DHT11的IO我也一并飞了，56号引脚。

![image-20230411093539499](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230411093539499.png)

![image-20230323165705623](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230323165705623.png)



## 2.修改设备树



