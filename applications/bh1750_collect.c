#include <rtthread.h>
#include <rtdevice.h>

#define BH1750_I2C_BUS_NAME         "i2c2"   /* ���������ӵ�I2C�����豸���� */
#define BH1750_ADDR                  0x23    /* �ӻ���ַ */

typedef enum
{
        POWER_OFF_CMD      =        0x00,        //�ϵ磺�޼���״̬
        POWER_ON_CMD       =        0x01,        //ͨ�磺�ȴ�����ָ��
        RESET_REGISTER      =        0x07,        //�������ּĴ������ڶϵ�״̬�²������ã�
        CONT_H_MODE           =        0x10,        //����H�ֱ���ģʽ����11x�ֱ����¿�ʼ����������ʱ��120ms
        CONT_H_MODE2         =        0x11,        //����H�ֱ���ģʽ2����0.51x�ֱ����¿�ʼ����������ʱ��120ms
        CONT_L_MODE            =        0x13,        //����L�ֱ���ģʽ����411�ֱ����¿�ʼ����������ʱ��16ms
        ONCE_H_MODE           =        0x20,        //һ�θ߷ֱ���ģʽ����11x�ֱ����¿�ʼ����������ʱ��120ms���������Զ�����Ϊ�ϵ�ģʽ
        ONCE_H_MODE2         =        0x21,        //һ�θ߷ֱ���ģʽ2����0.51x�ֱ����¿�ʼ����������ʱ��120ms���������Զ�����Ϊ�ϵ�ģʽ
        ONCE_L_MODE            =        0x23        //һ�εͷֱ���ģʽ����411x�ֱ����¿�ʼ����������ʱ��16ms���������Զ�����Ϊ�ϵ�ģʽ
} BH1750_MODE;

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C�����豸��� */

/**
 * @brief        ��BH1750����һ��ָ��
 * @param        cmd ���� BH1750����ģʽָ���BH1750_MODE��ö�ٶ��壩
 * @retval        �ɹ�����RT_EOK
*/
rt_err_t BH1750_Send_Cmd(BH1750_MODE cmd)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.len = 1;
    msgs.buf = (rt_uint8_t*)&cmd;

    /* ����I2C�豸�ӿڴ������� */
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
 * @brief        ��BH1750����һ�ι�ǿ����
 * @param        dat ���� �洢����ǿ�ȵĵ�ַ�������ֽ����飩
 * @retval        �ɹ� ���� ����RT_EOK
*/
rt_err_t BH1750_Read_Dat(rt_uint8_t* dat)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.len = 2;
    msgs.buf = dat;

    /* ����I2C�豸�ӿڴ������� */
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
 * @brief        ��BH1750�������ֽ�����ת��Ϊ����ǿ��ֵ��0-65535��
 * @param        dat  ���� �洢����ǿ�ȵĵ�ַ�������ֽ����飩
 * @retval        �ɹ� ���� ���ع���ǿ��ֵ
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
    rt_uint8_t dat[2] = {0};                //dat[0]�Ǹ��ֽڣ�dat[1]�ǵ��ֽ�

    /* ����I2C�����豸����ȡI2C�����豸��� */
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find(BH1750_I2C_BUS_NAME);

    if(i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", BH1750_I2C_BUS_NAME);
    }
    while(1)
    {
        /* ������������ģʽ */
        BH1750_Send_Cmd(ONCE_H_MODE);

        /* �ȴ�����ת����� */
        rt_thread_mdelay(150);

        /* ��ȡ���� */
        BH1750_Read_Dat(dat);

        /* ת�����ݲ���ӡ */
        rt_kprintf("current: %5d lux\n", BH1750_Dat_To_Lux(dat));

        rt_thread_mdelay(1000);
    }
}
int i2c_bh1750_example(void)
{
    rt_thread_t tid;    //�߳̾��

    tid = rt_thread_create("bh1750_thread",
                            i2c_bh1750_example_entry,
                            RT_NULL,
                            512,
                            9,
                            10);
   if(tid != RT_NULL)
   {
        //�̴߳����ɹ��������߳�
        rt_thread_startup(tid);
   }

   return 0;

}

/* ������ msh �����б��� */
MSH_CMD_EXPORT(i2c_bh1750_example, i2c bh1750 example);
