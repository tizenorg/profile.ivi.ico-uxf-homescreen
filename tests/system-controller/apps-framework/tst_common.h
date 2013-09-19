/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef __TST_COMMON_H__
#define __TST_COMMON_H__

#include "ico_syc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* log macro */
#define print_ok(fmt, arg...)                   \
    do {                                        \
        fprintf(stdout,                         \
                "[TestCase] " fmt " : OK\n",    \
                ##arg);                         \
    } while (0)

#define print_ng(fmt, arg...)                   \
    do {                                        \
        fprintf(stdout,                         \
                "[TestCase] " fmt " : NG\n",    \
                ##arg);                         \
    } while (0)


/* test server uri */
#define SRV_URI ":18081"

/* return value */
#define TST_APPID       (char *)"org.test.syc.app"
#define TST_WIN_NAME    (char *)"surface A"

#define TST_SURFACE     100
#define TST_NODE        1
#define TST_LAYER       2
#define TST_POS_X       10
#define TST_POS_Y       20
#define TST_WIDTH       1024
#define TST_HEIGHT      768
#define TST_RAISE       ICO_SYC_WIN_RAISE_RAISE
#define TST_VISIBLE     ICO_SYC_WIN_VISIBLE_SHOW
#define TST_INVISIBLE   ICO_SYC_WIN_VISIBLE_HIDE
#define TST_ACTIVE      ICO_SYC_WIN_ACTIVE_NONE
#define TST_STRIDE      1
#define TST_FORMAT      ICO_SYC_THUMB_FORMAT_ARGB

#define TST_ZONE_A      (char *)"zone A"
#define TST_ZONE_B      (char *)"zone B"
#define TST_NAME_A      (char *)"name A"
#define TST_NAME_B      (char *)"name B"
#define TST_NAME_C      (char *)"name C"
#define TST_ID_A        (char *)"id A"
#define TST_ID_B        (char *)"id B"

#define TST_ADJUST      1
#define TST_INPUT_EV    2
#define TST_RES_TYPE    0
#define TST_REG_WIDTH   200
#define TST_REG_HEIGHT  100
#define TST_REG_HOT_X   120
#define TST_REG_HOT_Y   80
#define TST_REG_CUR_X   40
#define TST_REG_CUR_Y   30
#define TST_REG_CUR_WIDTH  160
#define TST_REG_CUR_HEIGHT 100
#define TST_REG_ATTR    12345678

#define TST_USER_A      (char *)"user A"
#define TST_USER_B      (char *)"User B"
#define TST_USER_C      (char *)"user c"

#define TST_LASTINFO    (char *)"{\"page\": \"last page\", \"time\": 5}"

#ifdef __cplusplus
}
#endif

#endif /* __TST_COMMON_H__ */
/* vim: set expandtab ts=4 sw=4: */
