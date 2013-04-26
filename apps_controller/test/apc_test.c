/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Applocation Controller Policy library test
 *
 * @date    Feb-15-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <stdbool.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <sys/ioctl.h>
#include    <sys/time.h>
#include    <fcntl.h>
#include    <Ecore.h>

#include    "ico_syc_apc.h"
#include    "ico_syc_apc_private.h"

static Ecore_Task_Cb ecore_timer = NULL;
static void *ecore_user_data = NULL;
static int  time_ms = 100;

/* Ecore dummy routine  */
Ecore_Fd_Handler *
ecore_main_fd_handler_add(int fd, Ecore_Fd_Handler_Flags flags, Ecore_Fd_Cb func,
                          const void *data, Ecore_Fd_Cb buf_func, const void *buf_data)
{
    apfw_trace("TEST: dummyy ecore_main_fd_handler_add(%d,%d,%08x,%08x,%08x,%08x)",
               fd, (int)flags, (int)func, (int)data, (int)buf_func, (int)buf_data);
    return (Ecore_Fd_Handler *)0x12345678;
}

Eina_Bool
ecore_main_fd_handler_active_get(Ecore_Fd_Handler *fd_handler,
                                 Ecore_Fd_Handler_Flags flags)
{
    apfw_trace("TEST: dummyy ecore_main_fd_handler_active_get(%08x,%d)",
               (int)fd_handler, (int)flags);
    return EINA_FALSE;
}

void
ecore_main_fd_handler_active_set(Ecore_Fd_Handler *fd_handler,
                                 Ecore_Fd_Handler_Flags flags)
{
    apfw_trace("TEST: dummyy ecore_main_fd_handler_active_set(%08x,%d)",
               (int)fd_handler, (int)flags);
}

void *
ecore_main_fd_handler_del(Ecore_Fd_Handler *fd_handler)
{
    apfw_trace("TEST: dummyy ecore_main_fd_handler_del(%08x)", (int)fd_handler);
    return NULL;
}

Ecore_Timer *
ecore_timer_add(double dtime, Ecore_Task_Cb func, const void *user_data)
{
    ecore_timer = func;
    ecore_user_data = (void *)user_data;
    time_ms = (int)(dtime * 1000);
    if (time_ms < 10)       time_ms = 10;
    if (time_ms >= 1000)    time_ms = 990;

    return (Ecore_Timer *)ecore_timer;
}

static void displaycontrol(const Ico_Uxf_conf_application *conf, const int control)
{
    apfw_trace("TEST: displaycontrol(%s, %d)", conf->appid, control);
}

static void soundcontrol(const Ico_Uxf_conf_application *conf, const int control)
{
    apfw_trace("TEST: soundcontrol(%s, %d)", conf->appid, control);
}

static void inputcontrol(const Ico_Uxf_conf_application *conf, const int control)
{
    apfw_trace("TEST: inputcontrol(%s, %d)", conf->appid, control);
}

int main(int argc, char *argv[])
{
    int         ret;
    Eina_Bool   bret;

    ret = ico_syc_apc_init(displaycontrol, soundcontrol, inputcontrol);
    apfw_trace("TEST: ico_syc_apc_init() = %d", ret);

    while (1)   {
        usleep(time_ms * 1000);

        if (ecore_timer)    {
            apfw_trace("TEST: call ecore_timer");
            bret = (*ecore_timer)(ecore_user_data);
            apfw_trace("TEST: ret  ecore_timer(%d)", (int)bret);
        }
        else    {
            apfw_trace("TEST: NO  ecore_timer");
            break;
        }
    }

    exit(0);
}

