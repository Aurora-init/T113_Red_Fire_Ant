#include <linux/ide.h>
#include <linux/module.h>
#include <linux/i2c.h>

struct class *class;    /* 类 		*/
int major;              /* 主设备号 */
struct i2c_client *gy_sensor_client;

// 构造i2c_msg通过这个client调用i2c_tansfer来读写
static int gy_sensor_write_reg(unsigned char addr)
{
    int ret = -1;
    struct i2c_msg msgs;

    printk("gy_sensor_client -> addr=%d\n", gy_sensor_client->addr);
    msgs.addr = gy_sensor_client->addr; //   GY302_ADDR，直接封装于i2c_msg
    msgs.buf = &addr;
    msgs.len = 1;   //长度1 byte
    msgs.flags = 0; //表示写

    ret = i2c_transfer(gy_sensor_client->adapter, &msgs, 1); 
    //这里都封装好了，本来根据i2c协议写数据需要先写入器件写地址，然后才能读
    if (ret < 0)
    {
        printk("i2c_transfer write err\n");
        return -1;
    }
    return 0;
}
static int gy_sensor_read_reg(unsigned char *buf)
{
    int ret = -1;
    struct i2c_msg msg;
    msg.addr = gy_sensor_client->addr; // GY30_ADDR
    msg.buf = buf;
    msg.len = 2;                                            //长度1 byte
    msg.flags = I2C_M_RD;                                   //表示读
    ret = i2c_transfer(gy_sensor_client->adapter, &msg, 1); //这里都封装好了，本来根据i2c协议读数据需要先写入读地址，然后才能读
    if (ret < 0)
    {
        printk("i2c_transfer write err\n");
        return -1;
    }
    return 0;
}

// 初始化光线传感器
int gy_sensor_open(struct inode *inode, struct file *file)
{
    printk("open gy_sensor\n");
    gy_sensor_write_reg(0x01); // power up
    gy_sensor_write_reg(0x11);
    return 0;
}

// 读出传感器的两个字节
static ssize_t gy_sensor_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
    unsigned char addr = 0, data[2];
    gy_sensor_read_reg(data);
    copy_to_user(buf, data, 2);
    return 1;
}

/* bh1750 操作函数 */
static const struct file_operations gy_sensor_fops = {
    .owner = THIS_MODULE,
    .open = gy_sensor_open, 
    .read = gy_sensor_read, 
};

/* 构造一个platform_driver，
其中的of_match_table字段需要与 light-sensor@23 节点的compatible属性值一致，
当匹配时则调用platform_driver的probe函数 */
static const struct of_device_id ids[] =
{
    {.compatible = "gy,bh1750"},
    {  }
};

// 在i2c_driver的probe函数中得到在总线驱动程序中解析得到的i2c_client,
// 并为该光线传感器注册一个字符设备
static int gy_sensor_probe(struct i2c_client *client,
                           const struct i2c_device_id *id)
{
    gy_sensor_client = client;

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    major = register_chrdev(0, "gy_sensor", &gy_sensor_fops);
    class = class_create(THIS_MODULE, "gy_sensor");
    device_create(class, NULL, MKDEV(major, 0), NULL, "gy_sensor"); /* /dev/gy_sensor */
    return 0;
}

// 在platform_driver的remove函数中，注销该字符设备
static int gy_sensor_remove(struct i2c_client *client)
{
    device_destroy(class, MKDEV(major, 0));
    class_destroy(class);
    unregister_chrdev(major, "gy_sensor");

    return 0;
}

/* 分配/设置i2c_driver */
static struct i2c_driver gy_sensor_driver = {
    .driver = {
        .name = "bh1750",
        .owner = THIS_MODULE,
        .of_match_table = ids,
    },
    .probe = gy_sensor_probe,
    .remove = gy_sensor_remove,
};

/*
 * @description	: 驱动入口函数
 */
static int __init bh1750_init(void)
{
    int ret = 0;

    ret = i2c_add_driver(&gy_sensor_driver);
    return ret;
}

/*
 * @description	: 驱动出口函数
 */
static void __exit bh1750_exit(void)
{
    i2c_del_driver(&gy_sensor_driver);
}

/* module_i2c_driver(bh1750_driver) */
module_init(bh1750_init);
module_exit(bh1750_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhu");

