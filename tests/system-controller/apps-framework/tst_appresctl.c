/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for App Resource Control API
 *
 * @date    Aug-6-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Ecore.h>
#include <Eina.h>

#include "ico_syc_appresctl.h"
#include "ico_syc_common.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Variable                                        */
/* ----------------------------------------------- */
static struct ico_syc_res_context *res_context  = NULL;

/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static void _check_res_info(const char *ev_name, ico_syc_res_info_t *info);
static void _check_input_region(const char *ev_name,
                                ico_syc_input_region_t *region);
static void _syc_callback(const ico_syc_ev_e event,
                          const void *detail, void *user_data);
static void tst_acquire_res(const ico_syc_res_window_t *window,
                            const ico_syc_res_sound_t *sound,
                            const ico_syc_res_input_t *input,
                            int type);
static void tst_release_res(void);
static void tst_set_input_region(const ico_syc_input_region_t *region, int attr);
static void tst_unset_input_region(const ico_syc_input_region_t *region);
static Eina_Bool ico_syc_appresctl_test(void *data);

/* ----------------------------------------------- */
/* Public API Test                                 */
/* ----------------------------------------------- */
static void
_check_res_info(const char *ev_name, ico_syc_res_info_t *info)
{
    ico_syc_res_window_t *window = NULL;
    ico_syc_res_sound_t *sound   = NULL;
    ico_syc_res_input_t *input   = NULL;
    int check_flag = 0;

    printf("--- %s\n", ev_name);

    window = info->window;
    sound = info->sound;
    input = info->input;
    if (window != NULL) {
        printf("\t(window zone[%s], name[%s], id[%s]\n",
                window->zone, window->name, window->id);
        if (strcmp(window->zone, TST_ZONE_A) != 0
            || strcmp(window->name, TST_NAME_A) != 0
            || strcmp(window->id, TST_ID_A) != 0) {
            check_flag = 1;
        }
    }
    if (sound != NULL) {
        printf("\t sound zone[%s], name[%s], id[%s], adjust[%d]\n",
               sound->zone, sound->name, sound->id, sound->adjust);
        if (strcmp(sound->zone, TST_ZONE_B) != 0
            || strcmp(sound->name, TST_NAME_B) != 0
            || strcmp(sound->id, TST_ID_B) != 0
            || sound->adjust != TST_ADJUST) {
            check_flag = 1;
        }
    }
    if (input != NULL) {
        printf("\t input name[%s], event[%d])\n", input->name, input->event);
        if (strcmp(input->name, TST_NAME_C) != 0
            || input->event != TST_INPUT_EV) {
            check_flag = 1;
        }
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
_check_input_region(const char *ev_name, ico_syc_input_region_t *region)
{
    printf("--- %s\n", ev_name);
    printf("\t(winname[%s], (x,y)[%d, %d], width[%d], height[%d], hot[%d,%d], "
           "cursor[%d.%d-%d,%d], attr[%d])\n",
           region->winname, region->pos_x, region->pos_y,
           region->width, region->height, region->hotspot_x, region->hotspot_y,
           region->cursor_x, region->cursor_y, region->cursor_width,
           region->cursor_height, region->attr);

    if ((strcmp(region->winname, TST_WIN_NAME) == 0)
        && region->pos_x == TST_POS_X && region->pos_y == TST_POS_Y
        && region->width == TST_REG_WIDTH && region->height == TST_REG_HEIGHT
        && region->hotspot_x == TST_REG_HOT_X && region->hotspot_y == TST_REG_HOT_Y
        && region->cursor_x == TST_REG_CUR_X && region->cursor_y == TST_REG_CUR_Y
        && region->cursor_width == TST_REG_CUR_WIDTH
        && region->cursor_height == TST_REG_CUR_HEIGHT
        && region->attr == TST_REG_ATTR)    {
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
    case ICO_SYC_EV_RES_ACQUIRE:
        _check_res_info("ICO_SYC_EV_RES_ACQUIRE", (ico_syc_res_info_t *)detail);
        break;
    case ICO_SYC_EV_RES_DEPRIVE:
        _check_res_info("ICO_SYC_EV_RES_DEPRIVE", (ico_syc_res_info_t *)detail);
        break;
    case ICO_SYC_EV_RES_WAITING:
        _check_res_info("ICO_SYC_EV_RES_WAITING", (ico_syc_res_info_t *)detail);
        break;
    case ICO_SYC_EV_RES_REVERT:
        _check_res_info("ICO_SYC_EV_RES_REVERT", (ico_syc_res_info_t *)detail);
        break;
    case ICO_SYC_EV_RES_RELEASE:
        _check_res_info("ICO_SYC_EV_RES_RELEASE", (ico_syc_res_info_t *)detail);
        break;
    case ICO_SYC_EV_INPUT_SET:
        _check_input_region("ICO_SYC_EV_INPUT_SET",
                            (ico_syc_input_region_t *)detail);
        break;
    case ICO_SYC_EV_INPUT_UNSET:
        _check_input_region("ICO_SYC_EV_INPUT_UNSET",
                            (ico_syc_input_region_t *)detail);
        break;
    default:
        break;
    }

    return;
}

/* test acquire resource */
static void
tst_acquire_res(const ico_syc_res_window_t *window,
                const ico_syc_res_sound_t *sound,
                const ico_syc_res_input_t *input,
                int type)
{
    char *func = "ico_syc_acquire_res";

    res_context = ico_syc_acquire_res(window, sound, input, type);
    if (res_context == NULL) {
        print_ng("%s (context is NULL)", func);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test release resource */
static void
tst_release_res(void)
{
    int ret;
    char *func = "ico_syc_release_res";

    ret = ico_syc_release_res(res_context);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test set input region */
static void
tst_set_input_region(const ico_syc_input_region_t *region, int attr)
{
    int ret;
    char *func = "ico_syc_set_input_region";

    ret = ico_syc_set_input_region(region, attr);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* test unset input region */
static void
tst_unset_input_region(const ico_syc_input_region_t *region)
{
    int ret;
    char *func = "ico_syc_unset_input_region";

    ret = ico_syc_unset_input_region(region);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    }
    print_ok("%s", func);

    return;
}

/* ------------------------ */
/* test main                */
/* ------------------------ */
static 
Eina_Bool ico_syc_appresctl_test(void *data)
{
    printf("\n");
    printf("##### ico_syc_appresctl API Test Start #####\n");

    /* window/sound resource */
    ico_syc_res_window_t window;
    ico_syc_res_sound_t sound;
    int type            = ICO_SYC_RES_BASIC;
    /* input resource */
    ico_syc_res_input_t input;
    /* input region */
    ico_syc_input_region_t region;
    int attr            = 1;

    /* set window resource info */
    window.zone         = "center";
    window.name         = "window A";
    window.id           = "id A";
    /* set sound resource info */
    sound.zone          = "all";
    sound.name          = "sound B";
    sound.id            = "id B";
    sound.adjust        = ICO_SYC_SOUND_MUTE;
    /* set sound resource info */
    input.name          = "input C";
    input.event         = 100;
    /* set input region info */
    strcpy(region.winname, TST_WIN_NAME);
    region.pos_x        = 10;
    region.pos_y        = 10;
    region.width        = 200;
    region.height       = 100;
    region.hotspot_x    = 28;
    region.hotspot_y    = 16;
    region.cursor_x     = 35;
    region.cursor_y     = 25;
    region.cursor_width = 150;
    region.cursor_height= 80;
    region.attr         = 98765432;

    /* acquire window/sound/input */
    tst_acquire_res((const ico_syc_res_window_t *)&window,
                    (const ico_syc_res_sound_t *)&sound,
                    (const ico_syc_res_input_t *)&input,
                    type);
    usleep(5000);
    /* set input region */
    tst_set_input_region(&region, attr);

    sleep(2);

    /* release window/sound/input */
    tst_release_res();
    usleep(5000);
    /* unset input region */
    tst_unset_input_region(&region);

    sleep(3);

    printf("##### ico_syc_appresctl API Test End #####\n");
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

    ecore_timer_add(1, ico_syc_appresctl_test, NULL);
    ecore_timer_add(5, quit_test, NULL);

    ecore_main_loop_begin();
    ecore_shutdown();

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
