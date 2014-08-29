/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   swipe input window
 *
 * @date    Sep-20-2013
 */
#include "CicoHSSwipeInputWindow.h"
#include "CicoHSSwipeTouch.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::CicoHSSwipeInputWindow
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSSwipeInputWindow::CicoHSSwipeInputWindow(void)
{
    ICO_DBG("CicoHSSwipeInputWindow::CicoHSSwipeInputWindow: constractor");
    evas = NULL;
    background = NULL;
    setuped = false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::~CicoHSSwipeInputWindow
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSSwipeInputWindow::~CicoHSSwipeInputWindow(void)
{
    ICO_DBG("CicoHSSwipeInputWindow::CicoHSSwipeInputWindow: destractor");
    /* not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::CreateSwipeInputWindow
 *          crate window (swipe input window)
 *
 * @param[in]   pos_x    position x
 * @param[in]   pos_y    position y
 * @param[in]   width    width
 * @param[in]   height   height
 * @param[in]   subname  swipe input window sub name
 * @return      OK or ERRROR
 */
/*--------------------------------------------------------------------------*/
int
CicoHSSwipeInputWindow::CreateSwipeInputWindow(int pos_x, int pos_y,
                                               int width, int height, const char *subname)
{
    int ret;

    ICO_DBG("CreateSwipeInputWindow: start(%s,x/y=%d/%d,w/h=%d/%d)",
            subname ? subname : "(null)", pos_x, pos_y, width, height);

    /*create window*/
    if (subname)    {
        snprintf(winname, sizeof(winname), "%s_%s",
                 ICO_HS_SWIPE_INPUT_WINDOW_TITLE, subname);
    }
    else    {
        strncpy(winname, ICO_HS_SWIPE_INPUT_WINDOW_TITLE, ICO_SYC_MAX_WINNAME_LEN);
    }
    winname[ICO_SYC_MAX_WINNAME_LEN-1] = 0;
    ICO_DBG("CreateSwipeInputWindow: winname=%s", winname);
    ret = CreateWindow(winname, pos_x, pos_y, width, height, EINA_TRUE);
    if(ret != ICO_OK){
        ICO_CRI("CicoHSSwipeInputWindow::CreateSwipeInputWindow: can not create window");
        return ret;
    }

    /* get evas */
    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_CRI("CicoHSSwipeInputWindow::CreateSwipeInputWindow: could not get evas.");
        return ICO_ERROR;
    }

    /* background object(transparent)    */
    background = evas_object_image_filled_add(evas);

    /* set mouse/touch callback */
    evas_object_event_callback_add(background, EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSSwipeTouch::TouchDownSwipe, this);
    evas_object_event_callback_add(background, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSSwipeTouch::TouchUpSwipe, this);
    evas_object_event_callback_add(background, EVAS_CALLBACK_MOUSE_MOVE,
                                       CicoHSSwipeTouch::TouchMoveSwipe, this);
    /* move and show window     */
    evas_object_move(background, 0, 0);
    evas_object_resize(background, width, height);
    evas_object_show(background);

    ICO_DBG("CreateSwipeInputWindow: end");
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::FreeSwipeInputWindow
 *          free window (swipe input window)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeInputWindow::FreeSwipeInputWindow(void)
{
    evas_object_del(background);
    FreeWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::SetupSwipeWindow
 *          initiale setup swipe input window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeInputWindow::SetupSwipeWindow(void)
{
    ico_syc_win_move_t move;
    ico_syc_animation_t show;

    if (! setuped)  {
        ICO_DBG("SetupSwipeWindow: start");
        setuped = true;
        move.zone = NULL;
        move.pos_x = pos_x;
        move.pos_y = pos_y;
        move.width = width;
        move.height = height;
        memset(&show, 0, sizeof(show));
        show.name = (char *)"none";

        /* move swipe input window to TouchLayer    */
        ico_syc_change_layer(appid, surface, HS_LAYER_TOUCHPANEL);
        /* move window position and size            */
        ico_syc_move(appid, surface, &move, NULL);
        /* show and raise window                    */
        show.time = ICO_SYC_WIN_SURF_RAISE;
        ico_syc_show(appid, surface, &show);

        /* show touch layer                         */
        ico_syc_show_layer(HS_LAYER_TOUCHPANEL);

        ICO_DBG("SetupSwipeWindow: end");
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::Show
 *          show my window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeInputWindow::Show(void)
{
    ICO_DBG("CicoHSSwipeInputWindow::Show: %08x.%s", surface, this->winname);
    ico_syc_show(appid, surface, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::Hide
 *          hide my window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeInputWindow::Hide(void)
{
    ICO_DBG("CicoHSSwipeInputWindow::Hide: %08x.%s", surface, this->winname);
    ico_syc_hide(appid, surface, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::isMyWindowName
 *          check window name
 *
 * @param[in]   winname     target window name
 * @return      check result
 * @retval      true        my window name is winname
 * @retval      false       my window name is NOT winname
 */
/*--------------------------------------------------------------------------*/
bool
CicoHSSwipeInputWindow::isMyWindowName(const char *winname)
{
    bool    ret;

    ret = (strcmp(winname, this->winname)) ? false : true;
    ICO_DBG("isMyWindowName: %s is %s", winname, ret ? "TRUE" : "FALSE");
    return ret;
}

int
CicoHSSwipeInputWindow::GetPosX(void)
{
    return pos_x;
}

int
CicoHSSwipeInputWindow::GetPosY(void)
{
    return pos_y;
}

int
CicoHSSwipeInputWindow::GetWidth(void)
{
    return width;
}

int
CicoHSSwipeInputWindow::GetHeight(void)
{
    return height;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSSwipeInputWindow::SetWindowID
 *          set appid and surface
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSSwipeInputWindow::SetWindowID(const char *appid, int surface)
{
    ICO_DBG("SetWindowID: appid=%s surface=%08x", appid, surface);
    strncpy(this->appid, appid, ICO_HS_MAX_PROCESS_NAME);
    this->surface = surface;
}
// vim: set expandtab ts=4 sw=4:
