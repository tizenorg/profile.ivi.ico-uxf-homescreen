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
#include "CicoHSSystemState.h"

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
	background = evas_object_rectangle_add(evas);
    evas_object_color_set(background,128,128,128,255);
    evas_object_move(background, 0, 0);
    evas_object_resize(background, width,height);
    evas_object_show(background);

    /*home button*/
    /* image file name*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY);

    /* set object*/
    menu_btn = evas_object_image_filled_add(evas);
    evas_object_image_file_set(menu_btn, img_path, NULL);
    evas_object_move(menu_btn, (width/2) - (ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH /2),
                      ICO_HS_CONTROL_BAR_MENU_BTN_START_POS_Y);
    evas_object_resize(menu_btn, ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH, 
                       ICO_HS_CONTROL_BAR_MENU_BTN_HEIGHT);;
    evas_object_event_callback_add(menu_btn, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSControlBarTouch::TouchDownControlBar,NULL);
    evas_object_event_callback_add(menu_btn, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSControlBarTouch::TouchUpControlBar,NULL);
    evas_object_show(menu_btn);    
 
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
    evas_object_del(background);
    evas_object_del(menu_btn);
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::SetNightMode
 *          set night mode color theme chagne
 *
 * @param   none
 * @return  none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::SetNightMode(void)
{
    ICO_DBG("CicoHSControlBarWindow::SetNightMode Enter");

    bool state = CicoHSSystemState::getInstance()->getNightMode();

    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
    if (true == state) {
        ICO_DBG("ICO_SYC_STATE_ON");
        evas_object_color_set(background,0,0,0,255);
        evas_object_show(background);    
        if (true == CicoHSSystemState::getInstance()->getRegulation()) {
            ICO_DBG("Regulation=ON");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT2);
        }
        else {
            ICO_DBG("Regulation=OFF");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT);
        }
        evas_object_image_file_set(menu_btn, img_path, NULL);
        evas_object_show(menu_btn);    
    }
    else {
        ICO_DBG("ICO_SYC_STATE_OFF");
        evas_object_color_set(background,128,128,128,255);
        evas_object_show(background);    
        if (true == CicoHSSystemState::getInstance()->getRegulation()) {
            ICO_DBG("Regulation=ON");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY2);
        }
        else {
            ICO_DBG("Regulation=OFF");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY);
        }
        evas_object_image_file_set(menu_btn, img_path, NULL);
        evas_object_show(menu_btn);    
    }
    evas_render(evas);
    ico_syc_hide(appid, surface, NULL);
    ico_syc_show(appid, surface, NULL);

    ICO_DBG("CicoHSControlBarWindow::SetNightMode Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::SetRegulation
 *          set regulation color theme chagne
 *
 * @param   none
 * @return  none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::SetRegulation(void)
{
    ICO_DBG("CicoHSControlBarWindow::SetRegulation Enter");

    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
    if (true == CicoHSSystemState::getInstance()->getNightMode()) {
        ICO_DBG("NightMode=ON");
        if (true == CicoHSSystemState::getInstance()->getRegulation()) {
            ICO_DBG("Regulation=ON");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT2);
        }
        else {
            ICO_DBG("Regulation=OFF");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT);
        }
    }
    else {
        ICO_DBG("NightMode=OFF");
        if (true == CicoHSSystemState::getInstance()->getRegulation()) {
            ICO_DBG("Regulation=ON");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY2);
        }
        else {
            ICO_DBG("Regulation=OFF");
            snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
                     ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY);
        }
    }
    evas_object_image_file_set(menu_btn, img_path, NULL);
    evas_object_show(menu_btn);    
    evas_render(evas);
    ico_syc_hide(appid, surface, NULL);
    ico_syc_show(appid, surface, NULL);

    ICO_DBG("CicoHSControlBarWindow::SetRegulation Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::SetMenuWindowID
 *          set appid and surface
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::SetWindowID(const char *appid,int surface)
{
    strncpy(this->appid,appid,ICO_HS_MAX_PROCESS_NAME);
    this->surface = surface;
}
// vim: set expandtab ts=4 sw=4:
