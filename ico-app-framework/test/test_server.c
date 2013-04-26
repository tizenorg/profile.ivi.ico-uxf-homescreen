/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   server(HomeScreen) program for application control
 *
 * @date    Feb-28-2013
 */
#ifdef HAVE_CONFIG_H
#undef HAVE_CONFIG_H
#endif

#ifdef HAVE_CONFIG_H

#include "config.h"
#else
#define __UNUSED__
#define PACKAGE_EXAMPLES_DIR "."
#endif

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include "ico_apf.h"
#include "ico_apf_ecore.h"

#define WIDTH  (520)
#define HEIGHT (380)

static Ecore_Evas *ee;
static const char *border_img_path = PACKAGE_EXAMPLES_DIR "/red.png";

static void
_on_destroy(Ecore_Evas *ee __UNUSED__)
{
    ecore_main_loop_quit();
}

static void
res_callback(ico_apf_resource_notify_info_t* info, void *user_data)
{
    printf("##==> Server callbacked! cmd=%d res=%d id=%d bid=%d appid=%s "
           "dev=%s user_data=%d\n",
           info->state, info->resid, info->id, info->bid, info->appid,
           info->device, (int)user_data);
    fflush(stdout);

    if ((info->state == ICO_APF_RESOURCE_COMMAND_GET) ||
        (info->state == ICO_APF_RESOURCE_COMMAND_RELEASE) ||
        (info->state == ICO_APF_RESOURCE_COMMAND_ADD) ||
        (info->state == ICO_APF_RESOURCE_COMMAND_CHANGE) ||
        (info->state == ICO_APF_RESOURCE_COMMAND_DELETE))   {
        switch (info->id % 4)   {
        case 0:
            ico_apf_resource_send_to_client(
                    info->appid, ICO_APF_RESOURCE_STATE_ACQUIRED, info->resid,
                    info->device[0] ? info->device : "UnKnown", info->id);
            printf("##==> Server send %d %d dev=%s id=%d\n",
                   ICO_APF_RESOURCE_STATE_ACQUIRED, info->resid, info->device, info->id);
            break;
        case 1:
            ico_apf_resource_send_to_client(
                    info->appid, ICO_APF_RESOURCE_STATE_DEPRIVED, info->resid,
                    info->device[0] ? info->device : "UnKnown", info->id);
            printf("##==> Server send %d %d dev=%s id=%d\n",
                   ICO_APF_RESOURCE_STATE_DEPRIVED, info->resid, info->device, info->id);
            break;
        case 2:
            ico_apf_resource_send_to_client(
                    info->appid, ICO_APF_RESOURCE_STATE_WAITTING, info->resid,
                    info->device[0] ? info->device : "UnKnown", info->id);
            printf("##==> Server send %d %d dev=%s id=%d\n",
                   ICO_APF_RESOURCE_STATE_WAITTING, info->resid, info->device, info->id);
            break;
        case 3:
            ico_apf_resource_send_to_client(
                    info->appid, ICO_APF_RESOURCE_STATE_RELEASED, info->resid,
                    info->device[0] ? info->device : "UnKnown", info->id);
            printf("##==> Server send %d %d dev=%s id=%d\n",
                   ICO_APF_RESOURCE_STATE_RELEASED, info->resid, info->device, info->id);
            break;
        default:
            break;
        }
    }
    else    {
        printf("##==> Server receive illegal command(%d)\n", info->state);
    }
    fflush(stdout);
}

