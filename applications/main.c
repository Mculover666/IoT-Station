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

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LED1_PIN    GET_PIN(B, 0)
#define LED2_PIN    GET_PIN(B, 7)
#define LED3_PIN    GET_PIN(B, 14)


int main(void)
{
    int count = 1;
    /* set LED pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);

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
