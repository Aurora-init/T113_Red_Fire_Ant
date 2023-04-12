# BH1750驱动(I2C设备)程序编写



实现步骤：

1.确定使用的IO口并准备硬件

2.修改设备树

3.裁剪LINUX内核，编译并烧写

4.BH1750驱动编写



## 1.确定使用的IO口并准备硬件

I2C接口的话，开发板上已经准备好了引脚引出，直接用就可以

![image-20230412113927376](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230412113927376.png)

![image-20230323165705623](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230323165705623.png)

## 2.修改设备树

### 2.1.设备树修改实操:

```
cd /buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master/arch/arm/boot/dts

sudo gedit sun8iw20p1-t113-100ask-t113-pro.dts
```

代码块1：挂载light-sensor@23设备节点到twi2设备节点上

```dtd
light-sensor@23 {
		compatible = "gy,bh1750";
		reg = <0x23>;
	};
```

![image-20230412120558562](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230412120558562.png)

## 3.裁剪LINUX内核，编译并烧写

### 3.1.裁剪内核

1. I2C总线支持：Device Drivers --> I2C support --> <*> I2C Hardware Bus support
2. I2C设备驱动程序支持：Device Drivers --> I2C support --> <*> I2C device interface
3. SYSFS支持：File systems --> Pseudo filesystems --> <*> sysfs file system support
4. 工作队列支持：Kernel Hacking --> Kernel Debugging --> [*] Debug Workqueue usage
5. 电源管理支持：Power management and ACPI options --> [*] ACPI (Advanced Configuration and Power Interface) Support --> <*> ACPI Support
6. 模块化支持：Loadable module support --> <*> Enable loadable module support

一般默认都添加上了，如果不放心可以检查一下

### 3.2.编译内核

进入到/buildroot目录下，输入以下两条命令即可在/output/images目录下找到生成的sd卡镜像，该命令用到了buildroot框架。

```bash
make linux-rebuild V=1 
make V=1
```

### 3.3.烧写SD卡

不演示



## 4.BH1750驱动编写

作为I2C设备，可以说学会了I2C驱动的编写，你才能说你大致学会了驱动开发的代码怎么写

### 4.1.BH1750基础知识





还是这七步：

/* 1. 确定主设备号*/

/* 2. 定义file_operations结构体*/

/* 3. 实现对应的open/read/write等函数，填入file_operations结构体 */

/* 4. 把file_operations结构体告诉内核：注册驱动程序*/

/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时就会去调用这个**入口函数***/

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调这个**出口函数***/

/* 7. 其他完善：提供设备信息，自动创建设备节点*/



### 4.2.bh1750_drv.c逐帧解析

/* 1. 确定主设备号*/

```c++
struct class *class;    /* 类 		*/
int major;              /* 主设备号 */
struct i2c_client *gy_sensor_client;
```

/* 2. 定义file_operations结构体*/

```c++
/* bh1750 操作函数 */
static const struct file_operations gy_sensor_fops = {
    .owner = THIS_MODULE,
    .open = gy_sensor_open, 
    .read = gy_sensor_read, 
};
```

/* 3. 实现对应的open/read/write等函数，填入file_operations结构体 */



/* 4. 把file_operations结构体告诉内核：注册驱动程序*/

/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时就会去调用这个**入口函数***/

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调这个**出口函数***/

/* 7. 其他完善：提供设备信息，自动创建设备节点*/

