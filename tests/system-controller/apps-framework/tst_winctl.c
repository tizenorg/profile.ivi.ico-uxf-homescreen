/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for Window Control API
 *
 * @date    Aug-19-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Ecore.h>
#include <Eina.h>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Variable                                        */
/* ----------------------------------------------- */


/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static void _check_win_info(const char *ev_name, ico_syc_win_info_t *info);
static void _check_win_attr(const char *ev_name, ico_syc_win_attr_t *attr);
static void _check_thumb_info(const char *ev_name, ico_syc_thumb_info_t *info);
static void _check_layer_attr(const char *ev_name, ico_syc_layer_attr_t *attr);
/* callback */
static void _syc_callback(const ico_syc_ev_e event,
                          const void *detail, void *user_data);
/* window */
static void tst_show(const char *appid, int surface,
                     ico_syc_animation_t *animation);
static void tst_hide(const char *appid, int surface,
                     ico_syc_animation_t *animation);
static void tst_move(const char *appid, int surface,
                     ico_syc_win_move_t *move,
                     ico_syc_animation_t *animation, const char *type);
static void tst_change_active(const char *appid, int surface);
static void tst_change_layer(const char *appid, int surface, int layer);
/* thumbnail */
static void tst_prepare_thumb(int surface, int framerate);
static void tst_map_thumb(int surface);
static void tst_unmap_thumb(int surface);
/* layer */
static void tst_show_layer(int layer);
static void tst_hide_layer(int layer);
/* test main */
static Eina_Bool ico_syc_winctl_test(void *data);

/* ----------------------------------------------- */
/* Public API Test                                 */
/* ----------------------------------------------- */
static void 
_check_win_info(const char *ev_name, ico_syc_win_info_t *info)
{
    printf("--- %s ", ev_name);
    printf("(appid[%s], name[%s], surface[%d])\n",
           info->appid, info->name, info->surface);

    if (strcmp(info->appid, TST_APPID) == 0
        && strcmp(info->name, TST_WIN_NAME) == 0
        && info->surface == TST_SURFACE) {
        print_ok("callback (%s)", ev_name);
    }
    else {
        print_ng("callback (%s)", ev_name);
    }

    return;
}

static void
_check_win_attr(const char *ev_name, ico_syc_win_attr_t *attr)
{
    printf("--- %s\n", ev_name);
    printf("\t(appid[%s], name[%s], surface[%d], node[%d], layer[%d],\n",
           attr->appid, attr->name, attr->surface, attr->nodeid, attr->layer);
    printf("\t (x,y)[%d, %d], width[%d], height[%d], ",
           attr->pos_x, attr->pos_y, attr->width, attr->height);
    printf("raise[%d], visible[%d], active[%d])\n",
           attr->raise, attr->visible, attr->active);

    // move
    if (strcmp(attr->name, "move") == 0) {
        if ((attr->pos_x > 0) && (attr->pos_y > 0)) {
            print_ok("callback (%s move (pos))", ev_name);
        }
        else {
            print_ok("callback (%s move (zone))", ev_name);
        }
        return;
    }

    // not move
    if (strcmp(attr->appid, TST_APPID) == 0
        && attr->surface == TST_SURFACE && attr->nodeid == TST_NODE
        && attr->layer == TST_LAYER && attr->pos_x == TST_POS_X
        && attr->pos_y == TST_POS_Y && attr->width == TST_WIDTH
        && attr->height == TST_HEIGHT && attr->raise == TST_RAISE
        && attr->visible == TST_VISIBLE && attr->active == TST_ACTIVE) {
        print_ok("callback (%s %s)", ev_name, attr->name);
    }
    else {
        print_ng("callback (%s %s)", ev_name, attr->name);
    }

    return;
}

static void 
_check_thumb_info(const char *ev_name, ico_syc_thumb_info_t *info)
{
    printf("--- %s ", ev_name);
    printf("\t(appid[%s], surface[%d], width[%d], height[%d], ",
           info->appid, info->surface, info->width, info->height);
    printf("stride[%d], format[%d])\n",
           info->stride, info->format);

    if (strcmp(info->appid, TST_APPID) == 0
        && info->surface == TST_SURFACE
        && info->width == TST_WIDTH && info->height == TST_HEIGHT
        && info->stride == TST_STRIDE && info->format == TST_FORMAT) {
        print_ok("callback (%s)", ev_name);
    }
    else {
        print_ng("callback (%s)", ev_name);
    }

    return;
}

static void 
_check_layer_attr(const char *ev_name, ico_syc_layer_attr_t *attr)
{
    printf("--- %s ", ev_name);
    printf("(layer[%d], visible[%d])\n",
           attr->layer, attr->visible);

    if (attr->layer == TST_LAYER && attr->visible == TST_VISIBLE) {
        print_ok("callback (%s show)", ev_name);
    }
    else if (attr->layer == TST_LAYER && attr->visible == TST_INVISIBLE) {
        print_ok("callback (%s hide)", ev_name);
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
    case ICO_SYC_EV_WIN_CREATE:
        _check_win_info("ICO_SYC_EV_WIN_CREATE", (ico_syc_win_info_t *)detail);
        break;
    case ICO_SYC_EV_WIN_DESTROY:
        _check_win_info("ICO_SYC_EV_WIN_DESTROY", (ico_syc_win_info_t *)detail);
        break;
    case ICO_SYC_EV_WIN_ACTIVE:
        _check_win_info("ICO_SYC_EV_WIN_ACTIVE", (ico_syc_win_info_t *)detail);
        break;
    case ICO_SYC_EV_WIN_ATTR_CHANGE:
        _check_win_attr("ICO_SYC_EV_WIN_ATTR_CHANGE",
                        (ico_syc_win_attr_t *)detail);
        break;
    case ICO_SYC_EV_THUMB_PREPARE:
        _check_thumb_info("ICO_SYC_EV_THUMB_PREPARE",
                          (ico_syc_thumb_info_t *)detail);
        break;
    case ICO_SYC_EV_THUMB_CHANGE:
        _check_thumb_info("ICO_SYC_EV_THUMB_CHANGE",
                          (ico_syc_thumb_info_t *)detail);
        break;
    case ICO_SYC_EV_THUMB_UNMAP:
        _check_thumb_info("ICO_SYC_EV_THUMB_UNMAP",
                          (ico_syc_thumb_info_t *)detail);
        break;
    case ICO_SYC_EV_LAYER_ATTR_CHANGE:
        _check_layer_attr("ICO_SYC_EV_LAYER_ATTR_CHANGE",
                          (ico_syc_layer_attr_t *)detail);
        break;
    default:
        break;
    }

    return;
}

