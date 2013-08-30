/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   control bar window
 *
 * @date    Aug-08-2013
 */
#include "CicoHSControlBarWindow.h"
#include "CicoHSControlBarTouch.h"
#include "CicoHomeScreen.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::CicoHSControlBarWindow
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSControlBarWindow::CicoHSControlBarWindow(void)
{
    evas = NULL;

    CicoHomeScreenResourceConfig::GetImagePath(img_dir_path,
        ICO_HS_MAX_PATH_BUFF_LEN);

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::~CicoHSControlBarWindow
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSControlBarWindow::~CicoHSControlBarWindow(void)
{
    /* not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::CreateControlBarWindow
 *          crate window (control bar)
 *
 * @param[in]   pos_x    position x
 * @param[in]   pos_y    position y
 * @param[in]   width    width
 * @param[in]   height   height
 * @return      OK or ERRROR
 */
/*--------------------------------------------------------------------------*/
int 
CicoHSControlBarWindow::CreateControlBarWindow(int pos_x,int pos_y,int width,int height)
{
    int ret;
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
    
    /*create window*/
    ret = CreateWindow(ICO_HS_CONTROL_BAR_WINDOW_TITLE,pos_x,pos_y,width,height,EINA_TRUE);
    if(ret != ICO_OK){
       return ret;
    }
       
    /* get evas */
    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_DBG("CicoHSControlBarWindow::CreateControlBarWindow: could not get evas.");
        return ICO_ERROR;
    }

    /*control bar back*/
    /* image file name*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_CONTROL_BAR_BASE);
 
    /* set object*/
    canvas = evas_object_image_filled_add(evas);
    int err = evas_object_image_load_error_get(canvas);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_DBG("CicoHSControlBarWindow::CreateControlBarWindow: backgound image is not exist");

        evas_object_del(canvas);
        FreeWindow();
        return ICO_ERROR;
    }
    
    evas_object_image_file_set(canvas, img_path, NULL);
    evas_object_move(canvas, 0, 0);
    evas_object_resize(canvas, width,height);
    evas_object_show(canvas);


    /*home button*/
    /* image file name*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON);

    /* set object*/
    home_button = evas_object_image_filled_add(evas);
    evas_object_image_file_set(home_button, img_path, NULL);
    evas_object_move(home_button, (width/2) - (ICO_HS_CONTROL_BAR_HOME_BUTTON_WIDTH /2),
                      ICO_HS_CONTROL_BAR_HOME_BUTTON_START_POS_Y);
    evas_object_resize(home_button, ICO_HS_CONTROL_BAR_HOME_BUTTON_WIDTH, 
                       ICO_HS_CONTROL_BAR_HOME_BUTTON_HEIGHT);;
    evas_object_event_callback_add(home_button, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSControlBarTouch::TouchDownControlBar,NULL);
    evas_object_event_callback_add(home_button, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSControlBarTouch::TouchUpControlBar,NULL);
    evas_object_show(home_button);    
 
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::FreeControlBarWindow
 *          free window (control bar)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::FreeControlBarWindow(void)
{
    evas_object_del(home_button);
    evas_object_del(canvas);
    FreeWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::TouchHome
 *          touch home button
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSControlBarWindow::TouchHome(void)
{
    CicoHomeScreen::ChangeMode(ICO_HS_SHOW_HIDE_PATTERN_SLIDE);
}
