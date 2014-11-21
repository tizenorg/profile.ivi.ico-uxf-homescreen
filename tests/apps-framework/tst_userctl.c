/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for User Control API
 *
 * @date    Sep-5-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Ecore.h>
#include <Eina.h>

#include "ico_syc_common.h"
#include "ico_syc_userctl.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Variable                                        */
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static void _check_userlist(const char *ev_name, ico_syc_userlist_t *list);
static void _syc_callback(const ico_syc_ev_e event,
                          const void *detail, void *user_data);
static void tst_change_user(void);
static void tst_get_userlist(void);
static void tst_get_lastinfo(void);
static void tst_set_lastinfo(void);
static Eina_Bool  ico_syc_userctl_test(void *data);

/* ----------------------------------------------- */
/* Public API Test                                 */
/* ----------------------------------------------- */
static void
_check_userlist(const char *ev_name, ico_syc_userlist_t *list)
{
    int check_flag = 0;
    char *name;
    int id;

    printf("--- %s userlist[%d] (", ev_name, list->user_num);
    for (id = 0; id < list->user_num; id++) {
        if (id > 0) printf(", ");
        name = *(list->userlist);
        printf("%s", name);
        if (strcmp(name, TST_USER_A) != 0 && strcmp(name, TST_USER_B) != 0
            && strcmp(name, TST_USER_C) != 0) {
            check_flag++;
        }
        list->userlist++;
    }
    printf("), login[%s]\n", list->user_login);

    if (strcmp(list->user_login, TST_USER_A) != 0) {
        check_flag++;
    }

    if (check_flag == 0) {
        print_ok("callback (%s)", ev_name);
    }
    else {
        print_ng("callback (%s)", ev_name);
    }

    return;
}

static void
_syc_callback(const ico_syc_ev_e event,
              const void *detail, void *user_data)
{
    switch (event) {
    case ICO_SYC_EV_USERLIST:
        _check_userlist("ICO_SYC_EV_USERLIST", (ico_syc_userlist_t *)detail);
        break;
    case ICO_SYC_EV_AUTH_FAIL:
        if (detail == NULL) {
            printf("--- ICO_SYC_EV_AUTH_FAIL (detail is NULL)\n");
            print_ok("callback (ICO_SYC_EV_AUTH_FAIL)");
        }
        else {
            printf("--- ICO_SYC_EV_AUTH_FAIL (detail is not NULL)\n");
            print_ng("callback (ICO_SYC_EV_AUTH_FAIL)");
        }
        break;
    case ICO_SYC_EV_LASTINFO:
        printf("--- lastinfo: %s\n", (char *)detail);
        if (strcmp((char *)detail, TST_LASTINFO) == 0) {
            print_ok("callback (ICO_SYC_EV_LASTINFO)");
        }
        else {
            print_ng("callback (ICO_SYC_EV_LASTINFO)");
        }
        break;
    default:
        break;
    }

    return;
}

/* test change user */
static void
tst_change_user(void)
{
    (void)ico_syc_change_user("Alice", "");
    print_ok("(void)ico_syc_change_user");

    return;
}

/* test get userlist */
static void
tst_get_userlist(void)
{
    (void)ico_syc_get_userlist();
    print_ok("(void)ico_syc_get_userlist");

    return;
}

/* test get lastinfo */
static void
tst_get_lastinfo(void)
{
    (void)ico_syc_get_lastinfo();
    print_ok("(void)ico_syc_get_lastinfo");

    return;
}

/* test set lastinfo */
static void
tst_set_lastinfo(void)
{
    (void)ico_syc_set_lastinfo(TST_LASTINFO);
    print_ok("(void)ico_syc_set_lastinfo");

    return;
}

/* test main */
static Eina_Bool 
ico_syc_userctl_test(void *data)
{
    printf("\n");
    printf("##### ico_syc_userctl API Test Start #####\n");

    tst_change_user();
    usleep(5000);
    tst_get_userlist();
    usleep(5000);
    tst_set_lastinfo();
    usleep(5000);
    tst_get_lastinfo();

    printf("##### ico_syc_userctl API Test End #####\n");
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

    ico_syc_connect(_syc_callback, NULL);

    ecore_timer_add(1, ico_syc_userctl_test, NULL);
    ecore_timer_add(5, quit_test, NULL);

    ecore_main_loop_begin();
    ecore_shutdown();

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
