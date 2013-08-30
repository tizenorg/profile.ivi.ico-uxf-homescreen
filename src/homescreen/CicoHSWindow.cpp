/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Window class
 *
 * @date    Aug-08-2013
 */
#include "CicoHSWindow.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::CicoHSWindow
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSWindow::CicoHSWindow(void)
{
    window = NULL;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::~CicoHSWindow
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSWindow::~CicoHSWindow(void)
{
    /* Do not somthing to do */
    if(window != NULL){
        FreeWindow();
    }
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::CreateWindow
 *          create new window(ecore evas)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int 
CicoHSWindow::CreateWindow(const char *title,int pos_x,int pos_y,int width,int height,int alpha)
{
 
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->width = width;
    this->height = height;

    /* Make a new ecore_evas */
    window = ecore_evas_new(NULL, pos_x, pos_y, width, height, "frame=0");  
    /* if do not creted new, enlightenment return NULL */
    if (!window) {
        EINA_LOG_CRIT("CicoHSWindow::Initialize: could not create new_window.");
        return ICO_ERROR;
    }
    strncpy(this->title,title,ICO_MAX_TITLE_NAME_LEN);
    ecore_evas_title_set(window,this->title);

    /* alpha channel is enable*/
    ecore_evas_alpha_set(window, alpha);

    return ICO_OK;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::FreeWindow
 *          Free the window(ecore evas)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSWindow::FreeWindow(void)
{
    ecore_evas_free(window);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::WindowSetting
 *          setting to window(ecore evas)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindow::WindowSetting(int pos_x,int pos_y,int width,int height,int alpha)
{
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->width = width;
    this->height = height;

    /* move */
    ecore_evas_move(window,pos_x,pos_y);
    /* resize */
    ecore_evas_resize(window, width, height);
    /* alpha channel is enable*/
    ecore_evas_alpha_set(window, alpha);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::ShowWindow
 *          showing window(ecore evas)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindow::ShowWindow(void)
{
    /* showing */
    ecore_evas_show(window);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindow::HideWindow
 *          hiding window(ecore evas)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindow::HideWindow(void)
{
    /* hiding */
    ecore_evas_hide(window);
}