int
main(int argc, char *argv[])
{
    Evas *canvas, *sub_canvas;
    Evas_Object *bg, *r1, *r2, *r3; /* "sub" canvas objects */
    Evas_Object *border, *img; /* canvas objects */
    Ecore_Evas *sub_ee;
    int     i;
    int     width;
    int     height;
    unsigned int    color;
    int     r, g, b, a;
    int     appno = 1;
    char    sTitle[64];

    width = WIDTH;
    height = HEIGHT;
    color = 0xc080b0e0;
    for (i = 1; i < argc; i++)  {
        if (argv[i][0] == '@')  {
            appno = strtol(&argv[i][1], (char **)0, 0);
        }
        if (argv[i][0] != '-')  continue;
        if (strncasecmp(argv[i], "-width=", 7) == 0)    {
            width = strtol(&argv[i][7], (char **)0, 0);
        }
        else if (strncasecmp(argv[i], "-height=", 8) == 0)  {
            height = strtol(&argv[i][8], (char **)0, 0);
        }
        else if (strncasecmp(argv[i], "-color=", 7) == 0)   {
            color = strtoul(&argv[i][7], (char **)0, 0);
        }
    }

    ecore_evas_init();

    if( ico_apf_ecore_init_server(NULL) != ICO_APF_E_NONE)  {
        fprintf(stderr, "ico_apf_ecore_init_server() Error\n"); fflush(stderr);
        ecore_evas_shutdown();
        return -1;
    }
    ico_apf_resource_set_event_cb(res_callback, (void*)2);

    /* this will give you a window with an Evas canvas under the first
     * engine available */
    ee = ecore_evas_new(NULL, 0, 0, width, height, "frame=0");
    if (!ee) goto error;

    ecore_evas_size_min_set(ee, width, height);
    ecore_evas_size_max_set(ee, width, height);

    ecore_evas_callback_delete_request_set(ee, _on_destroy);
    sprintf(sTitle, "EFL Native Application %d", appno);
    ecore_evas_title_set(ee, sTitle);
    ecore_evas_show(ee);

    canvas = ecore_evas_get(ee);

    bg = evas_object_rectangle_add(canvas);
    r = (color>>16)&0x0ff;
    g = (color>>8)&0x0ff;
    b = color&0x0ff;
    a = (color>>24)&0x0ff;
    if (a != 255)   {
        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;
    }
    evas_object_color_set(bg, r, g, b, a); /* bg color */
    evas_object_move(bg, 0, 0); /* at origin */
    evas_object_resize(bg, width, height); /* covers full canvas */
    evas_object_show(bg);

    /* this is a border around the image containing a scene of another * canvas */
    border = evas_object_image_filled_add(canvas);
    evas_object_image_file_set(border, border_img_path, NULL);
    evas_object_image_border_set(border, 3, 3, 3, 3);
    evas_object_image_border_center_fill_set(border, EVAS_BORDER_FILL_NONE);

    evas_object_move(border, width / 6, height / 6);
    evas_object_resize(border, (2 * width) / 3, (2 * height) / 3);
    evas_object_show(border);

    img = ecore_evas_object_image_new(ee);
    evas_object_image_filled_set(img, EINA_TRUE);
    evas_object_image_size_set(
        img, ((2 * width) / 3) - 6, ((2 * height) / 3) - 6);
    sub_ee = ecore_evas_object_ecore_evas_get(img);
    sub_canvas = ecore_evas_object_evas_get(img);

    evas_object_move(img, (width / 6) + 3, (height / 6) + 3);

    /* apply the same size on both! */
    evas_object_resize(img, ((2 * width) / 3) - 6, ((2 * height) / 3) - 6);
    ecore_evas_resize(sub_ee, ((2 * width) / 3) - 6, ((2 * height) / 3) - 6);

    r1 = evas_object_rectangle_add(sub_canvas);
    evas_object_color_set(r1, g, b, r, 255);
    evas_object_move(r1, 10, 10);
    evas_object_resize(r1, 100, 100);
    evas_object_show(r1);

    r2 = evas_object_rectangle_add(sub_canvas);
    evas_object_color_set(r2, b/2, g/2, r/2, 128);
    evas_object_move(r2, 10, 10);
    evas_object_resize(r2, 50, 50);
    evas_object_show(r2);

    r3 = evas_object_rectangle_add(sub_canvas);
    evas_object_color_set(r3, b, r, g, 255);
    evas_object_move(r3, 60, 60);
    evas_object_resize(r3, 50, 50);
    evas_object_show(r3);

    evas_object_show(img);

    ecore_main_loop_begin();

    ico_apf_ecore_term_server();

    ecore_evas_free(ee);
    ecore_evas_shutdown();

    return 0;

error:
    fprintf(stderr, "You got to have at least one Evas engine built"
                    " and linked up to ecore-evas for this example to run"
                    " properly.\n");
    ecore_evas_shutdown();
    return -1;
}

