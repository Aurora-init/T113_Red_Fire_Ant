## DHT11驱动(单总线设备)程序编写





实现步骤：

1.确定使用的IO口并准备硬件

2.修改设备树

3.裁剪LINUX内核，编译并烧写

4.DHT11驱动编写



## 1.确定使用的IO口并准备硬件

在之前飞线LED的IO口时，DHT11的IO我也一并飞了，56号引脚，PD14。

![image-20230411093539499](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230411093539499.png)

![image-20230323165705623](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230323165705623.png)



## 2.修改设备树



### 2.1.设备树修改实操:

```bash
cd /buildroot-100ask_t113-pro/buildroot/output/build/linux-origin_master/arch/arm/boot/dts

sudo gedit sun8iw20p1-t113-100ask-t113-pro.dts
```



代码块1：挂载dht11设备节点到根设备节点上

```
/*添加DHT11的设备树文件*/
	dht11 { /*定义了一个名称为"dht11"的节点。*/
		compatible = "dht-11";/*属性指定了该节点与哪种设备兼容，这里的 “dht-11” 表示该节点与 DHT11 温湿度传感器设备兼容*/
		pinctrl-names = "default";
		pinctrl-1 = <&dht11_pin>;/*属性指向了名为 “dht11_pin” 的 pin control 节点，该节点包含了与 “default” pin control 关联的硬件引脚配置信息。即代码块2信息*/
		dht11-gpios = <&pio PD 14 GPIO_ACTIVE_HIGH>; /*属性指定了 DHT11 节点使用的 GPIO 引脚，其中包括了引脚号（PD 14）、设备名称（pio）和电平状态（GPIO_ACTIVE_HIGH）*/
		status = "okay"; 
	}; 
```

代码块2：pio设置

```
dht11_pin: dht11_pin{
		allwinner,pins = "PD14"; /*dht11的时钟和数据线接PD14上面*/
	};
```

在根设备节点下添加挂载dht11设备节点：

![image-20230411094229616](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230411094229616.png)

## 3.裁剪LINUX内核，编译并烧写

### 3.1.裁剪内核

为了驱动 DHT11 设备，Linux 内核需要支持 GPIO 和 I2C 子系统。因此，如果要进行内核裁剪，需要保留这两个子系统。

内核菜单路径：

- GPIO 子系统：
  - Device Drivers -> GPIO Support
- I2C 子系统：
  - Device Drivers -> I2C Support

### 3.2.编译内核

进入到/buildroot目录下，输入以下两条命令即可在/output/images目录下找到生成的sd卡镜像，该命令用到了buildroot框架。

```bash
make linux-rebuild V=1 
make V=1
```

### 3.3.烧写SD卡

不演示



## 4.DHT11驱动编写



### 4.1.I2C子系统





















