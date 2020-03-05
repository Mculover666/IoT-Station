#include <rtthread.h>
#include <rtdevice.h>

#define BH1750_I2C_BUS_NAME         "i2c2"   /* 传感器连接的I2C总线设备名称 */
#define BH1750_ADDR                  0x23    /* 从机地址 */

typedef enum
{
        POWER_OFF_CMD      =        0x00,        //断电：无激活状态
        POWER_ON_CMD       =        0x01,        //通电：等待测量指令
        RESET_REGISTER      =        0x07,        //重置数字寄存器（在断电状态下不起作用）
        CONT_H_MODE           =        0x10,        //连续H分辨率模式：在11x分辨率下开始测量，测量时间120ms
        CONT_H_MODE2         =        0x11,        //连续H分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms
        CONT_L_MODE            =        0x13,        //连续L分辨率模式：在411分辨率下开始测量，测量时间16ms
        ONCE_H_MODE           =        0x20,        //一次高分辨率模式：在11x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
        ONCE_H_MODE2         =        0x21,        //一次高分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
        ONCE_L_MODE            =        0x23        //一次低分辨率模式：在411x分辨率下开始测量，测量时间16ms，测量后自动设置为断电模式
} BH1750_MODE;

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C总线设备句柄 */

/**
 * @brief        向BH1750发送一条指令
 * @param        cmd —— BH1750工作模式指令（在BH1750_MODE中枚举定义）
 * @retval        成功返回RT_EOK
*/
rt_err_t BH1750_Send_Cmd(BH1750_MODE cmd)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.len = 1;
    msgs.buf = (rt_uint8_t*)&cmd;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(i2c_bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
/**
 * @brief        从BH1750接收一次光强数据
 * @param        dat —— 存储光照强度的地址（两个字节数组）
 * @retval        成功 —— 返回RT_EOK
*/
rt_err_t BH1750_Read_Dat(rt_uint8_t* dat)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.len = 2;
    msgs.buf = dat;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(i2c_bus, &msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
/**
 * @brief        将BH1750的两个字节数据转换为光照强度值（0-65535）
 * @param        dat  —— 存储光照强度的地址（两个字节数组）
 * @retval        成功 —— 返回光照强度值
*/
rt_uint16_t BH1750_Dat_To_Lux(rt_uint8_t* dat)
{
        rt_uint16_t lux = 0;
        lux = dat[0];
        lux <<= 8;
        lux += dat[1];
        lux = (int)(lux / 1.2);

        return lux;
}

void i2c_bh1750_example_entry(void *parameter)
{
    rt_uint8_t dat[2] = {0};                //dat[0]是高字节，dat[1]是低字节

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find(BH1750_I2C_BUS_NAME);

    if(i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", BH1750_I2C_BUS_NAME);
    }
    while(1)
    {
        /* 发送命令设置模式 */
        BH1750_Send_Cmd(ONCE_H_MODE);

        /* 等待数据转换完成 */
        rt_thread_mdelay(150);

        /* 读取数据 */
        BH1750_Read_Dat(dat);

        /* 转换数据并打印 */
        rt_kprintf("current: %5d lux\n", BH1750_Dat_To_Lux(dat));

        rt_thread_mdelay(1000);
    }
}
int i2c_bh1750_example(void)
{
    rt_thread_t tid;    //线程句柄

    tid = rt_thread_create("bh1750_thread",
                            i2c_bh1750_example_entry,
                            RT_NULL,
                            512,
                            9,
                            10);
   if(tid != RT_NULL)
   {
        //线程创建成功，启动线程
        rt_thread_startup(tid);
   }

   return 0;

}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(i2c_bh1750_example, i2c bh1750 example);