/* test show window */
static void
tst_show(const char *appid, int surface, ico_syc_animation_t *animation)
{
    int ret;
    char *func = "ico_syc_show";

    ret = ico_syc_show(appid, surface, animation);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test hide window */
static void
tst_hide(const char *appid, int surface, ico_syc_animation_t *animation)
{
    int ret;
    char *func = "ico_syc_hide";

    ret = ico_syc_hide(appid, surface, animation);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test move window */
static void
tst_move(const char *appid, int surface,
         ico_syc_win_move_t *move,
         ico_syc_animation_t *animation, const char *type)
{
    int ret;
    char *func = "ico_syc_move";

    ret = ico_syc_move(appid, surface, move, animation);
    if (ret != 0) {
        print_ng("%s (%s) (ret: %d)", func, type, ret);
        return;
    } 
    print_ok("%s (%s)", func, type);

    return;
}

/* test change active window */
static void
tst_change_active(const char *appid, int surface)
{
    int ret;
    char *func = "ico_syc_change_active";

    ret = ico_syc_change_active(appid, surface);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test change window layer */
static void
tst_change_layer(const char *appid, int surface, int layer)
{
    int ret;
    char *func = "ico_syc_change_layer";

    ret = ico_syc_change_layer(appid, surface, layer);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test prepare thumbnail */
static void
tst_prepare_thumb(int surface, int framerate)
{
    int ret;
    char *func = "ico_syc_prepare_thumb";

    ret = ico_syc_prepare_thumb(surface, framerate);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test map thumbnail */
static void
tst_map_thumb(int surface)
{
    ico_syc_thumb_data_t *ret = NULL;
    char *func = "ico_syc_map_thumb";

    ret = ico_syc_map_thumb(surface);
    if (ret == NULL) {
        print_ng("%s (return is NULL)", func);
        return;
    } 
    print_ok("%s", func);
    free(ret);

    return;
}

/* test unmap thumbnail */
static void
tst_unmap_thumb(int surface)
{
    int ret;
    char *func = "ico_syc_unmap_thumb";

    ret = ico_syc_unmap_thumb(surface);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test show layer */
static void
tst_show_layer(int layer)
{
    int ret;
    char *func = "ico_syc_show_layer";

    ret = ico_syc_show_layer(layer);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* test hide layer */
static void
tst_hide_layer(int layer)
{
    int ret;
    char *func = "ico_syc_hide_layer";

    ret = ico_syc_hide_layer(layer);
    if (ret != 0) {
        print_ng("%s (ret: %d)", func, ret);
        return;
    } 
    print_ok("%s", func);

    return;
}

/* ------------------------------- */
/* test main                       */
/* ------------------------------- */
static Eina_Bool
ico_syc_winctl_test(void *data)
{
    const char *appid   = "org.test.app.testapp";
    int surface         = 98765;
    int layer           = 5;
    ico_syc_win_move_t move, move_pos;
    ico_syc_animation_t animation;

    printf("\n");
    printf("##### ico_syc_winctl API Test Start #####\n");

    /* set move info (zone) */
    move.zone = "center:bottom";
    move.width = ICO_SYC_WIN_NOCHANGE;
    move.height = 600;
    /* set move info (position) */
    move_pos.zone = NULL;
    move_pos.pos_x = 10;
    move_pos.pos_y = 20;
    move_pos.width = 1024;
    move_pos.height = ICO_SYC_WIN_NOCHANGE;

    /* set animation data */
    animation.name = "fade";
    animation.time = 200;

    /* window */
    tst_show(appid, surface, &animation);
    usleep(5000);
    tst_hide(appid, surface, NULL);
    usleep(5000);
    tst_move(appid, surface, &move, &animation, "zone");
    usleep(5000);
    tst_move(appid, surface, &move_pos, NULL, "pos");
    usleep(5000);
    tst_change_active(appid, surface);
    usleep(5000);
    tst_change_layer(appid, surface, layer);

    sleep(1);

    /* thumbnail */
    tst_prepare_thumb(surface, 200);
    usleep(5000);
    tst_map_thumb(surface);
    usleep(5000);
    tst_unmap_thumb(surface);

    sleep(1);

    /* layer */
    tst_show_layer(layer);
    usleep(5000);
    tst_hide_layer(layer);

    printf("##### ico_syc_winctl API Test End #####\n");
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

    ecore_timer_add(1, ico_syc_winctl_test, NULL);
    ecore_timer_add(5, quit_test, NULL);

    ecore_main_loop_begin();
    ecore_shutdown();

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
