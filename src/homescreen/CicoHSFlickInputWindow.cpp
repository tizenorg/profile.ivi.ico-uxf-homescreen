/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   flick input window
 *
 * @date    Sep-20-2013
 */
#include "CicoHSFlickInputWindow.h"
#include "CicoHSFlickTouch.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::CicoHSFlickInputWindow
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSFlickInputWindow::CicoHSFlickInputWindow(void)
{
    ICO_DBG("CicoHSFlickInputWindow::CicoHSFlickInputWindow: constractor");
    evas = NULL;
    background = NULL;
    setuped = false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::~CicoHSFlickInputWindow
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSFlickInputWindow::~CicoHSFlickInputWindow(void)
{
    ICO_DBG("CicoHSFlickInputWindow::CicoHSFlickInputWindow: destractor");
    /* not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::CreateFlickInputWindow
 *          crate window (flick input window)
 *
 * @param[in]   pos_x    position x
 * @param[in]   pos_y    position y
 * @param[in]   width    width
 * @param[in]   height   height
 * @param[in]   subname  flick input window sub name
 * @return      OK or ERRROR
 */
/*--------------------------------------------------------------------------*/
int
CicoHSFlickInputWindow::CreateFlickInputWindow(int pos_x, int pos_y,
                                               int width, int height, const char *subname)
{
    int ret;

    ICO_DBG("CreateFlickInputWindow: start(%s,x/y=%d/%d,w/h=%d/%d)",
            subname ? subname : "(null)", pos_x, pos_y, width, height);

    /*create window*/
    if (subname)    {
        snprintf(winname, sizeof(winname), "%s_%s",
                 ICO_HS_FLICK_INPUT_WINDOW_TITLE, subname);
    }
    else    {
        strncpy(winname, ICO_HS_FLICK_INPUT_WINDOW_TITLE, ICO_SYC_MAX_WINNAME_LEN);
    }
    winname[ICO_SYC_MAX_WINNAME_LEN-1] = 0;
    ICO_DBG("CreateFlickInputWindow: winname=%s", winname);
    ret = CreateWindow(winname, pos_x, pos_y, width, height, EINA_TRUE);
    if(ret != ICO_OK){
        ICO_CRI("CicoHSFlickInputWindow::CreateFlickInputWindow: can not create window");
        return ret;
    }

    /* get evas */
    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_CRI("CicoHSFlickInputWindow::CreateFlickInputWindow: could not get evas.");
        return ICO_ERROR;
    }

    /* background object(transparent)    */
    background = evas_object_image_filled_add(evas);

    /* set mouse/touch callback */
    evas_object_event_callback_add(background, EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSFlickTouch::TouchDownFlick, this);
    evas_object_event_callback_add(background, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSFlickTouch::TouchUpFlick, this);

    /* move and show window     */
    evas_object_move(background, 0, 0);
    evas_object_resize(background, width, height);
    evas_object_show(background);

    ICO_DBG("CreateFlickInputWindow: end");
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::FreeFlickInputWindow
 *          free window (flick input window)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickInputWindow::FreeFlickInputWindow(void)
{
    evas_object_del(background);
    FreeWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::SetupFlickWindow
 *          initiale setup flick input window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickInputWindow::SetupFlickWindow(void)
{
    ico_syc_win_move_t move;
    ico_syc_animation_t show;

    if (! setuped)  {
        ICO_DBG("SetupFlickWindow: start");
        setuped = true;
        move.zone = NULL;
        move.pos_x = pos_x;
        move.pos_y = pos_y;
        move.width = width;
        move.height = height;
        memset(&show, 0, sizeof(show));
        show.time = ICO_SYC_WIN_SURF_RAISE;

        /* move flick input window to TouchLayer    */
        ico_syc_change_layer(appid, surface, HS_LAYER_TOUCH);
        /* move window position and size            */
        ico_syc_move(appid, surface, &move, NULL);
        /* show and raise window                    */
        ico_syc_show(appid, surface, &show);

        /* show touch layer                         */
        ico_syc_show_layer(HS_LAYER_TOUCH);

        ICO_DBG("SetupFlickWindow: end");
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::Show
 *          show my window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickInputWindow::Show(void)
{
    ico_syc_show(appid, surface, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::Hide
 *          hide my window
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickInputWindow::Hide(void)
{
    ico_syc_hide(appid, surface, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::isMyWindowName
 *          check window name
 *
 * @param[in]   winname     target window name
 * @return      check result
 * @retval      true        my window name is winname
 * @retval      false       my window name is NOT winname
 */
/*--------------------------------------------------------------------------*/
bool
CicoHSFlickInputWindow::isMyWindowName(const char *winname)
{
    bool    ret;

    ret = (strcmp(winname, this->winname)) ? false : true;
    ICO_DBG("isMyWindowName: %s is %s", winname, ret ? "TRUE" : "FALSE");
    return ret;
}

int
CicoHSFlickInputWindow::GetPosX(void)
{
    return pos_x;
}

int
CicoHSFlickInputWindow::GetPosY(void)
{
    return pos_y;
}

int
CicoHSFlickInputWindow::GetWidth(void)
{
    return width;
}

int
CicoHSFlickInputWindow::GetHeight(void)
{
    return height;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSFlickInputWindow::SetWindowID
 *          set appid and surface
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSFlickInputWindow::SetWindowID(const char *appid, int surface)
{
    ICO_DBG("SetWindowID: appid=%s surface=%08x", appid, surface);
    strncpy(this->appid, appid, ICO_HS_MAX_PROCESS_NAME);
    this->surface = surface;
}
// vim: set expandtab ts=4 sw=4:
