/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Pop Window
 *
 * @date    Feb-17-2014
 */
#ifndef __CICO_ONSCREEN_POP_WINDOW_H__
#define __CICO_ONSCREEN_POP_WINDOW_H__

#ifdef HAVE_CONFIG_H
    #include "config.h"
    #define __UNUSED__
#else
    #define __UNUSED__
#endif

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstring>

#include <app.h>
#include <aul.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>

#include <stdbool.h>
#include <CicoNotification.h>
#include <CicoNotificationService.h>

#include "ico_syc_common.h"
#include "ico_syc_privilege.h"

#include "ico_log.h"

/*============================================================================*/
/* Define fixed parameters                                                    */
/*============================================================================*/
/* Window Size */
#define STATUS_BAR_HEIGHT    64
#define CTRL_BAR_HEIGHT      128
#define WIDTH                1080
#if 1
#define HEIGHT               1920
#else
#define HEIGHT               (1920 - STATUS_BAR_HEIGHT - CTRL_BAR_HEIGHT)
#endif

/* Popup Size */
#define POPUP_WIDTH         640
#define POPUP_HEIGHT        300

/* Popup Frame Size */
#define POPUP_FRAME_THICK   10
#define POPUP_FRAME_WIDTH   (POPUP_WIDTH+POPUP_FRAME_THICK)
#define POPUP_FRAME_HEIGHT  (POPUP_HEIGHT+POPUP_FRAME_THICK)

#if 0
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
#endif

#define FALSE                0
#define TRUE                 1

#if 0
#define ICON_PATH      (char *)"/usr/share/icons/default/small/org.tizen.dialer.png"
#endif
#define ICO_OS_THEMES_EDJ_FILEPATH "/usr/apps/org.tizen.ico.onscreen/res/themes/onscreen.edj"
/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/

/*============================================================================*/
/* Define class                                                               */
/*============================================================================*/
class CicoOSPopWindow :public CicoNotification {
public:
    CicoOSPopWindow(notification_type_e type);
    CicoOSPopWindow(notification_h noti);
    virtual ~CicoOSPopWindow();
    bool    showPopup();
    void    hidePopup(bool buttonTouch = false);
    bool    acquireRes();
    bool    releaseRes();
    const CicoNotification& getNotif() const {
        return (const CicoNotification&)*this;
    }
    bool    createMainWindow();

private:
    bool    InitializeWindow(void);
    static void evasMouseUpCB(void *data, Evas *e, Evas_Object *obj,
                              void *event_info);
    void    makeResWindowT(ico_syc_res_window_t& w);
    static int   m_windowno;

public:
    Ecore_Evas*  m_window;
    Evas_Object* m_icon;
    Evas_Object* m_theme;
    uint32_t     m_resourceId;
    struct ico_syc_res_context* m_context;
    std::string  m_appsvc_pkgname;
    bool         m_buttonTouch;
};

#endif  // __CICO_ONSCREEN_POP_WINDOW_H__
// vim:set expandtab ts=4 sw=4:
