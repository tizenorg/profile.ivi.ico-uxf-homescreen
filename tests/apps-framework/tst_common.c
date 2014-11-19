/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for Connect/Disconnect to System Controller API
 *
 * @date    Aug-19-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "ico_syc_common.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Variable                                        */
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static void tst_connect(void);
static void tst_disconnect(void);
static int ico_syc_common_test(void);

/* ----------------------------------------------- */
/* Public API Test                                 */
/* ----------------------------------------------- */
/* test connect */
static void
tst_connect(void)
{
    int ret;
    char *func = "ico_syc_connect";

    ret = ico_syc_connect(NULL, NULL);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
    }
    print_ok("%s", func);

    return;
}

/* test disconnect */
static void
tst_disconnect(void)
{
    (void)ico_syc_disconnect();
    print_ok("(void)ico_syc_disconnect");

    return;
}

/* test main */
static int
ico_syc_common_test()
{
    tst_connect();
    sleep(1);
    tst_disconnect();

    sleep(3);

    return 1;
}

/* ----------------------------------------------- */
/* Main                                            */
/* ----------------------------------------------- */
static GMainLoop *g_mainloop = NULL;

static gboolean
exit_program(gpointer data)
{
    g_main_loop_quit(g_mainloop);

    return FALSE;
}

/* main */
int
main(int argc, char **argv)
{
    g_setenv("PKG_NAME", "org.test.ico.syc_common", 1);
    g_mainloop = g_main_loop_new(NULL, 0);

    printf("\n");
    printf("##### ico_syc_common API Test Start #####\n");
    ico_syc_common_test();
    printf("##### ico_syc_common API Test End #####\n");
    printf("\n");

    g_timeout_add_seconds(1, exit_program, NULL);
    g_main_loop_run(g_mainloop);

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
