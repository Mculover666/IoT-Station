/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-09     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#include <drv_soft_i2c.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LED1_PIN    GET_PIN(B, 0)
#define LED2_PIN    GET_PIN(B, 7)
#define LED3_PIN    GET_PIN(B, 14)

extern int sht30_collect(void);

int main(void)
{
    int count = 1;
    /* set LED pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);

    sht30_collect();

    while (count++)
    {
        /* set LED pin level to high or low */
        rt_pin_write(LED1_PIN, count % 2);
        rt_pin_write(LED2_PIN, count % 2);
        rt_pin_write(LED3_PIN, count % 2);
        //LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

int register_i2c(void)
{
    rt_hw_i2c_init("i2c1", GET_PIN(B,8), GET_PIN(B,9));

    return RT_EOK;
}
//注册到系统中，自动初始化设备
INIT_BOARD_EXPORT(register_i2c);

