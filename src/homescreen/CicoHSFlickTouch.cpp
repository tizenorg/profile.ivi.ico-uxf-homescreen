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
 * @date    Sep^2--2013
 */
#include "CicoHSFlickTouch.h"
#include "CicoHSAppInfo.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"
#include "ico_syc_inputctl.h"
#include <linux/input.h>

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSControlBarWindow* CicoHSFlickTouch::ctl_bar_window;
CicoHSAppHistoryExt*    CicoHSFlickTouch::app_history;
int CicoHSFlickTouch::full_width;
int CicoHSFlickTouch::full_height;
int CicoHSFlickTouch::touch_state_b_x;
int CicoHSFlickTouch::touch_state_b_y;
int CicoHSFlickTouch::touch_state_a_x;
int CicoHSFlickTouch::touch_state_a_y;
int CicoHSFlickTouch::touch_lasttime;

Ecore_Timer *CicoHSFlickTouch::timer;
bool CicoHSFlickTouch::touch_down;
bool CicoHSFlickTouch::long_act;
bool CicoHSFlickTouch::set_xy_pos;

int CicoHSFlickTouch::num_windows;
CicoHSFlickInputWindow* CicoHSFlickTouch::flick_windows[ICO_HS_MAX_FLICKWINDOWS];

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickTouch::Initialize
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
CicoHSFlickTouch::Initialize(CicoHSControlBarWindow* ctl_bar, CicoHSAppHistoryExt* apphist,
                             int width, int height)
{
    timer = NULL;
    num_windows = 0;
    touch_down = false;
    long_act = false;
    set_xy_pos = false;

    ctl_bar_window = ctl_bar;
    app_history = apphist;
    full_width = width;
    full_height = height;
    touch_lasttime = 0;
    ICO_DBG("Initialize: ctlbar=%08x apphist=%08x width=%d height=%d",
            (int)ctl_bar, (int)apphist, width, height);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickTouch::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickTouch::Finalize(void)
{
  /*nothing to do*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickTouch::TouchFlick
 *          touch down action at flick input window
 *
 * @param[in]   data    CicoHSFlickInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickTouch::TouchDownFlick(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down   *info;
    CicoHSFlickInputWindow  *window;
    struct timeval ctime;

    info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    window = (CicoHSFlickInputWindow *)data;

    if ((info->output.x < 0) || (info->output.y < 0) ||
        (info->output.x >= 4096) || (info->output.y >= 4096))   {
        ICO_DBG("TouchDownFlick: illegal position(%d/%d)", info->output.x, info->output.y);
    }
    else if (set_xy_pos == false)   {
        set_xy_pos = true;
        touch_state_b_x = info->output.x + window->GetPosX();
        touch_state_b_y = info->output.y + window->GetPosY();
    }
    if (touch_down) {
        ICO_DBG("TouchDownFlick: dual touch down, Skip");
        return;
    }

    gettimeofday(&ctime, NULL);
    touch_lasttime = (ctime.tv_sec * 1000 + ctime.tv_usec/1000);

    ICO_DBG("TouchDownFlick: x/y=%d/%d->%d/%d", info->output.x, info->output.y,
            touch_state_b_x, touch_state_b_y);

    touch_down = true;
    long_act = false;
    timer = ecore_timer_add(ICO_HS_FLICK_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS,
                            LongPushed,NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   LongPushed::Touch timeout
 *          touch down timeout called from ecore
 *
 * @param[in]   data    user data(unused)
 * @return      fixed EINA_TRUE
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSFlickTouch::LongPushed(void *data)
{
    ICO_DBG("LongPushed: timedout");
    long_act = true;
    timer = NULL;

    /* send ABS_X/Y     */
    ICO_DBG("LongPushed: Not Flick, send Touch Down event to application");
    ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 2,
                       ABS_Z, (touch_state_b_x << 16) | touch_state_b_y);
    /* send TOUCH Down  */
    ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 1,
                       BTN_TOUCH, 1);
    /* send ABS_X/Y     */
    ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                       ABS_Z, ((touch_state_b_x+1) << 16) | (touch_state_b_y+1));

    return ECORE_CALLBACK_CANCEL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickTouch::TouchUpFlick
 *          touch up action at flick input window
 *
 * @param[in]   data    CicoHSFlickInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickTouch::TouchUpFlick(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up     *info;
    CicoHSFlickInputWindow  *window;
    CicoHSAppInfo           *appinfo;
    ico_hs_window_info      *wininfo;
    ico_syc_animation_t     animation;
    struct timeval ctime;
    int sub;
    int idx;
    int flick = 0;
    std::string histapp;
    std::string curapp;

    info = reinterpret_cast<Evas_Event_Mouse_Up*>(event_info);
    window = (CicoHSFlickInputWindow *)data;
    touch_state_a_x = info->output.x + window->GetPosX();
    touch_state_a_y = info->output.y + window->GetPosY();
    set_xy_pos = false;

    ICO_DBG("TouchUpFlick: x/y=%d/%d->%d/%d (before %d/%d)",
            info->output.x, info->output.y,
            touch_state_b_x, touch_state_b_y, touch_state_a_x, touch_state_a_y);

    if(timer != NULL){
        ecore_timer_del(timer);
        timer = NULL;
    }
    /* long push    */
    if((touch_down == false) || (long_act == true)) {
        ICO_DBG("TouchUpFlick: timedout");
        touch_down = false;

        /* send touch release event */
        gettimeofday(&ctime, NULL);
        touch_lasttime = (ctime.tv_sec * 1000 + ctime.tv_usec/1000);

        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 2,
                           ABS_Z, (touch_state_a_x << 16) | touch_state_a_y);
        /* send TOUCH Up    */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 1,
                           BTN_TOUCH, 0);
        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                           ABS_Z, ((touch_state_a_x+1) << 16) | (touch_state_a_y+1));
        return;
    }
    touch_down = false;
    sub = touch_state_a_x - touch_state_b_x;

    /* check slide left to right or right to left   */
    if (sub > ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE) {
        if (touch_state_b_x < ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE) {
            if (CicoHSSystemState::getInstance()->getRegulation() == true)  {
                ICO_DBG("TouchUpFlick: Flick left side to right, but Regulation=ON");
            }
            else    {
                /* flick at left side to right = back before application*/
                ICO_DBG("TouchUpFlick: Flick left side to right");

                /* get before application   */
                curapp = app_history->getSwipeCurrentAppid();
                histapp = app_history->prevSwipe();
                ICO_DBG("TouchUpFlick: Flick left to right(cur/prev=%s/%s)",
                        curapp.c_str(), histapp.c_str());
                if (histapp.empty())    {
                    ICO_DBG("TouchUpFlick: Flick left to right(prev not exist)");
                }
                else    {
                    animation.time = ICO_HS_FLICK_TOUCH_FLICK_ANIMA_TIME |
                                     ICO_SYC_WIN_SURF_RAISE | ICO_SYC_WIN_SURF_NORESCTL;

                    /* show before application with slide to right  */
                    appinfo = CicoHomeScreen::GetAppInfo(histapp.c_str());
                    if (appinfo)    {
                        animation.name = (char *)"slide.toright";
                        for (idx = 0; ; idx++)  {
                            wininfo = appinfo->GetWindowInfo(idx);
                            if (! wininfo)  break;
                            ICO_DBG("TouchUpFlick: %s.%d surface=%08x",
                                    wininfo->appid, idx, wininfo->surface);
                            ico_syc_show(wininfo->appid, wininfo->surface, &animation);
                        }
                    }
                    else    {
                        ICO_DBG("TouchUpFlick: prev app(%s) dose not exist", histapp.c_str());
                    }

                    /* hide current applicaiton with slide to right */
                    if (curapp.empty()) {
                        ICO_DBG("TouchUpFlick: Flick left to right(current not exist)");
                    }
                    else    {
                        animation.time = ICO_HS_FLICK_TOUCH_FLICK_ANIMA_TIME |
                                         ICO_SYC_WIN_SURF_NORESCTL;
                        appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                        if (appinfo)    {
                            animation.name = (char *)"slide.toleft";
                            for (idx = 0; ; idx++)  {
                                wininfo = appinfo->GetWindowInfo(idx);
                                if (! wininfo)  break;
                                ICO_DBG("TouchUpFlick: %s.%d surface=%08x",
                                        wininfo->appid, idx, wininfo->surface);
                                ico_syc_hide(wininfo->appid, wininfo->surface, &animation);
                            }
                        }
                        else    {
                            ICO_DBG("TouchUpFlick: current app(%s) dose not exist",
                                    curapp.c_str());
                        }
                    }
                    /* set history timer                            */
                    app_history->selectApp(histapp.c_str());
                }
            }
            flick = 1;
        }
        else    {
            ICO_DBG("TouchUpFlick: Flick left to right, but nop");
            flick = -1;
        }
    }
    else if (sub < (-1 * ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE)) {
        if (touch_state_b_x >
            (full_width - ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE))    {
            if (CicoHSSystemState::getInstance()->getRegulation() == true)  {
                ICO_DBG("TouchUpFlick: Flick right side to left, but Regulation=ON");
            }
            else    {
                /* flick at right side to left = go next applicaton     */

                /* get next application     */
                curapp = app_history->getSwipeCurrentAppid();
                histapp = app_history->nextSwipe();
                ICO_DBG("TouchUpFlick: Flick right to left(cur/next=%s/%s)",
                        curapp.c_str(), histapp.c_str());
                if (histapp.empty())    {
                    ICO_DBG("TouchUpFlick: Flick right to left(next not exist)");
                }
                else    {
                    animation.time = ICO_HS_FLICK_TOUCH_FLICK_ANIMA_TIME |
                                     ICO_SYC_WIN_SURF_RAISE | ICO_SYC_WIN_SURF_NORESCTL;

                    /* show next application with slide to left     */
                    appinfo = CicoHomeScreen::GetAppInfo(histapp.c_str());
                    if (appinfo)    {
                        animation.name = (char *)"slide.toleft";
                        for (idx = 0; ; idx++)  {
                            wininfo = appinfo->GetWindowInfo(idx);
                            if (! wininfo)  break;
                            ICO_DBG("TouchUpFlick: %s.%d surface=%08x",
                                    wininfo->appid, idx, wininfo->surface);
                            ico_syc_show(wininfo->appid, wininfo->surface, &animation);
                        }
                    }
                    else    {
                        ICO_DBG("TouchUpFlick: next app(%s) dose not exist", histapp.c_str());
                    }

                    /* hide current applicaiton with slide to left  */
                    if (curapp.empty()) {
                        ICO_DBG("TouchUpFlick: Flick right to left(current not exist)");
                    }
                    else    {
                        animation.time = ICO_HS_FLICK_TOUCH_FLICK_ANIMA_TIME |
                                         ICO_SYC_WIN_SURF_NORESCTL;
                        appinfo = CicoHomeScreen::GetAppInfo(curapp.c_str());
                        if (appinfo)    {
                        animation.name = (char *)"slide.toright";
                            for (idx = 0; ; idx++)  {
                                wininfo = appinfo->GetWindowInfo(idx);
                                if (! wininfo)  break;
                                ICO_DBG("TouchUpFlick: %s.%d surface=%08x",
                                        wininfo->appid, idx, wininfo->surface);
                                ico_syc_hide(wininfo->appid, wininfo->surface, &animation);
                            }
                        }
                        else    {
                            ICO_DBG("TouchUpFlick: current app(%s) dose not exist",
                                    curapp.c_str());
                        }
                    }
                    /* set history timer                            */
                    app_history->selectApp(histapp.c_str());
                }
            }
            flick = 1;
        }
        else    {
            ICO_DBG("TouchUpFlick: Flick right side to left, but nop");
            flick = -1;
        }
    }

#if 0       /* currently not support    */
    /* check slide buttom to top or top to buttom   */
    sub = touch_state_a_y - touch_state_b_y;
    if (sub > ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE) {
        if (touch_state_b_y < ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE) {
            /* flick at top side to buttom = unknown    */
            ICO_DBG("TouchUpFlick: Flick top side to buttom");
            flick = 1;
        }
        else    {
            ICO_DBG("TouchUpFlick: Flick top side to buttom, but nop");
            flick = -1;
        }
    }
    else if (sub < (-1 * ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE)) {
        if (touch_state_b_y >
            (full_width - ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE))    {
            /* flick at buttom side to top = show home menu screen  */
            ICO_DBG("TouchUpFlick: Flick buttom side to top");
            flick = 1;
            if (ctl_bar_window) {
                ctl_bar_window->TouchHome();
            }
        }
        else    {
            ICO_DBG("TouchUpFlick: Flick buttom side to top, but nop");
            flick = -1;
        }
    }
#endif
    if (flick <= 0) {
        /* send touch press event   */
        ICO_DBG("TouchUpFlick: Not Flick, send event to application");
        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 2,
                           ABS_Z, (touch_state_b_x << 16) | touch_state_b_y);
        /* send TOUCH Down  */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 1,
                           BTN_TOUCH, 1);
        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                           ABS_Z, ((touch_state_b_x+1) << 16) | (touch_state_b_y+1));

        /* send touch release event */
        gettimeofday(&ctime, NULL);
        touch_lasttime = (ctime.tv_sec * 1000 + ctime.tv_usec/1000);

        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 2,
                           ABS_Z, (touch_state_a_x << 16) | touch_state_a_y);
        /* send TOUCH Up    */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 1,
                           BTN_TOUCH, 0);
        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                           ABS_Z, ((touch_state_a_x+1) << 16) | (touch_state_a_y+1));
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickTouch::TouchMoveFlick
 *          touch move action at flick input window
 *
 * @param[in]   data    CicoHSFlickInputWindow object
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickTouch::TouchMoveFlick(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Move   *info;
    CicoHSFlickInputWindow  *window;
    struct timeval ctime;

    info = reinterpret_cast<Evas_Event_Mouse_Move*>(event_info);

    ICO_DBG("TouchMoveFlick: button=%x cur.x/y=%d/%d prv.x/y=%d/%d",
            info->buttons, info->cur.output.x, info->cur.output.y,
            info->prev.output.x, info->prev.output.y);

    window = (CicoHSFlickInputWindow *)data;
    if ((info->cur.output.x < 0) || (info->cur.output.y < 0) ||
        (info->cur.output.x >= 4096) || (info->cur.output.y >= 4096))   {
        ICO_DBG("TouchMoveFlick: Illegal position(x/y=%d/%d), Skip",
                info->cur.output.x, info->cur.output.y);
        return;
    }
    if (set_xy_pos == false)    {
        set_xy_pos = true;
        touch_state_b_x = info->cur.output.x + window->GetPosX();
        touch_state_b_y = info->cur.output.y + window->GetPosY();
    }
    touch_state_a_x = info->cur.output.x + window->GetPosX();
    touch_state_a_y = info->cur.output.y + window->GetPosY();

    /* long push    */
    if((touch_down == false) || (long_act == true)) {
        ICO_DBG("TouchMoveFlick: not down(%d) or timedout(%d)",
                (int)touch_down, (int)long_act);

        if(timer != NULL){
            ecore_timer_del(timer);
            timer = NULL;
        }

        /* send ABS_X/Y     */
        gettimeofday(&ctime, NULL);
        touch_lasttime = (ctime.tv_sec * 1000 + ctime.tv_usec/1000);

        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                           ABS_Z, (touch_state_a_x << 16) | touch_state_a_y);
        return;
    }

    if (abs(touch_state_b_y - touch_state_a_y)
        > ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_MOVE_Y)  {
        /* slide to top or buttom over threashold, flick cancel */
        ICO_DBG("TouchMoveFlick: over Y direction");
        if(timer != NULL){
            ecore_timer_del(timer);
            timer = NULL;
        }
        long_act = true;

        /* send ABS_X/Y     */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 2,
                           ABS_Z, (touch_state_b_x << 16) | touch_state_b_y);
        /* send TOUCH Down  */
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime - 1,
                               BTN_TOUCH, 1);
        /* send ABS_X/Y     */
        gettimeofday(&ctime, NULL);
        touch_lasttime = (ctime.tv_sec * 1000 + ctime.tv_usec/1000);
        ico_syc_send_input("\0", 0, ICO_SYC_INPUT_TYPE_TOUCH, 0, touch_lasttime,
                           ABS_Z, (touch_state_a_x << 16) | touch_state_a_y);
    }
}
