/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action of menu
 *
 * @date    Sep-20-2013
 */
#include "CicoHSSwipeTouch.h"
#include "CicoHSAppInfo.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"
#include "ico_syc_inputctl.h"
#include <linux/input.h>

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSControlBarWindow* CicoHSSwipeTouch::ctl_bar_window;
CicoHSAppHistoryExt*    CicoHSSwipeTouch::app_history;
int CicoHSSwipeTouch::full_width;
int CicoHSSwipeTouch::full_height;
int CicoHSSwipeTouch::touch_state_b_x;
int CicoHSSwipeTouch::touch_state_b_y;
int CicoHSSwipeTouch::touch_state_a_x;
int CicoHSSwipeTouch::touch_state_a_y;

Ecore_Timer *CicoHSSwipeTouch::timer;
int CicoHSSwipeTouch::touch_down;
bool CicoHSSwipeTouch::long_act;
bool CicoHSSwipeTouch::set_xy_pos;

int CicoHSSwipeTouch::num_windows;
CicoHSSwipeInputWindow* CicoHSSwipeTouch::swipe_windows[ICO_HS_MAX_SWIPEWINDOWS];

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeTouch::Initialize
 *          Initialize
 *
 * @param[in]   ctl_bar     instance of control bar window
 * @param[in]   apphist     instance of application history control
 * @param[in]   width       screen full width
 * @param[in]   height      screen full height
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeTouch::Initialize(CicoHSControlBarWindow* ctl_bar, CicoHSAppHistoryExt* apphist,
                             int width, int height)
{
    timer = NULL;
    num_windows = 0;
    touch_down = 0;
    long_act = false;
    set_xy_pos = false;

    ctl_bar_window = ctl_bar;
    app_history = apphist;
    full_width = width;
    full_height = height;

    ICO_DBG("Initialize: ctlbar=%08x apphist=%08x width=%d height=%d",
            (int)ctl_bar, (int)apphist, width, height);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeTouch::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeTouch::Finalize(void)
{
    /* nothing to do    */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeTouch::TouchSwipe
 *          touch down action at swipe input window
 *
 * @param[in]   data    CicoHSSwipeInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeTouch::TouchDownSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down   *info;
    CicoHSSwipeInputWindow  *window;
    int     x, y;

    if (timer)  {
        ecore_timer_del(timer);
        timer = NULL;
    }
    touch_down ++;

    info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    window = (CicoHSSwipeInputWindow *)data;
    x = info->output.x + window->GetPosX();
    y = info->output.y + window->GetPosY();
    ICO_PRF("TOUCH_EVENT Swipe Down (%d,%d)->(%d,%d) (%d)",
            info->output.x, info->output.y, x, y, touch_down);

    if ((x >= 0) && (x < 4096) && (y >= 0) && (y < 4096))   {
        if (set_xy_pos == false)    {
            set_xy_pos = true;
            touch_state_b_x = x;
            touch_state_b_y = y;
        }
        touch_state_a_x = x;
        touch_state_a_y = y;
    }

    if (touch_down == 1)   {
        long_act = false;
        timer = ecore_timer_add(ICO_HS_SWIPE_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS,
                                LongPushed, data);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   LongPushed::Touch timeout
 *          touch down timeout called from ecore
 *
 * @param[in]   data    CicoHSSwipeInputWindow object
 * @return      fixed ECORE_CALLBACK_CANCEL
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSSwipeTouch::LongPushed(void *data)
{
    int     idx;

    ICO_DBG("LongPushed: timedout");

    long_act = true;
    timer = NULL;

    if (set_xy_pos == false)    {
        ICO_DBG("LongPushed: unknown coordinate, Skip");

        long_act = false;
        return ECORE_CALLBACK_CANCEL;
    }
    ICO_DBG("LongPushed: Not Swipe, send Touch Down event to application");

    /* grab off             */
    CicoHSSwipeInputWindow *window = static_cast<CicoHSSwipeInputWindow *>(data);
    if (window) {
        window->GrabOff();
    }

    /* send start X/Y       */
    ico_syc_send_pointer_event(" ", ABS_Z | ICO_SYC_INPUT_CODE_TOUCH_CANCEL,
                               (touch_state_b_x << 16) | touch_state_b_y);
    /* send TOUCH Down      */
    for (idx = 0; idx < touch_down; idx++)  {
        ico_syc_send_pointer_event(" ", BTN_TOUCH | ICO_SYC_INPUT_CODE_TOUCH_DOWN,
                                   (touch_state_b_x << 16) | touch_state_b_y);
    }
    /* send current X/Y     */
    ico_syc_send_pointer_event(" ", ABS_Z,
                               (touch_state_a_x << 16) | touch_state_a_y);
    return ECORE_CALLBACK_CANCEL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeTouch::TouchUpSwipe
 *          touch up action at swipe input window
 *
 * @param[in]   data    CicoHSSwipeInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeTouch::TouchUpSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up     *info;
    CicoHSSwipeInputWindow  *window;
    CicoHSAppInfo           *appinfo;
    ico_hs_window_info      *wininfo;
    ico_syc_animation_t     animation;
    int sub;
    int idx;
    int swipe = 0;
    std::string histapp;
    std::string curapp;
    int x, y;

    info = reinterpret_cast<Evas_Event_Mouse_Up*>(event_info);
    window = (CicoHSSwipeInputWindow *)data;
    x = info->output.x + window->GetPosX();
    y = info->output.y + window->GetPosY();
    if ((x >= 0) && (x < 4096) && (y >= 0) && (y < 4096))   {
        touch_state_a_x = x;
        touch_state_a_y = y;
    }

    ICO_PRF("TOUCH_EVENT Swipe Up   (%d,%d)->(%d,%d) before(%d,%d) (%d)",
            info->output.x, info->output.y, touch_state_a_x, touch_state_a_y,
            touch_state_b_x, touch_state_b_y, touch_down - 1);

    if (touch_down != 1) {
        if (touch_down > 0) touch_down --;
        ICO_DBG("TouchUpSwipe: touch counter not 0(%d), Skip", touch_down);
        return;
    }

    if(timer != NULL){
        ecore_timer_del(timer);
        timer = NULL;
    }

    touch_down = 0;
    set_xy_pos = false;

    if (long_act == true)   {
        long_act = false;
        ICO_DBG("TouchUpSwipe: no touch down, Skip");
        return;
    }

    sub = touch_state_a_x - touch_state_b_x;

    swipe = -1;

    /* check slide left to right or right to left   */
    if (sub > ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_DISTANCE) {
        if (touch_state_b_x < ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH)   {
            /* get current application  */
            curapp = app_history->getSwipeCurrentAppid();

            if (CicoHSSystemState::getInstance()->getRegulation() == true)  {
                ICO_DBG("TouchUpSwipe: Swipe left side to right, but Regulation=ON");
                appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                if (appinfo)    {
                    ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                }
            }
            else    {
                /* swipe at left side to right = back before application*/
                ICO_DBG("TouchUpSwipe: Swipe left side to right");

                /* get before application   */
                histapp = app_history->prevSwipe();
                ICO_DBG("TouchUpSwipe: Swipe left to right(cur/prev=%s/%s)",
                        curapp.c_str(), histapp.c_str());
                if (histapp.empty())    {
                    ICO_DBG("TouchUpSwipe: Swipe left to right(prev not exist)");
                    appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                    if (appinfo)    {
                        ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                    }
                }
                else    {
                    animation.time = ICO_HS_SWIPE_TOUCH_SWIPE_ANIMA_TIME |
                                     ICO_SYC_WIN_SURF_RAISE | ICO_SYC_WIN_SURF_NORESCTL;

                    /* show before application with slide to right  */
                    appinfo = CicoHomeScreen::GetAppInfo(histapp.c_str());
                    if (appinfo)    {
                        animation.name = (char *)"slide.toright";
                        for (idx = 0; ; idx++)  {
                            wininfo = appinfo->GetWindowInfo(idx);
                            if (! wininfo)  break;

                            /* reset avtive surface         */
                            if (idx == 0)   {
                                ico_syc_change_active(wininfo->appid, 0);
                            }
                            ICO_DBG("TouchUpSwipe: %s.%d surface=%08x",
                                    wininfo->appid, idx, wininfo->surface);
                            ico_syc_show(wininfo->appid, wininfo->surface, &animation);
                        }

                        /* hide current applicaiton with slide to right */
                        if (curapp.empty()) {
                            ICO_DBG("TouchUpSwipe: Swipe left to right(current not exist)");
                        }
                        else    {
                            animation.time = ICO_HS_SWIPE_TOUCH_SWIPE_ANIMA_TIME |
                                             ICO_SYC_WIN_SURF_NORESCTL;
                            appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                            if (appinfo)    {
                                animation.name = (char *)"slide.toleft";
                                for (idx = 0; ; idx++)  {
                                    wininfo = appinfo->GetWindowInfo(idx);
                                    if (! wininfo)  break;
                                    ICO_DBG("TouchUpSwipe: %s.%d surface=%08x",
                                            wininfo->appid, idx, wininfo->surface);
                                    ico_syc_hide(wininfo->appid,
                                                 wininfo->surface, &animation);
                                }
                            }
                            else    {
                                ICO_DBG("TouchUpSwipe: current app(%s) dose not exist",
                                        curapp.c_str());
                            }
                        }
                        /* set history timer                        */
                        app_history->selectApp(histapp.c_str());
                    }
                    else    {
                        ICO_DBG("TouchUpSwipe: prev app(%s) dose not exist", histapp.c_str());
                        appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                        if (appinfo)    {
                            ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                        }
                    }
                }
            }
            swipe = 1;
        }
        else    {
            ICO_DBG("TouchUpSwipe: Swipe left to right, but nop");
        }
    }
    else if (sub < (-1 * ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_DISTANCE)) {
        if (touch_state_b_x >=
            (full_width - ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH))  {
            /* get current application  */
            curapp = app_history->getSwipeCurrentAppid();

            if (CicoHSSystemState::getInstance()->getRegulation() == true)  {
                ICO_DBG("TouchUpSwipe: Swipe right side to left, but Regulation=ON");
                appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                if (appinfo)    {
                    ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                }
            }
            else    {
                /* swipe at right side to left = go next applicaton     */

                /* get next application     */
                histapp = app_history->nextSwipe();
                ICO_DBG("TouchUpSwipe: Swipe right to left(cur/next=%s/%s)",
                        curapp.c_str(), histapp.c_str());
                if (histapp.empty())    {
                    ICO_DBG("TouchUpSwipe: Swipe right to left(next not exist)");
                    appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                    if (appinfo)    {
                        ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                    }
                }
                else    {
                    animation.time = ICO_HS_SWIPE_TOUCH_SWIPE_ANIMA_TIME |
                                     ICO_SYC_WIN_SURF_RAISE | ICO_SYC_WIN_SURF_NORESCTL;

                    /* show next application with slide to left     */
                    appinfo = CicoHomeScreen::GetAppInfo(histapp.c_str());
                    if (appinfo)    {
                        animation.name = (char *)"slide.toleft";
                        for (idx = 0; ; idx++)  {
                            wininfo = appinfo->GetWindowInfo(idx);
                            if (! wininfo)  break;

                            /* reset avtive surface         */
                            if (idx == 0)   {
                                ico_syc_change_active(wininfo->appid, 0);
                            }
                            ICO_DBG("TouchUpSwipe: %s.%d surface=%08x",
                                    wininfo->appid, idx, wininfo->surface);
                            ico_syc_show(wininfo->appid, wininfo->surface, &animation);
                        }

                        /* hide current applicaiton with slide to left  */
                        if (curapp.empty()) {
                            ICO_DBG("TouchUpSwipe: Swipe right to left(current not exist)");
                        }
                        else    {
                            animation.time = ICO_HS_SWIPE_TOUCH_SWIPE_ANIMA_TIME |
                                             ICO_SYC_WIN_SURF_NORESCTL;
                            appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                            if (appinfo)    {
                            animation.name = (char *)"slide.toright";
                                for (idx = 0; ; idx++)  {
                                    wininfo = appinfo->GetWindowInfo(idx);
                                    if (! wininfo)  break;
                                    ICO_DBG("TouchUpSwipe: %s.%d surface=%08x",
                                            wininfo->appid, idx, wininfo->surface);
                                    ico_syc_hide(wininfo->appid,
                                                 wininfo->surface, &animation);
                                }
                            }
                            else    {
                                ICO_DBG("TouchUpSwipe: current app(%s) dose not exist",
                                        curapp.c_str());
                            }
                        }
                        /* set history timer                            */
                        app_history->selectApp(histapp.c_str());
                    }
                    else    {
                        ICO_DBG("TouchUpSwipe: next app(%s) dose not exist", histapp.c_str());
                        appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                        if (appinfo)    {
                            ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
                        }
                    }
                }
            }
            swipe = 1;
        }
        else    {
            ICO_DBG("TouchUpSwipe: Swipe right side to left, but nop");
        }
    }

#if 0       /* currently not support    */
    /* check slide buttom to top or top to buttom   */
    sub = touch_state_a_y - touch_state_b_y;
    if (sub > ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_DISTANCE) {
        if (touch_state_b_y < ICO_HS_SWIPE_TOUCH_SWIPE_HEIGHT)  {
            /* swipe at top side to buttom = unknown    */
            ICO_DBG("TouchUpSwipe: Swipe top side to buttom");
            swipe = 1;
        }
        else    {
            ICO_DBG("TouchUpSwipe: Swipe top side to buttom, but nop");
        }
    }
    else if (sub < (-1 * ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_DISTANCE)) {
        if (touch_state_b_y >=
            (full_width - ICO_HS_SWIPE_TOUCH_SWIPE_HEIGHT)) {
            /* swipe at buttom side to top = show home menu screen  */
            ICO_DBG("TouchUpSwipe: Swipe buttom side to top");
            swipe = 1;
            if (ctl_bar_window) {
                ctl_bar_window->TouchHome();
            }
        }
        else    {
            ICO_DBG("TouchUpSwipe: Swipe buttom side to top, but nop");
        }
    }
#endif
    if (swipe <= 0) {
        ICO_DBG("TouchUpSwipe: Not Swipe, send event to application");
        /* send start X/Y       */
        ico_syc_send_pointer_event(" ", ABS_Z,
                                   (touch_state_b_x << 16) | touch_state_b_y);
        /* send TOUCH Down      */
        ico_syc_send_pointer_event(" ", BTN_TOUCH | ICO_SYC_INPUT_CODE_TOUCH_DOWN,
                                   (touch_state_b_x << 16) | touch_state_b_y);
        /* send current X/Y     */
        ico_syc_send_pointer_event(" ", ABS_Z,
                                   (touch_state_a_x << 16) | touch_state_a_y);
        /* send TOUCH Up        */
        ico_syc_send_pointer_event(" ", BTN_TOUCH | ICO_SYC_INPUT_CODE_TOUCH_UP,
                                   (touch_state_a_x << 16) | touch_state_a_y);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeTouch::TouchMoveSwipe
 *          touch move action at swipe input window
 *
 * @param[in]   data    CicoHSSwipeInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeTouch::TouchMoveSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Move   *info;
    CicoHSSwipeInputWindow  *window;
    int     idx;
    int     x, y;

    info = reinterpret_cast<Evas_Event_Mouse_Move*>(event_info);

    window = (CicoHSSwipeInputWindow *)data;
    x = info->cur.output.x + window->GetPosX();
    y = info->cur.output.y + window->GetPosY();
    if ((x < 0) || (x >= 4096) || (y < 0) || (y >= 4096))   {
        ICO_DBG("TouchMoveSwipe: Illegal position(x/y=%d/%d->%d/%d), Skip",
                info->cur.output.x, info->cur.output.y, touch_state_a_x, touch_state_a_y);
        return;
    }
    touch_state_a_x = x;
    touch_state_a_y = y;

    if (set_xy_pos == false)    {
        touch_state_b_x = touch_state_a_x;
        touch_state_b_y = touch_state_a_y;
        ICO_DBG("TouchMoveSwipe: save x/y=%d/%d", touch_state_b_x, touch_state_b_y);
    }

    ICO_DBG("TouchMoveSwipe: Swipe Move (%d,%d)->(%d,%d) Button=%x",
            info->cur.output.x, info->cur.output.y,
            touch_state_a_x, touch_state_a_y, info->buttons);

    if (long_act == true)   {
        ICO_DBG("TouchMoveSwipe: long act, Skip");
        return;
    }

    if(touch_down == 0) {
        ICO_DBG("TouchMoveSwipe: no TouchDown, Skip");
        return;
    }

    if (abs(touch_state_b_y - touch_state_a_y)
        > ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_MOVE_Y)  {
        /* slide to top or buttom over threashold, swipe cancel */
        ICO_DBG("TouchMoveSwipe: over Y direction");
        if(timer != NULL){
            ecore_timer_del(timer);
            timer = NULL;
        }
        long_act = true;

        /* grab off             */
        CicoHSSwipeInputWindow *window = static_cast<CicoHSSwipeInputWindow *>(data);
        if (window) {
            window->GrabOff();
        }

        /* send start X/Y       */
        ico_syc_send_pointer_event(" ", ABS_Z | ICO_SYC_INPUT_CODE_TOUCH_CANCEL,
                                   (touch_state_b_x << 16) | touch_state_b_y);
        /* send TOUCH Down      */
        for (idx = 0; idx < touch_down; idx++)  {
            ico_syc_send_pointer_event(" ", BTN_TOUCH | ICO_SYC_INPUT_CODE_TOUCH_DOWN,
                                       (touch_state_b_x << 16) | touch_state_b_y);
        }
        /* send current X/Y     */
        ico_syc_send_pointer_event(" ", ABS_Z,
                                   (touch_state_a_x << 16) | touch_state_a_y);
    }
}
// vim: set expandtab ts=4 sw=4:
