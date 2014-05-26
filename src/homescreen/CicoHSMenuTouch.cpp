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
 * @date    Aug-08-2013
 */
#include "CicoHomeScreen.h"
#include "CicoHSMenuTouch.h"
#include "CicoSound.h"

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
char CicoHSMenuTouch::terminate_appid[ICO_HS_MAX_PROCESS_NAME];
int CicoHSMenuTouch::touch_state_b_x;
int CicoHSMenuTouch::touch_state_b_y;
int CicoHSMenuTouch::touch_state_a_x;
int CicoHSMenuTouch::touch_state_a_y;

Ecore_Timer *CicoHSMenuTouch::timer;
bool CicoHSMenuTouch::long_act;
bool CicoHSMenuTouch::touch_down;

Evas_Object *CicoHSMenuTouch::grabbed_object;

CicoHSMenuWindow* CicoHSMenuTouch::menu_window;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::Initialize
 *          Initialize
 *
 * @param[in]   menu_window    instance of menu window
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::Initialize(CicoHSMenuWindow* menu_window)
{
    timer = NULL;
    long_act = false;
    touch_down = false;
    grabbed_object = NULL;

    CicoHSMenuTouch::menu_window = menu_window;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::Finalize(void)
{
  /*nothing to do*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchDownMenu
 *          touch down action at menu
 *
 * @param[in]   data        data
 * @param[in]   evas        evas
 * @param[in]   obj         object
 * @param[in]   event_info  event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchDownMenu(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *info;
    int     x, y;

    info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    x = info->output.x;
    y = info->output.y;

    touch_down = true;

    if (timer)  {
        ecore_timer_del(timer);
        timer = NULL;
        if ((x >= 0) && (y >= 0))   {
            touch_state_b_x = x;
            touch_state_b_y = y;
        }
    }
    else    {
        touch_state_b_x = x;
        touch_state_b_y = y;
    }

    char *appid = reinterpret_cast<char*>(data);

    if (appid == NULL) {
        ICO_DBG("CicoHSMenuTouch::TouchDownMenu: %08x (%d,%d) No App",
            (int)data, touch_state_b_x, touch_state_b_y);
        return;
    }
    ICO_PRF("TOUCH_EVENT Menu Down (%d,%d) app=%s",
            touch_state_b_x, touch_state_b_y, appid);

    long_act = false;
    timer = ecore_timer_add(ICO_HS_MENU_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS,
                            LongPushed, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   LongPushed::Touch timeout
 *          touch down timeout called from ecore
 *
 * @param[in]   data    user data(unused)
 * @return      fixed ECORE_CALLBACK_CANCEL
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSMenuTouch::LongPushed(void *data)
{
    ICO_TRA("CicoHSMenuTouch::LongPushed Enter");
    timer = NULL;

    if (CicoHomeScreen::getInstance()->GetMode() == ICO_HS_MODE_MENU)   {
        long_act = true;

        /*stop select*/
        menu_window->ChangeTerminateMode();

        ICO_TRA("CicoHSMenuTouch::LongPushed Leave");
    }
    else    {
        long_act = false;
        ICO_TRA("CicoHSMenuTouch::LongPushed Leave(Not in Menu)");
    }
    return ECORE_CALLBACK_CANCEL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchUpMenu
 *          touch up action at menu
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchUpMenu(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up *info;
    char    *appid = reinterpret_cast<char*>(data);
    int     sub_x;
    int     sub_y;
    int     x, y;

    info = reinterpret_cast<Evas_Event_Mouse_Up*>(event_info);
    x = info->output.x;
    y = info->output.y;

    if (timer != NULL) {
        ecore_timer_del(timer);
        timer = NULL;
    }
    else if (touch_down == false)   {
        ICO_DBG("CicoHSMenuTouch::TouchUpMenu: (%d,%d) No Down", x, y);
        // Menu manipulation is normally processed, even if there is no touchdown.
        touch_state_b_x = x;
        touch_state_b_y = y;
    }
    touch_down = false;

    /* long push*/
    if (long_act == true) {
        ICO_DBG("CicoHSMenuTouch::TouchUpMenu: timedout");
        long_act = false;
        return;
    }

    touch_state_a_x = x;
    touch_state_a_y = y;

    sub_x = touch_state_a_x - touch_state_b_x;
    sub_y = touch_state_a_y - touch_state_b_y;

    ICO_PRF("TOUCH_EVENT Menu Up   (%d,%d)->(%d,%d) app=%s",
            touch_state_b_x, touch_state_b_y,
            touch_state_a_x, touch_state_a_y, appid ? appid: "(NULL)");

    if (abs(sub_x) > abs(sub_y)) {
        /* menu slide*/
        if (sub_x > ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE) {
            menu_window->GoBackMenu();
            touch_state_b_x = 0;
            touch_state_b_y = 0;
            return;
        }
        else if (sub_x < (-1 * ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE)) {
            menu_window->GoNextMenu();
            touch_state_b_x = 0;
            touch_state_b_y = 0;
            return;
        }
    }
    else {
        if (sub_y > ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE) {
            menu_window->UpBackMenu();
            touch_state_b_x = 0;
            touch_state_b_y = 0;
            return;
        }
        else if (sub_y < (-1 * ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE)) {
            menu_window->DownNextMenu();
            touch_state_b_x = 0;
            touch_state_b_y = 0;
            return;
        }
    }

    /*execute application*/
    if (appid != NULL) {
        // play operation sound
        CicoSound::GetInstance()->PlayOperationSound();

        menu_window->ExecuteApp(appid);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::FocusOutEvas
 *          focus out at menu evas
 *
 * @param[in]   data        data(canvas object, unused)
 * @param[in]   evas        evas(unused)
 * @param[in]   event_info  event information(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::FocusOutEvas(void *data, Evas *evas, void *event_info)
{
    // Ungrab focus
    if (grabbed_object) {
        evas_object_focus_set(grabbed_object, EINA_FALSE);
        grabbed_object = NULL;
    }

    // Focus Out, Cancel menu touch
    if (timer != NULL) {
        // delete timer
        ecore_timer_del(timer);
        timer = NULL;
    }
    else if (touch_down == false)   {
        ICO_DBG("CicoHSMenuTouch::FocusOutEvas: No Down, Skip");
        return;
    }
    touch_down = false;

    if (long_act)   {
        // Stop Terminate Mode
        long_act = false;
        menu_window->ChangeNormalMode();
    }

    ICO_PRF("TOUCH_EVENT Menu FocusOut(Evas)");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::SetGrabbedObject
 *          set/reset grabbed Evas Object
 *
 * @param[in]   object      grabbed Evas object or NULL
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::SetGrabbedObject(Evas_Object *object)
{
    ICO_DBG("CicoHSMenuTouch::SetGrabbedObject %08x", (int)object);
    grabbed_object = object;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchDownTerm
 *          touch down action at terminate icon
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchDownTerm(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    char *appid = reinterpret_cast<char*>(data);

    ICO_PRF("TOUCH_EVENT Term Down (%d,%d) app=%s",
            info->output.x, info->output.y, appid ? appid : "(NIL)");
    touch_down = true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchUpTerm
 *          touch up action at terminate icon
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchUpTerm(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    char *appid = reinterpret_cast<char*>(data);

    ICO_PRF("TOUCH_EVENT Term Up   (%d,%d) app=%s",
            info->output.x, info->output.y, appid ? appid : "(NIL)");
    if (touch_down == false)    {
        ICO_DBG("CicoHSMenuTouch::TouchUpTerm: No Down");
        // Menu manipulation is normally processed, even if there is no touchdown.
    }
    touch_down = false;

    // play opration sound
    CicoSound::GetInstance()->PlayOperationSound();

    strncpy(terminate_appid, appid, ICO_HS_MAX_PROCESS_NAME);

    menu_window->ShowTerminateButton();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchUpTeminateYes
 *          touch up action at menu
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchUpTerminateYes(void *data, Evas *evas, Evas_Object *obj,
                                     void *event_info)
{
    Evas_Event_Mouse_Down *info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);

    ICO_PRF("TOUCH_EVENT TermYes (%d,%d) app=%s",
            info->output.x, info->output.y, terminate_appid);

    // play opration sound
    CicoSound::GetInstance()->PlayOperationSound();

    menu_window->TerminateApp(terminate_appid);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchUpTerminateNo
 *          touch up action at menu
 *
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchUpTerminateNo(void *data, Evas *evas, Evas_Object *obj,
                                    void *event_info)
{
    Evas_Event_Mouse_Down *info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);

    ICO_PRF("TOUCH_EVENT TermNo (%d,%d) app=%s",
            info->output.x, info->output.y, terminate_appid);

    // play opration sound
    CicoSound::GetInstance()->PlayOperationSound();

    menu_window->TerminateApp(NULL);
}
// vim: set expandtab ts=4 sw=4:
