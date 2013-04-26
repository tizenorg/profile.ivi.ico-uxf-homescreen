/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   client program for application resource control test
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
#include "ico_uxfw_conf.h"

#define WIDTH  (520)
#define HEIGHT (380)

static int  loop = 0;

static Ecore_Evas *ee;
static const char *border_img_path = PACKAGE_EXAMPLES_DIR "/red.png";

static void
_on_destroy(Ecore_Evas *ee __UNUSED__)
{
    ecore_main_loop_quit();
}

static int  callno = 0;

static void
send_command(const int cno)
{
    int     ret = 0;
    char    device[32];

    sprintf(device, "Dev%03d", cno+1);

    switch (cno)    {
    case 0:
        ret = ico_apf_resource_get_screen_mode(device, cno+101);
        break;
    case 1:
        ret = ico_apf_resource_release_screen_mode(device, cno+101);
        break;
    case 2:
        ret = ico_apf_resource_get_int_screen_mode(device, cno+201, cno+101);
        break;
    case 3:
        ret = ico_apf_resource_get_int_screen_mode_disp(device, cno+101);
        break;
    case 4:
        ret = ico_apf_resource_release_int_screen_mode(device, cno+201, cno+101);
        break;
    case 5:
        ret = ico_apf_resource_release_int_screen_mode_disp(device, cno+101);
        break;
    case 6:
        ret = ico_apf_resource_get_sound_mode(device, cno+101, cno+301);
        break;
    case 7:
        ret = ico_apf_resource_release_sound_mode(device, cno+101);
        break;
    case 8:
        ret = ico_apf_resource_get_int_sound_mode(device, cno+101, cno+301);
        break;
    case 9:
        ret = ico_apf_resource_release_int_sound_mode(device, cno+101);
        break;
    case 10:
        ret = ico_apf_resource_add_input_event(device, cno+101);
        break;
    case 11:
        ret = ico_apf_resource_change_input_event(device, cno+101);
        break;
    case 12:
        ret = ico_apf_resource_delete_input_event(device, cno+101);
        break;
    default:
        break;
    }

    if (ret == 0)   {
        printf("## Client send command(%d) dev=%s\n", cno, device);
    }
    else    {
        printf("## Client send command(%d) Error[%d]\n", cno, ret);
    }
    fflush(stdout);
}

static void
res_callback(ico_apf_resource_notify_info_t* info, void *user_data)
{
    printf("##==> Client callbacked! evt=%d res=%d id=%d bid=%d appid=%s "
           "dev=%s user_data=%d\n",
           info->state, info->resid, info->id, info->bid, info->appid,
           info->device, (int)user_data);
    fflush(stdout);

    callno ++;
    if ((loop !=0) && (callno > 12))    {
        callno = 0;
    }
    if (callno <= 12)   {
        send_command(callno);
    }
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
    int     no_callback = 0;
    char    sTitle[64];

    width = WIDTH;
    height = HEIGHT;
    color = 0xc0c0c0c0;
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
        else if (strncasecmp(argv[i], "-nocb", 5) == 0)   {
            no_callback = 1;
        }
        else if (strncasecmp(argv[i], "-loop", 5) == 0)   {
            loop = 1;
        }
    }

    ecore_evas_init();

    /* configuration test   */
    (void)ico_uxf_getSysConfig();
    (void)ico_uxf_getAppConfig();

    if( ico_apf_ecore_init(NULL) != ICO_APF_E_NONE) {
        fprintf(stderr, "ico_apf_ecore_init() Error\n"); fflush(stderr);
        ecore_evas_shutdown();
        return -1;
    }
    if (no_callback == 0)   {
        ico_apf_resource_set_event_cb(res_callback, (void*)1);
    }

    /* this will give you a window with an Evas canvas under the first engine available */
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

    /* get screen           */
    send_command(0);
    if (no_callback)    {
        int     ii;

        for (ii = 1; ii <= 12; ii++)    {
            send_command(ii);
        }

        /* check receive buffer overflow    */
        for (ii = 0; ii <  64; ii++)    {
            send_command(ii);
        }
    }

    ecore_main_loop_begin();

    ico_apf_ecore_term();

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

