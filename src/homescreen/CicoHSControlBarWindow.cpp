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
#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenResourceConfig.h"
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

    CicoHomeScreenConfig config;
    config.Initialize(ICO_HOMESCREEN_CONFIG_FILE);
    const char *value = config.ConfigGetString("switchzone", "keyname", NULL);
    changeZoneKeyName = value[0];

    char tmp_str[16];
    for (unsigned int ii = 0; ii < ICO_HS_CONTROL_BAR_SHORTCUT_MAX_NUM; ii++) {
        sprintf(tmp_str, ICO_HS_CONTROL_BAR_CONFIG_SHORTCUT_APP"%d", ii);
        value = config.ConfigGetString(ICO_HS_CONFIG_CONTROL_BAR, tmp_str, NULL);
        if ((NULL != value) && (0 != strcmp(value, "none"))) {
            shortcut_appid[ii] = value;
            ICO_DBG("shortcut_appid[%d] = [%s]", ii, shortcut_appid[ii]);
        }
        else {
            shortcut_appid[ii] = NULL;
            ICO_DBG("shortcut_appid[%d] = NULL", ii);
        }
    }
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
CicoHSControlBarWindow::CreateControlBarWindow(int pos_x, int pos_y,
                                               int width, int height)
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
        ICO_ERR("ecore_evas_get failed.");
        ICO_DBG("CicoHSControlBarWindow::CreateControlBarWindow Leave(ERROR)");
        return ICO_ERROR;
    }

    // create background evas object
    background = evas_object_rectangle_add(evas);

    // add callback functions
    evas_object_event_callback_add(background, EVAS_CALLBACK_KEY_DOWN, 
                                   CicoHSControlBarWindow::evasKeyDownCB, this);

    evas_object_focus_set(background, EINA_FALSE);     
    Eina_Bool eret = evas_object_key_grab(background, "m", 0, 0, EINA_TRUE);
    if (EINA_FALSE == eret) {
        ICO_WRN("evas_object_key_grab failed.");
    }

    // set background coloer
    evas_object_color_set(background,128,128,128,255);

    // change window geometry
    evas_object_move(background, 0, 0);
    evas_object_resize(background, width,height);

    // show window
    evas_object_show(background);

    // home button
    // image file name
    snprintf(img_path, sizeof(img_path), "%s%s",
             img_dir_path, ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY);

    // set object
    menu_btn = evas_object_image_filled_add(evas);

    // preload image
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
             ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY2);
    evas_object_image_file_set(menu_btn, img_path, NULL);

    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
             ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT);
    evas_object_image_file_set(menu_btn, img_path, NULL);

    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
             ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT2);
    evas_object_image_file_set(menu_btn, img_path, NULL);

    // load fisrt show icon image 
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,
             ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY);
    evas_object_image_file_set(menu_btn, img_path, NULL);

    evas_object_move(menu_btn,
                     (width/2) - (ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH /2),
                     ICO_HS_CONTROL_BAR_MENU_BTN_START_POS_Y);
    evas_object_resize(menu_btn, ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH, 
                       ICO_HS_CONTROL_BAR_MENU_BTN_HEIGHT);
    evas_object_event_callback_add(menu_btn, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSControlBarTouch::TouchDownControlBar,
                                   NULL);
    evas_object_event_callback_add(menu_btn, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSControlBarTouch::TouchUpControlBar,
                                   NULL);
    evas_object_show(menu_btn);    

    /* shortcut */
    AddShortcut(evas, width);
 
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::AddShortcut
 *          shortcut addition (control bar)
 *
 * @param[in]   evas     Evas Object
 * @param[in]   width    width
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::AddShortcut(Evas *evas, int width)
{
    ICO_DBG("CicoHSControlBarWindow::AddShortcut Enter");

    int escPosX;
    int x;
    int tmp_space;
    int s_cnt = 0;
    const char *tmp_appid;
    CicoSCLifeCycleController *life_cycle_controller;

    /* menu button x position */
    escPosX = (width / 2) - (ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH / 2);

    life_cycle_controller = CicoSCLifeCycleController::getInstance();

    tmp_space = (width - (ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH
                * ICO_HS_CONTROL_BAR_BTN_MAX_NUM))
                / (ICO_HS_CONTROL_BAR_BTN_MAX_NUM + 1);
    ICO_DBG("CicoHSControlBarWindow::AddShortcut tmp_space = [%d]",
            tmp_space);

    for (unsigned ii = 0; ii < ICO_HS_CONTROL_BAR_BTN_MAX_NUM; ii++) {
        x = (ICO_HS_CONTROL_BAR_SHORTCUT_BTN_WIDTH * ii) +
            (tmp_space * (ii + 1));

        if ((x <= escPosX) &&
           ((x + ICO_HS_CONTROL_BAR_SHORTCUT_BTN_WIDTH) >= escPosX)) {
            /* The position of a menu button is skipped */
            continue;
        }
        ICO_DBG("CicoHSControlBarWindow::AddShortcut x = [%d]", x);

        tmp_appid = (const char *)(shortcut_appid[s_cnt]);
        if (tmp_appid == NULL) {
            /* No shortcut appid */
            s_cnt++;
            continue;
        }

        /* get APP information */
        std::vector<CicoSCAilItems> aillist =
        life_cycle_controller->getAilList();

        /* add shortcut object */
        for (unsigned int kk = 0; kk < aillist.size(); kk++ ) {
            if (strncmp(aillist[kk].m_appid.c_str(), tmp_appid,
                ICO_HS_MAX_PROCESS_NAME) == 0) {
                ICO_DBG("CicoHSControlBarWindow::AddShortcut tmp_appid = [%s]",
                        tmp_appid);
                shortcut[s_cnt] = evas_object_image_filled_add(evas);
                evas_object_image_file_set(shortcut[s_cnt],
                                  aillist[kk].m_icon.c_str(), NULL);
                evas_object_move(shortcut[s_cnt], x,
                                  ICO_HS_CONTROL_BAR_SHORTCUT_BTN_START_POS_Y);
                evas_object_resize(shortcut[s_cnt],
                                  ICO_HS_CONTROL_BAR_SHORTCUT_BTN_WIDTH,
                                  ICO_HS_CONTROL_BAR_SHORTCUT_BTN_HEIGHT);
                evas_object_event_callback_add(shortcut[s_cnt],
                                  EVAS_CALLBACK_MOUSE_UP,
                                  CicoHSControlBarTouch::TouchUpControlBar,
                                  tmp_appid);
                evas_object_show(shortcut[s_cnt]);
            }
        }
        s_cnt++;
    }

    ICO_DBG("CicoHSControlBarWindow::AddShortcut Leave");
    return;
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
    }
    else {
        ICO_DBG("ICO_SYC_STATE_OFF");
        evas_object_color_set(background,128,128,128,255);
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
    }

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

    ICO_DBG("CicoHSControlBarWindow::SetRegulation Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::GetChangeZoneKeyName
 *          get change zone key name
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
char
CicoHSControlBarWindow::GetChangeZoneKeyName(void)
{
    return changeZoneKeyName;
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

//--------------------------------------------------------------------------
/**
 *  @brief  key up event callback function
 *
 *  @pamam [in] data    user data
 *  @param [in] evas    evas instcance
 *  @param [in] obj     evas object instcance
 *  @param [in] info    event information(Evas_Event_Key_Down)
 */
//--------------------------------------------------------------------------
void
CicoHSControlBarWindow::evasKeyDownCB(void *data, Evas *evas,
                                      Evas_Object *obj, void *info)
{
    Evas_Event_Key_Down *evinfo = NULL;
    evinfo = (Evas_Event_Key_Down*)info;
    CicoHSControlBarWindow *ctrlbarwin = (CicoHSControlBarWindow*)(data);

    ICO_DBG("KeyDownCB: keyname=%s, key=%d",
            evinfo->keyname, (char)*evinfo->key);

    if (evinfo->keyname[0] == ctrlbarwin->GetChangeZoneKeyName()) {
        CicoHomeScreen::ChangeZone();
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSControlBarWindow::TouchShortcut
 *          touch shortcut button
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSControlBarWindow::TouchShortcut(const char *appid)
{
    ICO_DBG("CicoHSControlBarWindow::TouchShortcut Enter");

    if (appid != NULL) {
        ICO_DBG("CicoHSControlBarWindow::TouchShortcut appid = [%s]", appid);
        CicoHomeScreen::ExecuteApp(appid);
    }

    ICO_DBG("CicoHSControlBarWindow::TouchShortcut Leave");
}

// vim: set expandtab ts=4 sw=4:
