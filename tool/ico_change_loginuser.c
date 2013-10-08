/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Login Tool to change login user
 *
 * @date    Sep-30-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ico_syc_common.h>
#include <ico_syc_error.h>
#include <ico_syc_userctl.h>
#include <ico_log.h>

/*============================================================================*/
/* Function prototype for static(internal) functions                          */
/*============================================================================*/
static void _sysctl_callback(const ico_syc_ev_e event, const void *detail,
                             void *user_data);

/*============================================================================*/
/* Tables and Variables                                                       */
/*============================================================================*/
/* original stdout file discriptor  */
static FILE *org_stdout;
/* new user name    */
static char newuser[128];
/* program name     */
static char progname[128];

/*============================================================================*/
/* Function                                                                   */
/*============================================================================*/
/**
 * @brief _sysctl_callback
 */
static void
_sysctl_callback(const ico_syc_ev_e event, const void *detail, void *user_data)
{
    ico_syc_userlist_t  *userlist;
    int     i;

    switch (event) {
    case ICO_SYC_EV_USERLIST:
        userlist = (ico_syc_userlist_t *)detail;

        fprintf(org_stdout, "%s: # of users = %d\n", progname, userlist->user_num);
        ICO_INF("%s: # of users = %d", progname, userlist->user_num);
        for (i = 0; i < userlist->user_num; i++) {
            if (strcmp(userlist->user_login, userlist->userlist[i]))    {
                fprintf(org_stdout, "%s:%2d. %s *\n", progname, i+1, userlist->userlist[i]);
                ICO_INF("%s:%2d. %s *", progname, i+1, userlist->userlist[i]);
            }
            else    {
                fprintf(org_stdout, "%s:%2d. %s\n", progname, i+1, userlist->userlist[i]);
                ICO_INF("%s:%2d. %s", progname, i+1, userlist->userlist[i]);
            }
        }
        (void)ico_syc_disconnect();
        ico_log_close();
        break;
    case ICO_SYC_EV_AUTH_FAIL:
        fprintf(org_stdout, "%s: fail to authenticate user<%s>\n", progname, newuser);
        ICO_ERR("%s: fail to authenticate user<%s>", progname, newuser);
        (void)ico_syc_disconnect();
        ico_log_close();
        break;
    default:
        ICO_WRN("other event (%d)", event);
        break;
    }
}

/**
 * @brief main
 */
int
main(int argc, char *argv[])
{
    int retry;
    int ret;

    strncpy(progname, argv[0], sizeof(progname));
    progname[sizeof(progname)-1] = 0;

    /* setting the log output       */
    org_stdout = stdout;
    ico_log_open("ico_change_loginuser");

    memset(newuser, 0, sizeof(newuser));
    if (argc > 1)   {
        strncpy(newuser, argv[1], sizeof(newuser)-1);
        ICO_INF("%s: change user to <%s>", progname, newuser);
    }
    else    {
        ICO_INF("%s: user list", progname);
    }

    /* connect to SystemController  */
    ret = ico_syc_connect(_sysctl_callback, NULL);
    if (ret != ICO_SYC_ERR_NONE) {
        ICO_WRN("%s: ico_syc_connect failed (ret: %d)", progname, ret);
        ico_log_close();
        fprintf(org_stdout, "%s: ico_syc_connect failed (ret: %d)\n", progname, ret);
        exit(1);
    }
    /* connect wait 200 ms          */
    for (retry = 0; retry < (200/20); retry++) {
        /* service for conection of SystemController    */
        ico_syc_service();
        usleep(20*1000);
    }
    if (argc <= 1)  {
        /* get userlist             */
        (void)ico_syc_get_userlist();
    }
    else    {
        /* change login user        */
        (void)ico_syc_change_user(newuser, "\0");
    }

    /* wait 300 ms                  */
    for (retry = 0; retry < (300/20); retry++)  {
        /* service for send/receive */
        ico_syc_service();
        usleep(20*1000);
    }

    (void)ico_syc_disconnect();
    ico_log_close();
    return 0;
}
