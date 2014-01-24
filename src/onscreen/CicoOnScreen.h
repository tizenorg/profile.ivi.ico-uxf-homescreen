/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   On Screen
 *
 * @date    Jan-07-2014
 */
#ifndef __CICO_ON_SCREEN_H__
#define __CICO_ON_SCREEN_H__

#ifdef HAVE_CONFIG_H
    #include "config.h"
    #define __UNUSED__
#else
    #define __UNUSED__
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <app.h>
#include <aul.h>
#include <Ecore_Evas.h>
#include <Evas.h>

#include <stdbool.h>
#include <CicoNotification.h>
#include <CicoNotificationService.h>

#include "ico_log.h"

/*============================================================================*/
/* Define fixed parameters                                                    */
/*============================================================================*/
/* Window Size */
#define STATUS_BAR_HEIGHT    64
#define CTRL_BAR_HEIGHT      128
#define WIDTH                1080
#define HEIGHT               (1920 - STATUS_BAR_HEIGHT - CTRL_BAR_HEIGHT)

/* Popup Size */
#define POPUP_WIDTH         640
#define POPUP_HEIGHT        300
#define POPUP_ST_X          ((WIDTH-POPUP_WIDTH)/2)
#define POPUP_ST_Y          ((HEIGHT-POPUP_HEIGHT)/2)

/* Popup Frame Size */
#define POPUP_FRAME_THICK   10
#define POPUP_FRAME_WIDTH   (POPUP_WIDTH+POPUP_FRAME_THICK)
#define POPUP_FRAME_HEIGHT  (POPUP_HEIGHT+POPUP_FRAME_THICK)
#define POPUP_FRAME_ST_X    (POPUP_ST_X-POPUP_FRAME_THICK/2)
#define POPUP_FRAME_ST_Y    (POPUP_ST_Y-POPUP_FRAME_THICK/2)

/* Popup Icon Size */
#define POPUP_ICON_WIDTH    50
#define POPUP_ICON_HEIGHT   50
#define POPUP_ICON_ST_X     POPUP_ST_X
#define POPUP_ICON_ST_Y     POPUP_ST_Y

/* Popup Title Size */
#define POPUP_TITLE_WIDTH   (POPUP_WIDTH-POPUP_ICON_WIDTH)
#define POPUP_TITLE_HEIGHT  POPUP_ICON_HEIGHT
#define POPUP_TITLE_ST_X    (POPUP_ST_X+POPUP_ICON_WIDTH)
#define POPUP_TITLE_ST_Y    POPUP_ST_Y

/* Popup Content BG Size */
#define POPUP_CONTENT_BG_WIDTH  POPUP_WIDTH
#define POPUP_CONTENT_BG_HEIGHT (POPUP_HEIGHT-POPUP_TITLE_HEIGHT)
#define POPUP_CONTENT_BG_ST_X   POPUP_ST_X
#define POPUP_CONTENT_BG_ST_Y   (POPUP_ST_Y+POPUP_TITLE_HEIGHT)

/* Popup Content Size */
#define POPUP_CONTENT_WIDTH  POPUP_WIDTH
#define POPUP_CONTENT_HEIGHT POPUP_TITLE_HEIGHT
#define POPUP_CONTENT_ST_X   POPUP_ST_X
#define POPUP_CONTENT_ST_Y   (POPUP_ST_Y+POPUP_TITLE_HEIGHT+(POPUP_HEIGHT-POPUP_CONTENT_HEIGHT)/2)

#define ICO_ORIENTATION_VERTICAL (1) 
#define ICO_ORIENTATION_HORIZONTAL (2)

#define LEMOLO_PKGNAME       "org.tizen.dialer"

#define FALSE                0
#define TRUE                 1

#define ICON_PATH      (char *)"/usr/share/icons/default/small/org.tizen.dialer.png"

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
struct popup_data
{
    Ecore_Evas  *window;
    Evas        *evas;
    Evas_Object *background;
    Evas_Object *icon;
    Evas_Object *title;
    Evas_Object *content;
    Evas_Object *icon_bg;
    Evas_Object *title_bg;
    Evas_Object *content_bg;
    int         show_flag;
};

struct lemolo_noti_data
{
    char *title;             /* if exist then box-type else band-type */
    char *content;           /* Mandatory */
    char *icon;              /* Mandatory(box-type), Optional(band-type) */
    char *text;
    bundle *service_handle;
};

/*============================================================================*/
/* Define class                                                               */
/*============================================================================*/
class CicoOnScreen
{
  public:
    CicoOnScreen(void);
    ~CicoOnScreen(void);
    bool StartOnScreen(int orientation);
    void Finalize(void);
    static void NotificationCallback(void *data, notification_type_e type,
        notification_op *op_list, int num_op);

  private:
    void ShowPopup(void);
    static void HidePopup(void *data, Evas *e, Evas_Object *obj, void *event_info);
    void InitializePopup(void);
    void InitializeNotificationData(void);
    void InitializePopupData(void);

    static CicoOnScreen *os_instance;
    static struct popup_data ico_appdata;
    static struct lemolo_noti_data ico_notidata;

  protected:
    CicoNotificationService notiservice_;
};
#endif  // __CICO_ON_SCREEN_H__
// vim:set expandtab ts=4 sw=4:
