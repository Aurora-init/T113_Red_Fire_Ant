# Linux蓝牙通信(基于BlueZ的C语言BLE蓝牙编程)

参考自https://blog.csdn.net/qq_46079439/article/details/126252232?spm=1001.2014.3001.5502

## 1.获取BlueZ源码并编译

在buildroot目录下通过find 命令查找bluez的包在哪里

```bash
cd /buildroot-100ask_t113-pro/buildroot
find ./* -name "blue*"
```

![image-20230327193055607](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327193055607.png)

`./dl/bluez5_utils/bluez-5.63.tar.xz` 就是我们想要的软件包，将它复制到某个目录下，然后解压

我这里是在/home/book目录下新建了一个bluez目录，把它复制到其中，并解压

```bash
cd /home/book
mkdir bluez
cp -rf ./dl/bluez5_utils/bluez-5.63.tar.xz /home/book/bluez
cd /home/book/bluez
xz -d bluez-5.63.tar.xz
tar -xvf bluez-5.63.tar
```

![image-20230327201729455](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327201729455.png)

进入到*bluez-5.63*目录

```
cd bluez-5.63
```

使用的是以下这个目录 `peripheral`

```
cd peripheral
```

![image-20230327203256029](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327203256029.png)

可以把它当作一个装应用程序的文件夹，虽然可以直接编译，但还是写一个makefile文件比较好

```
sudo gedit Makefile
```

Makefile内容：

![image-20230327220058920](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327220058920.png)

这里就不贴了，在make时会报一个错，代码改好了也放在同级目录下了，编译完出来一个可执行文件如下：

![image-20230327220203672](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327220203672.png)

## 2.可执行文件测试

### 2.1.开发板操作

```bash
echo 0 > /sys/class/rfkill/rfkill0/state
sleep 1
echo 1 > /sys/class/rfkill/rfkill0/state
sleep 1

#绑定设备
hciattach -n ttyS1 xradio > /dev/null 2>&1 &
```

1. `0 > /sys/class/rfkill/rfkill0/state`：将数字0重定向到文件`/sys/class/rfkill/rfkill0/state`，表示关闭rfkill设备号为0的射频开关。
2. `echo 1 > /sys/class/rfkill/rfkill0/state`：将数字1写入文件`/sys/class/rfkill/rfkill0/state`，表示打开rfkill设备号为0的射频开关。
3. `hciattach -n ttyS1 xradio > /dev/null 2>&1 &`：绑定设备，将HCI设备连接到ttyS1端口，使用xradio驱动程序，同时将输出重定向到/dev/null（丢弃输出），并在后台运行。
4. `sleep 1`：让程序暂停1秒钟。



### 2.2.将可执行文件上传至开发板并运行

```
cd /home/book/bulez/bluez-5.63/peripheral
adb push ./bt_app /root
chmod 777 bt_app
./bt_app
```

![image-20230327221937883](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327221937883.png)

<img src="C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327222020035.png" alt="image-20230327222020035" style="zoom: 33%;" />



手机用蓝牙助手搜索bluez设备，可以看到这么一个蓝牙设备，点击connect连接

![image-20230327222158832](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327222158832.png)

连接后T113开发板打印信息如上

<img src="C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327222903110.png" alt="image-20230327222903110" style="zoom:33%;" />

可以看到两个BLE服务，第一个服务是接收数据的服务，数据为：BlueZ Peripheral，第二个服务为空。

## 3.源码分析

### 3.1.BLE名称更改

![image-20230327223550117](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327223550117.png)

在gap.c的第288行可以更改这行代码来更改连接的BLE名称.

### 3.2.BLE服务创建

看到gatt.c这个.c文件内，在210行，这里的bt_uuid16_create创建一个蓝牙服务，不太理解，后面再看看

![image-20230327224418514](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327224418514.png)

其中gap_device_name_read是类似于多线程的那个函数的read_fun

![image-20230327224537025](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327224537025.png)

gatt_db_service_add_characteristic 的原型如下：

```
gatt_db_service_add_characteristic(struct gatt_db_attribute *attrib, --> service
				const bt_uuid_t *uuid,									--> &uuid
				uint32_t permissions,									--> BT_ATT_PERM_READ
				uint8_t properties,										--> BT_GATT_CHRC_PROP_READ
				gatt_db_read_t read_func,								--> 读取，read_func
				gatt_db_write_t write_func,								--> 写入，write_func
				void *user_data)										--> 用户数据？
```

## 4.源码编写

备份一下原来的代码，直接在原本的代码上编写自己的代码即可

![image-20230327225221336](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327225221336.png)

![image-20230327225344233](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327225344233.png)



用以上代码代替原本的populate_gap_service(struct gatt_db *db)程序

![image-20230327225520467](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327225520467.png)

代替原本的gap_device_name_read程序

![image-20230327225732785](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327225732785.png)

增加一个gap_user_write_service程序

![image-20230327230314340](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327230314340.png)

在void gatt_server_start(void)内添加如上内容

![image-20230327230448662](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230327230448662.png)

在static struct gatt_conn *gatt_conn_new(int fd)函数中注释掉bt_att_set_security(conn->att, BT_SECURITY_MEDIUM);这行代码，这行的代码是蓝牙设备与手机通信时的验证配对操作

代码编写完可以去试试了

![image-20230331143942770](C:\Users\11148\AppData\Roaming\Typora\typora-user-images\image-20230331143942770.png)

编译好的代码如上,后续再添加别的东西