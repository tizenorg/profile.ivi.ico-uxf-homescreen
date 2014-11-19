/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoComponentimplementation.cpp
 *
 *  @brief  This file is implimentation of CicoStatusBarClockComponent class
 *                                  and CicoNotificationPanelComponent class
 */
//==========================================================================

#include <Edje.h>
#include <ctime>
#include <cassert>
#include <memory>
#include <tzplatform_config.h>

#include <ico_log.h>
#include "ico_syc_private.h"
#include "CicoComponentImplementation.h"

/*  define of path   */
#define IMG_DIR     ICO_SYC_PACKAGE_STATUSBAR "/res/images"
#define THEME_PATH  ICO_SYC_PACKAGE_STATUSBAR "/res/themes/statusbar.edj"

/*  Theme group name  */
#define THEME_CLOCK_NAME        "ClockImage"
#define THEME_NOTIFICATION_NAME "Notification"

// set image file path of clock
static const char *clock_image_path[] = {
    "time_0.png",
    "time_1.png",
    "time_2.png",
    "time_3.png",
    "time_4.png",
    "time_5.png",
    "time_6.png",
    "time_7.png",
    "time_8.png",
    "time_9.png",
    "time_am.png",
    "time_pm.png",
    "time_ten.png",
    NULL
};

/*  define of special image index  */
#define TIME_IMAGE_AM (10)
#define TIME_IMAGE_PM (11)
#define TIME_IMAGE_SP (12)

/*  array of clock object name  */
static const char *clock_image_object[] = {
    "AM_PM_img",
    "HOUR10_img",
    "HOUR1_img",
    "TIME_SP_img",
    "MIN10_img",
    "MIN1_img",
    NULL
};

/*--------------------------------------------------------------------------*/
/**
 *  @brief   default constructor
 *
 *  @param[in]   none
 *  @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarClockComponent::CicoStatusBarClockComponent()
{
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   destructor
 *
 *  @param[in]   none
 *  @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarClockComponent::~CicoStatusBarClockComponent()
{
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   initialize of clock component
 *
 *  @param[in]  windowobj
 *  @param[in]  posx
 *  @param[in]  posy
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::Initialize(Evas_Object *windowobj, Evas *evas)
{
    ICO_TRA("CicoStatusBarClockComponent::Initialize Enter");

    /*  initialize display clock image */
    int i;
    for(i=0; i<CLOCK_OBJECT_COUNT; i++ ){
        last_clock_image[i] = -1;
    }

    /*  make TimeImage object  */
    evasobj_=edje_object_add(evas);
    char    wrk_path[256];
    snprintf(wrk_path, sizeof(wrk_path), "%s/" THEME_PATH,
             tzplatform_getenv(TZ_SYS_RO_APP));
    if (!edje_object_file_set(evasobj_, wrk_path, THEME_CLOCK_NAME )) {
        Edje_Load_Error err = edje_object_load_error_get(evasobj_);
        const char *errmsg = edje_load_error_str(err);
        ICO_ERR("could not load 'main' from statusbar.edj: %s",
                errmsg);
        return false;
    }

    /*  move TimeImage to UpRight of window  */
    /*  Do not need this process , if positiong of UpRight enabled with .EDC file */
    Evas_Coord x,y,w,h;
    Evas_Coord w_width=0;
    Evas_Object* obj = NULL;
    evas_object_geometry_get( windowobj, &x, &y, &w_width, &h );
    evas_object_resize( evasobj_, w_width, h );
    obj = (Evas_Object*)edje_object_part_object_get(evasobj_,"MIN1_img" );
    evas_object_geometry_get( obj, &x, &y, &w, &h );
    Evas_Coord offset = w_width - x - w -18;

    for ( i=0; i<CLOCK_OBJECT_COUNT; i++ ) {
        obj = (Evas_Object*)edje_object_part_object_get(
                        evasobj_, clock_image_object[i] );
        if ( obj != NULL ) {
            evas_object_geometry_get( obj, &x, &y, &w, &h );
            evas_object_move( obj, x+offset , y);
            evas_object_show(obj);
        }
    }

    /*  initial display  */
    bool ret = true;
    ret = Update();
    Show();

    ICO_TRA("CicoStatusBarClockComponent::Initialize Leave");
    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   update of clock component
 *
 *  @param[in]  none
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::Update()
{
    //ICO_TRA("CicoStatusBarClockComponent::Update Enter");
    if (evasobj_ == NULL) {
        return false;
    }

    time_t nowtime;
    nowtime = std::time(NULL);
    std::tm *tm = std::localtime(&nowtime);
    int min = tm->tm_min;
    int hour = tm->tm_hour;

    /*  Make now clock image tabel  */
    int now_clock_image[CLOCK_OBJECT_COUNT];
    if (hour > 11) {
        now_clock_image[0]=TIME_IMAGE_PM;
    }
    else {
        now_clock_image[0]=TIME_IMAGE_AM;
    }
    time_t hour_m = hour % 12;
    now_clock_image[1]=hour_m / 10;
    now_clock_image[2]=hour_m % 10;
    now_clock_image[3]=TIME_IMAGE_SP;
    now_clock_image[4]=min / 10;
    now_clock_image[5]=min % 10;

    /*  Set now clock image  */
    int i;
    for ( i=0; i<CLOCK_OBJECT_COUNT; i++ ) {

        /*  if now clock image different last clock  */
        if ( now_clock_image[i] != last_clock_image[i] ) {

            Evas_Object* obj = NULL;
            obj = (Evas_Object*)edje_object_part_object_get(
                         evasobj_,clock_image_object[i] );
            if ( obj != NULL ) {
                ICO_DBG("CicoStatusBarClockComponent image set[%s][%s]",
                        clock_image_object[i], clock_image_path[now_clock_image[i]]);
                char    wrk_path[256];
                snprintf(wrk_path, sizeof(wrk_path), "%s/" IMG_DIR "/%s",
                         tzplatform_getenv(TZ_SYS_RO_APP),
                         clock_image_path[now_clock_image[i]]);
                evas_object_image_file_set(obj, wrk_path, NULL);
                // add update view area
                //    (Omitted update area is set so that evas_object_image_file_set())
                //Evas_Coord x,y,w,h;
                //evas_object_geometry_get( obj, &x, &y, &w, &h );
                //evas_object_image_data_update_add( obj, x, y, w, h );

            }
            else {
                ICO_DBG("CicoStatusBarClockComponent image set error[object not found]" );
            }
            last_clock_image[i] = now_clock_image[i];
        }
    }

    //ICO_TRA("CicoStatusBarClockComponent::Update Leave(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   get clock start position
 *
 *  @param[out]  x  X axis
 *  @param[out]  y  Y axis
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::GetClockStart( Evas_Coord *x_ret, Evas_Coord *y_ret)
{
    ICO_TRA("CicoStatusBarClockComponent::GetClockStart Enter");
    if (evasobj_ == NULL) {
        ICO_TRA("CicoStatusBarClockComponent::GetClockStart Leave(false)");
        return false;
    }

    Evas_Object* obj = NULL;
    obj = (Evas_Object*)edje_object_part_object_get(
                 evasobj_, clock_image_object[0] );
    if ( obj == NULL ) {
        ICO_TRA("CicoStatusBarClockComponent::GetClockStart Leave(false)");
        return false;
    }

    Evas_Coord x,y,w,h;
    evas_object_geometry_get( obj, &x, &y, &w, &h );
    *x_ret = x;
    *y_ret = y;

    ICO_TRA("CicoStatusBarClockComponent::GetClockStart Leave(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   default constructor
 *
 *  @param[in]  none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
CicoNotificationPanelComponent::CicoNotificationPanelComponent()
    : CicoCommonModule()
{
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   destructor
 *
 *  @param[in]  none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
CicoNotificationPanelComponent::~CicoNotificationPanelComponent()
{
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   initialize of notification component
 *
 *  @param[in]  window
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoNotificationPanelComponent::Initialize(Evas_Object *window, Evas *evas)
{
    ICO_TRA("CicoNotificationPanelComponent::Initialize Enter");

    if (evasobj_ != NULL) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    if (window == NULL) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    /*  create Notification object  */
    ICO_DBG("Create Text Module end");
    evasobj_=edje_object_add(evas);
    char    wrk_path[256];
    snprintf(wrk_path, sizeof(wrk_path), "%s/" THEME_PATH,
             tzplatform_getenv(TZ_SYS_RO_APP));
    if (!edje_object_file_set(evasobj_, wrk_path, THEME_NOTIFICATION_NAME)) {
        Edje_Load_Error err = edje_object_load_error_get(evasobj_);
        const char *errmsg = edje_load_error_str(err);
        ICO_ERR("could not load 'main' from statusbar.edj: %s",
                errmsg);
        return false;
    }
    Evas_Coord x,y,w,h;
    evas_object_geometry_get( window, &x, &y, &w, &h );
    evas_object_resize( evasobj_, w, h );

    /*  initial display  */
    SetText("");
    Hide();

    ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   set notification panel
 *
 *  @param[in]  text
 *  @param[in]  iconpaht
 *  @param[in]  soundpath
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
void
CicoNotificationPanelComponent::SetNotification(const char *text,
                                                const char *iconpath,
                                                const char *soundpath)
{
    ICO_TRA("CicoNotificationPanelComponent::SetNotification Enter"
            "(text=%s icon=%s sound=%s)", text, iconpath, soundpath);

    /*  set image file  */
    if (iconpath != NULL) {
        Evas_Object* obj = NULL;
        obj = (Evas_Object*)edje_object_part_object_get(
                     evasobj_, "noti_image");
        if ( obj != NULL ) {
            ICO_DBG("SetNotification image set[%s]",iconpath);
            evas_object_image_file_set(obj, iconpath, NULL);
        }
        else {
            ICO_DBG("SetNotification image set error[object not found]" );
        }
    }

    /*  set text  */
    if (text != NULL) {
        SetText(text);
    }

    Show();

    ICO_TRA("CicoNotificationPanelComponent::SetNotification Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set text to Content_text
 *  @param[in]  text
 *  @return     true: success
 */
//--------------------------------------------------------------------------
bool
CicoNotificationPanelComponent::SetText(const char *text)
{
    ICO_TRA("CicoNotificationPanelComponent::SetText Enter(text=%s)",
            text? text:"NULL");
    if ( text == NULL ) {
        edje_object_part_text_set(evasobj_, "content_text", "");
        return true;
    }
    edje_object_part_text_set(evasobj_, "content_text", text);

    ICO_TRA("CicoNotificationPanelComponent::SetText Leave");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set text to Content_text
 *  @param[in]  text
 *  @return     true: success
 */
//--------------------------------------------------------------------------
void
CicoNotificationPanelComponent::SetTextEndPosition( Evas_Coord x_end, Evas_Coord y_end)
{
    ICO_TRA("CicoNotificationPanelComponent::SetTextEndPosition Enter(%d,%d)",
            x_end,y_end);

    if (evasobj_ == NULL) {
        ICO_TRA("CicoNotificationPanelComponent::SetTextEndPosition Leave(false)");
        return;
    }

    Evas_Object* obj = NULL;
    obj = (Evas_Object*)edje_object_part_object_get(
                 evasobj_, "content_text" );
    if ( obj == NULL ) {
        ICO_TRA("CicoNotificationPanelComponent::SetTextEndPosition Leave(false)");
        return;
    }

    Evas_Coord x,y,w,h;
    evas_object_geometry_get( obj, &x, &y, &w, &h );
    evas_object_resize( evasobj_, x_end, h );
    ICO_TRA("CicoNotificationPanelComponent::text width %d -> %d",w, x_end - x -1);
    evas_object_resize( obj, x_end - x -1, h );
    edje_object_calc_force( obj );

    ICO_TRA("CicoNotificationPanelComponent::SetTextEndPosition Leave" );
}

// vim: set expandtab ts=4 sw=4:
