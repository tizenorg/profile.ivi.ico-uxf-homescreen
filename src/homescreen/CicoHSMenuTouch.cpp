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
#include "CicoHSMenuTouch.h"

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
 * @param[in]   data    data
 * @param[in]   evas    evas
 * @param[in]   obj     object
 * @param[in]   event_info    event information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTouch::TouchDownMenu(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *info;

    info = reinterpret_cast<Evas_Event_Mouse_Down*>(event_info);
    touch_state_b_x = info->output.x;
    touch_state_b_y = info->output.y;

    char *appid = reinterpret_cast<char*>(data);
    
    if(appid == NULL){
        return;
    }

    long_act = false;
    timer = ecore_timer_add(ICO_HS_MENU_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS,
                           LongPushed,NULL);    
}

Eina_Bool
CicoHSMenuTouch::LongPushed(void *data)
{
   long_act = true;
   ecore_timer_del(timer);
   timer = NULL;

   /*stop select*/
   menu_window->ChangeTerminateMode();

   return EINA_TRUE;
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
    char *appid = reinterpret_cast<char*>(data);
    int sub = 0;

    if(timer != NULL){
        ecore_timer_del(timer);
        timer = NULL;
    }
    /* long push*/
    if(long_act == true){
        long_act = false;
        return;
    }

    info = reinterpret_cast<Evas_Event_Mouse_Up*>(event_info);
    touch_state_a_x = info->output.x;
    touch_state_a_y = info->output.y;
    sub = touch_state_a_x - touch_state_b_x;

    /* menu slide*/
    if( sub > ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE){
        menu_window->GoBackMenu();
        touch_state_b_x = 0;
        touch_state_b_y = 0;
        return;
    }else if(sub < (-1 * ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE)){
        menu_window->GoNextMenu();
        touch_state_b_x = 0;
        touch_state_b_y = 0;
        return;
    }

    /*execute application*/
    if(appid != NULL){
        menu_window->ExecuteApp(appid);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTouch::TouchUpTerm
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
CicoHSMenuTouch::TouchUpTerm(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up *info;
    int sub = 0;

    char *appid = reinterpret_cast<char*>(data);

    strncpy(terminate_appid,appid,ICO_HS_MAX_PROCESS_NAME);

    menu_window->ShowTerminateButton();
#if 0
    if(appid != NULL){
        menu_window->TerminateApp(appid);
    }
#endif
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
CicoHSMenuTouch::TouchUpTerminateYes(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
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
CicoHSMenuTouch::TouchUpTerminateNo(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    menu_window->TerminateApp(NULL);
}