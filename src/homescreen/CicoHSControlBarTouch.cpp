/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action of control bar
 *
 * @date    Aug-08-2013
 */
#include "CicoHSControlBarTouch.h"

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSControlBarWindow* CicoHSControlBarTouch::ctl_bar_window;
bool CicoHSControlBarTouch::touch_down;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarTouch::Initialize
 *          Initialize
 *
 * @param[in]   ctl_bar_window    instance of control bar window
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarTouch::Initialize(CicoHSControlBarWindow* ctl_bar_window)
{
    CicoHSControlBarTouch::ctl_bar_window = ctl_bar_window;
    touch_down = false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarTouch::Finalize
 *          Finalize
 *
 * @param[in]   void
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarTouch::Finalize(void)
{
  /*nothing to do*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarTouch::TouchDownControlBar
 *          touch down action at control bar
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarTouch::TouchDownControlBar(void *data, Evas *evas,
                                           Evas_Object *obj, void *event_info)
{
    touch_down = true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarTouch::TouchUpControlBar
 *          touch up action at control bar
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarTouch::TouchUpControlBar(void *data, Evas *evas,
                                         Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);

    ICO_DBG("CicoHSControlBarTouch::TouchUpControlBar Enter(down=%d)",
            (int)touch_down);

    if (touch_down == false)    {
        ICO_DBG("CicoHSControlBarTouch::TouchUpControlBar No Down");
        // Menu manipulation is normally processed, even if there is no touchdown.
    }
    touch_down = false;

    if (data == NULL) {
        ICO_PRF("TOUCH_EVENT Ctrl-Bar Down->Up (%d,%d) app=(NIL)",
                info->output.x, info->output.y);
        ctl_bar_window->TouchHome();
    }
    else {
        ICO_PRF("TOUCH_EVENT Ctrl-Bar Down->Up (%d,%d) app=%s",
                info->output.x, info->output.y, (const char *)data);
        ctl_bar_window->TouchShortcut((const char *)data);
    }
    ICO_DBG("CicoHSControlBarTouch::TouchUpControlBar Leave");
}
// vim: set expandtab ts=4 sw=4:
