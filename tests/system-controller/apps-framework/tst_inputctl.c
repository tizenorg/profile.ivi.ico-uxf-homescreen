/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for Input Control API
 *
 * @date    July-31-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Ecore.h>
#include <Eina.h>

#include "ico_syc_common.h"
#include "ico_syc_inputctl.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Variable                                        */
/* ----------------------------------------------- */


/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static void tst_add_input(const char *appid, const char *device,
                          int input, int fix, int keycode);
static void tst_delete_input(const char *appid, const char *device,
                             int input);
static void tst_send_input(const char *appid, int surface, int ev_type,
                           int deviceno, int ev_code, int ev_value);
static Eina_Bool ico_syc_inputctl_test(void *data);

/* ----------------------------------------------- */
/* Public API Test                                 */
/* ----------------------------------------------- */
/* test add input */
static void
tst_add_input(const char *appid, const char *device,
              int input, int fix, int keycode)
{
    int ret;
    char *func = "ico_syc_add_input";

    ret = ico_syc_add_input(appid, device, input, fix, keycode);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test delete input */
static void
tst_delete_input(const char *appid, const char *device, int input)
{
    int ret;
    char *func = "ico_syc_delete_input";

    ret = ico_syc_delete_input(appid, device, input);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test send input */
static void
tst_send_input(const char *appid, int surface, int ev_type,
               int deviceno, int ev_code, int ev_value)
{
    int ret;
    char *func = "ico_syc_send_input";

    ret = ico_syc_send_input(appid, surface, ev_type, deviceno,
                             ev_code, ev_value);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test main */
static Eina_Bool
ico_syc_inputctl_test(void *data)
{
    const char *appid   = "org.test.app.testapp";
    const char *device  = "input.dev";
    int input           = 100;
    int fix             = ICO_SYC_INPUT_ALLOC_FIX;
    int keycode         = 10;
    int surface         = 12345;
    int ev_type         = ICO_SYC_INPUT_TYPE_POINTER;
    int deviceno        = 123;
    int ev_code         = 1;
    int ev_value        = 1;

    printf("\n");
    printf("##### ico_syc_inputctl API Test Start #####\n");

    tst_add_input(appid, device, input, fix, keycode);
    usleep(5000);
    tst_delete_input(appid, device, input);
    usleep(5000);
    tst_send_input(appid, surface, ev_type, deviceno, ev_code, ev_value);

    printf("##### ico_syc_inputctl API Test End #####\n");
    printf("\n");

    return ECORE_CALLBACK_CANCEL;
}
/* ------------------------ */
/* quit test callback       */
/* ------------------------ */
static Eina_Bool
quit_test(void *data)
{
    ico_syc_disconnect();
    ecore_main_loop_quit();

    return ECORE_CALLBACK_CANCEL;
}

/* ----------------------------------------------- */
/* Main                                            */
/* ----------------------------------------------- */
/* main */
int
main(int argc, char **argv)
{
    ecore_init();

    ico_syc_connect(NULL, NULL);

    ecore_timer_add(1, ico_syc_inputctl_test, NULL);
    ecore_timer_add(5, quit_test, NULL);

    ecore_main_loop_begin();
    ecore_shutdown();

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
