/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Home Screen
 *
 * @date    Aug-08-2013
 */
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHomeScreen * CicoHomeScreen::hs_instance;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CicoHomeScreen
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHomeScreen::CicoHomeScreen(void)
{
    application_num = 0;
    for(int ii = 0;ii < ICO_HS_MAX_APP_NUM;ii++){
        apps_info[ii] = NULL;
    }

    hs_app_info = NULL;
    sb_app_info = NULL;
    os_app_info = NULL;
 
    full_width = 1;
    full_height = 1;
    menu_width = 1;
    menu_height = 1;
    bg_width = 1;
    bg_height = 1;
    ctl_bar_width = 1;
    ctl_bar_height = 1;

    menu_window = NULL;
    back_window = NULL;
    ctl_bar_window = NULL;

    //mode = ICO_HS_MODE_MENU;
    mode = ICO_HS_MODE_APPLICATION;
    config = NULL;
    hs_instance = NULL;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::~CicoHomeScreen
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHomeScreen::~CicoHomeScreen(void)
{
    /* Do not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowHomeScreenLayer
 *          Show Home screen layer
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowHomeScreenLayer(void)
{
    /*show home screen layers*/
    ico_syc_show_layer(HS_LAYER_APPLICATION);
    ico_syc_show_layer(HS_LAYER_HOMESCREEN);
    ico_syc_show_layer(HS_LAYER_TOUCH);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetHsPackageName
 *          get HomeScreen package name
 *
 * @param[in]   none
 * @return      package name
 */
/*--------------------------------------------------------------------------*/
char *
CicoHomeScreen::GetHsPackageName(void)
{
    return hs_package_name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetHsPackageName
 *          get StatusBar package name
 *
 * @param[in]   none
 * @return      package name
 */
/*--------------------------------------------------------------------------*/
char *
CicoHomeScreen::GetSbPackageName(void)
{
    return sb_package_name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetHsPackageName
 *          get OnScreen package name
 *
 * @param[in]   none
 * @return      package name
 */
/*--------------------------------------------------------------------------*/
char *
CicoHomeScreen::GetOsPackageName(void)
{
    return os_package_name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateAppInfoList
 *          create application info list
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::CreateAppInfoList(void)
{
    /* get APP information*/
    std::vector<CicoSCAilItems> aillist = life_cycle_controller->getAilList();

    for(int ii = 0; ii < ICO_HS_MAX_APP_NUM; ii++) {
        if(apps_info[ii] != NULL){
            delete apps_info[ii];
        }
    }

    /*create instance*/
    application_num = 0;
    for(unsigned int ii = 0; 
        (ii < aillist.size()) && (ii < ICO_HS_MAX_APP_NUM);
        ii++) {
        apps_info[ii] = new CicoHSAppInfo(aillist[ii].m_appid.c_str());
        application_num++;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetAppInfo
 *          get application info
 *
 * @param[in]   appid    application id
 * @return      Application info
 */
/*--------------------------------------------------------------------------*/
CicoHSAppInfo *
CicoHomeScreen::GetAppInfo(const char *appid)
{
    for(int ii = 0;ii < application_num;ii++){
        if(strcmp(apps_info[ii]->GetAppId(),appid) == 0){
            return apps_info[ii];
        }
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetAppStatus
 *          get application status
 *
 * @param[in]   appid    application id
 * @return      Application info
 */
/*--------------------------------------------------------------------------*/
bool
CicoHomeScreen::GetAppStatus(const char *appid)
{
    for(int ii = 0;ii < hs_instance->application_num;ii++){
        if(strcmp(hs_instance->apps_info[ii]->GetAppId(),appid) == 0){
            return hs_instance->apps_info[ii]->GetStatus();
        }
    }
    return false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetWindowInfo
 *          get window info 
 *
 * @param[in]   appinfo    application info
 * @param[in]   window    window name
 * @return      Window info
 */
/*--------------------------------------------------------------------------*/
ico_hs_window_info *
CicoHomeScreen::GetWindowInfo(CicoHSAppInfo *appinfo,const char *window)
{
    return appinfo->GetWindowInfo(window);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::UpdateTile
 *          update tile image
 *
 * @param[in]   appid    application id
 * @return      Window info
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::UpdateTile(const char *appid)
{
    CicoHSAppInfo *appinfo = GetAppInfo(appid);
    if(appinfo == NULL){
        return;
    }
    if(appinfo->GetStatus() == false){
        //show icon
        menu_window->ValidMenuIcon(appid);
        ICO_DBG("CicoHomeScreen::UpdateTile: show menuIcon %s",appid);
    }else{
        //show thumbnail
        menu_window->ValidThumbnail(appid,appinfo->GetLastSurface());
        ICO_DBG("CicoHomeScreen::ExecuteApp_i: raise app %s",appid);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ExecuteApp_i
 *          execute and showing application
 *
 * @param[in]   appid    application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ExecuteApp_i(const char *appid)
{
    CicoHSAppInfo *appinfo = GetAppInfo(appid);
    if(appinfo == NULL){
        return;
    }

    if(appinfo->GetStatus() == false){
        //execute
        appinfo->Execute();
        ICO_DBG("CicoHomeScreen::ExecuteApp_i: execute app %s",appid);
    }else{
        //raise
        RaiseApplicationWindow(appinfo->GetAppId(),appinfo->GetLastSurface());
        ICO_DBG("CicoHomeScreen::ExecuteApp_i: raise app %s",appid);
    }
    /* hide HomeScreen layer                        */
    if(GetMode() == ICO_HS_MODE_MENU){
        ChangeMode(ICO_HS_SHOW_HIDE_PATTERN_FADE);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::TerminateApp_i
 *          teminate application
 *
 * @param[in]   appid    application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::TerminateApp_i(const char *appid)
{
    if(appid != NULL){
        CicoHSAppInfo *appinfo = GetAppInfo(appid);
        if(appinfo == NULL){
            return;
        }

        //terminate
        appinfo->Terminate();
    }
    /* hide HomeScreen layer                        */
    if(GetMode() == ICO_HS_MODE_MENU){
        ChangeMode(ICO_HS_SHOW_HIDE_PATTERN_FADE);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowHomeScreenWindow
 *          show window (home screen)
 *
 * @param[in]   win attr    window attribute
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowHomeScreenWindow(ico_syc_win_attr_t *win_attr)
{
    ico_syc_win_move_t move;
    int layer;
    move.zone = NULL;
    move.pos_x = 0;
    move.width = full_width;
  
    if(strncmp(win_attr->name,ICO_HS_BACK_WINDOW_TITLE,
               ICO_MAX_TITLE_NAME_LEN) == 0){
        move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
        move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                      ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_BACKGROUND;
    }else if(strncmp(win_attr->name,ICO_HS_CONTROL_BAR_WINDOW_TITLE,
             ICO_MAX_TITLE_NAME_LEN) == 0){
        move.pos_y = full_height - ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        move.height = ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_TOUCH;
    }else if(strncmp(win_attr->name,ICO_HS_MENU_WINDOW_TITLE,
             ICO_MAX_TITLE_NAME_LEN) == 0){
        move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
        move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT - 
                      ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_HOMESCREEN;
    }else{
        /*do nothing*/
        return;
    }

    ico_syc_change_layer(win_attr->appid,win_attr->surface,layer);
    ICO_DBG("CicoHomeScreen::ShowHomeScreenWindow: id(%s) name(%s) surface(%d) pos(%d,%d) size(%d,%d)",win_attr->appid,
            win_attr->name,win_attr->surface,move.pos_x,move.pos_y,
            move.width,move.height);
    ico_syc_move(win_attr->appid,win_attr->surface,&move,NULL);
    
    /*first time menu is unvisible*/
    if((strncmp(win_attr->name,ICO_HS_MENU_WINDOW_TITLE,
             ICO_MAX_TITLE_NAME_LEN) == 0) && 
       (GetMode() == ICO_HS_MODE_APPLICATION)){
        return;
    }

    ico_syc_show(win_attr->appid, win_attr->surface,NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowStatusBarWindow
 *          show window (home screen)
 *
 * @param[in]   win attr    window attribute
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowStatusBarWindow(ico_syc_win_attr_t *win_attr)
{
    ico_syc_win_move_t move;
    int layer;
    move.zone = NULL;
    move.pos_x = 0;
    move.pos_y = 0;
    move.width = full_width;
    move.height = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
    layer = HS_LAYER_HOMESCREEN;

    ico_syc_change_layer(win_attr->appid,win_attr->surface,layer);
    ICO_DBG("CicoHomeScreen::ShowStatusBarWindow: id(%s) name(%s) surface(%d) pos(%d,%d) size(%d,%d)",win_attr->appid,
            win_attr->name,win_attr->surface,move.pos_x,move.pos_y,
            move.width,move.height);
    ico_syc_move(win_attr->appid,win_attr->surface,&move,NULL);
    ico_syc_show(win_attr->appid, win_attr->surface,NULL);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowApplicationWindow
 *          show window (application)
 *
 * @param[in]   win attr    window attribute
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowApplicationWindow(ico_syc_win_attr_t *win_attr)
{

    ico_syc_win_move_t move;
    int layer;
    move.zone = 0;
    move.pos_x = 0;
    move.width = full_width;
    move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
    move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                  ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
    layer = HS_LAYER_APPLICATION;
    
    ico_syc_change_layer(win_attr->appid,win_attr->surface,layer);
    ico_syc_move(win_attr->appid,win_attr->surface,&move,NULL);
    ico_syc_show(win_attr->appid, win_attr->surface,NULL);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::RaiseApplicationWindow
 *          raise window (application)
 *
 * @param[in]   appid      application id
 * @param[in]   surface    surface
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::RaiseApplicationWindow(const char *appid,int surface)
{
    ico_syc_show(appid, surface,NULL);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::EventCallBack
 *          callback for system controller
 *
 * @param[in]   event      kind of event
 * @param[in]   detail     detail 
 * @param[in]   user_data  user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::EventCallBack(const ico_syc_ev_e event,
                              const void *detail,void *user_data)
{
    ICO_DBG("CicoHomeScreen::EventCallBack: start (event %d)",event);
    if(event == ICO_SYC_EV_WIN_CREATE){
        ico_syc_win_info_t *win_info = 
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("CicoHomeScreen::EventCallBack : win create %s %i",
                win_info->appid,win_info->surface); 
        /*only save information*/
        if(strncmp(win_info->appid, hs_instance->GetHsPackageName(),
                   ICO_HS_MAX_PROCESS_NAME) == 0){
           /*Home Screen*/
           hs_instance->hs_app_info->AddWindowInfo(win_info);
        }else if(strncmp(win_info->appid, 
                         hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            /*Status Bar*/
            hs_instance->sb_app_info->AddWindowInfo(win_info);
        }else if(strncmp(win_info->appid, 
                         hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            /*On Screen*/
            hs_instance->os_app_info->AddWindowInfo(win_info);
        }else{
            /*Application*/
            CicoHSAppInfo *appinfo = 
                       hs_instance->GetAppInfo(win_info->appid);
            if(appinfo == NULL){
                return;
            }
            appinfo->AddWindowInfo(win_info);
        }
    }
    else if(event == ICO_SYC_EV_WIN_DESTROY){
        ico_syc_win_info_t *win_info = 
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("CicoHomeScreen::EventCallBack : win delete %s %i",
                win_info->appid,win_info->surface); 
        /*only save information*/
        if(strncmp(win_info->appid, hs_instance->GetHsPackageName(),
                   ICO_HS_MAX_PROCESS_NAME) == 0){
            hs_instance->hs_app_info->FreeWindowInfo(win_info->name);
        }else if(strncmp(win_info->appid, 
                         hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            hs_instance->sb_app_info->FreeWindowInfo(win_info->name);
        }else if(strncmp(win_info->appid, 
                         hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            hs_instance->os_app_info->FreeWindowInfo(win_info->name);
        }else{
            /*Application*/
            CicoHSAppInfo *appinfo = 
                       hs_instance->GetAppInfo(win_info->appid);
            if(appinfo == NULL){
                return;
            }
            hs_instance->UpdateTile(win_info->appid);
            appinfo->FreeWindowInfo(win_info->name);
        }
    }
    else if(event == ICO_SYC_EV_WIN_ACTIVE){
        ico_syc_win_info_t *win_info = 
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        /*if application*/ 
        CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(win_info->appid);
        if(appinfo == NULL){
            return;
        }
        appinfo->SetLastSurface(win_info->surface);
    }
    else if(event == ICO_SYC_EV_WIN_ATTR_CHANGE){
        ico_syc_win_attr_t *win_attr = 
            reinterpret_cast<ico_syc_win_attr_t*>(const_cast<void*>(detail));
        ICO_DBG("CicoHomeScreen::EventCallBack : win attr %s %s %d",
                win_attr->appid,win_attr->name,win_attr->surface); 
        if(strncmp(win_attr->appid, hs_instance->GetHsPackageName(),
                   ICO_HS_MAX_PROCESS_NAME) == 0){
            /*Home Screen*/
            hs_instance->hs_app_info->AddWindowAttr(win_attr);
            /*when Menu window*/
            if(strncmp(win_attr->name,ICO_HS_MENU_WINDOW_TITLE,
                       ICO_MAX_TITLE_NAME_LEN) == 0){
                hs_instance->menu_window->SetMenuWindowID(win_attr->appid,
                    win_attr->surface);
            }
            else if(strncmp(win_attr->name,ICO_HS_CONTROL_BAR_WINDOW_TITLE,
                            ICO_MAX_TITLE_NAME_LEN) == 0){
                hs_instance->ctl_bar_window->SetWindowID(win_attr->appid,
                                                         win_attr->surface);
            }
            ico_hs_window_info * l_win_info = 
                     hs_instance->GetWindowInfo(hs_instance->hs_app_info,
                                                win_attr->name);
            if(l_win_info == NULL){
                return;
            }
            if(hs_instance->hs_app_info->GetShowed(l_win_info) == true){
                return; 
            }
            /*show window*/
            hs_instance->ShowHomeScreenWindow(win_attr);
            hs_instance->hs_app_info->SetShowed(l_win_info,true);
        }else if(strncmp(win_attr->appid, hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            /*Status Bar*/
            hs_instance->sb_app_info->AddWindowAttr(win_attr);
            ico_hs_window_info * l_win_info = 
                     hs_instance->GetWindowInfo(hs_instance->sb_app_info,
                                                win_attr->name);
            if(l_win_info == NULL){
                return;
            }
            if(hs_instance->sb_app_info->GetShowed(l_win_info) == true){
                return; 
            }
            hs_instance->ShowStatusBarWindow(win_attr);
            hs_instance->sb_app_info->SetShowed(l_win_info,true);
        }else if(strncmp(win_attr->appid, hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0){
            /*On Screen*/
            hs_instance->os_app_info->AddWindowAttr(win_attr);
        }else{
            ICO_DBG("CicoHomeScreen::EventCallBack : application window %s %s",
                win_attr->appid,win_attr->name); 
            /*Application*/
            CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(win_attr->appid);
            if(appinfo == NULL){
                return;
            }
            ico_hs_window_info * l_win_info = 
                      hs_instance->GetWindowInfo(appinfo,
                                                win_attr->name);
            if(l_win_info == NULL){
                return;
            }
            if(appinfo->GetShowed(l_win_info) == true){
                return; 
            }
            
            appinfo->AddWindowAttr(win_attr);
            appinfo->SetLastSurface(win_attr->surface);
            hs_instance->ShowApplicationWindow(win_attr);
            ico_syc_change_active(win_attr->appid,win_attr->surface);
            hs_instance->UpdateTile(win_attr->appid);
            hs_instance->hs_app_info->SetShowed(l_win_info,true);
        }
    }
    else if(event == ICO_SYC_EV_THUMB_CHANGE){
        ico_syc_thumb_info_t *thumb_info = 
            reinterpret_cast<ico_syc_thumb_info_t*>(const_cast<void*>(detail));
        CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(thumb_info->appid);
        ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_CHANGE %s",
                thumb_info->appid);
        if(appinfo == NULL){
            return;
        }
        //show icon
        hs_instance->menu_window->SetThumbnail(thumb_info->appid,thumb_info->surface);
    }
    else if(event == ICO_SYC_EV_LAYER_ATTR_CHANGE){
   
    }
    else if(event == ICO_SYC_EV_USERLIST){
   
    }
    else if(event == ICO_SYC_EV_AUTH_FAIL){
   
    }
    else if(event == ICO_SYC_EV_RES_ACQUIRE){
   
    }
    else if(event == ICO_SYC_EV_RES_DEPRIVE){
   
    }
    else if(event == ICO_SYC_EV_RES_WAITING){
   
    }
    else if(event == ICO_SYC_EV_RES_REVERT){
   
    }
    else if(event == ICO_SYC_EV_RES_RELEASE){
   
    }
    else if(event == ICO_SYC_EV_INPUT_SET){
   
    }
    else if(event == ICO_SYC_EV_INPUT_UNSET){
   
    }
    else if(event == ICO_SYC_EV_STATE_CHANGE) {
        ico_syc_state_info_t *state_info = 
            reinterpret_cast<ico_syc_state_info_t*>(const_cast<void*>(detail));

        ICO_DBG("RECV: ICO_SYC_EV_STATE_CHANGE(id=%d state=%d)",
                state_info->id, state_info->state);
        if (ICO_SYC_STATE_REGULATION == state_info->id) {
            // set regulation state
            CicoHSSystemState::getInstance()->setRegulation(
                (state_info->state == ICO_SYC_STATE_ON) ? true : false);
            // regulation action
            ico_syc_animation_t animation;
            animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_SLIDE;
            animation.time = ICO_HS_MENU_ANIMATION_DURATION;
            hs_instance->menu_window->Hide(&animation);
            hs_instance->ctl_bar_window->SetRegulation();
        }
        else if (ICO_SYC_STATE_NIGHTMODE == state_info->id) {
            // set night mode state
            CicoHSSystemState::getInstance()->setNightMode(
                (state_info->state == ICO_SYC_STATE_ON) ? true : false);
            // night mode action
            hs_instance->ctl_bar_window->SetNightMode();
            hs_instance->menu_window->SetNightMode();
        }
    }

    ICO_DBG("CicoHomeScreen::EventCallBack: end");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::StartRelations
 *          Execute Onscreen and Statusbar
 *
 * @param[in]   none
 * @return      OK
 */
/*--------------------------------------------------------------------------*/
int 
CicoHomeScreen::StartRelations(void)
{
    int ret; 
    strncpy(sb_package_name,
            (char *)config->ConfigGetString(ICO_HS_CONFIG_HOMESCREEN,
                                             ICO_HS_CONFIG_SB,
                                             ICO_HS_APPID_DEFAULT_SB),
            ICO_HS_MAX_PROCESS_NAME);
    strncpy(os_package_name,
             (char *)config->ConfigGetString(ICO_HS_CONFIG_HOMESCREEN,
                                             ICO_HS_CONFIG_ONS,
                                             ICO_HS_APPID_DEFAULT_ONS),
            ICO_HS_MAX_PROCESS_NAME);
 
    /* start onscreen & statusbar apps */
#if 0
    os_app_info = new CicoHSAppInfo(os_package_name);
    ret = os_app_info->Execute();
    if (ret < 0) {
        ICO_WRN("execute failed(%s) err=%d", os_package_name, ret);
    } 
#endif

    sb_app_info = new CicoHSAppInfo(sb_package_name); 
    ret = sb_app_info->Execute();
    if (ret < 0) {
        ICO_WRN("execute failed(%s) err=%d", sb_package_name, ret);
    }
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::Initialize
 *          Initialize
 *
 * @param[in]   orientation    vertical or horizontal 
 * @param[in]   config         config 
 * @return      OK or ERROR
 */
/*--------------------------------------------------------------------------*/
int 
CicoHomeScreen::Initialize(int orientation,CicoHomeScreenConfig *config)
{
    
    ICO_DBG("CicoHomeScreen::Initialize: start"); 
    /*save instance pointer */
    hs_instance = this;

    /*config copy*/
    this->config = config;

    /* get pkg name (home screen,status bar,on screen)*/
    char *pkg;
    ICO_DBG("CicoHomeSceen::Initialize: %s: %s", 
            getenv("HOME"), getenv("PKG_NAME"));
    pkg = getenv("PKG_NAME");
    memset(hs_package_name, 0, ICO_HS_MAX_PROCESS_NAME);
    if(pkg){
        strncpy(hs_package_name, pkg, ICO_HS_MAX_PROCESS_NAME);
    }
    else{
        strncpy(hs_package_name, ICO_HS_PROC_DEFAULT_HOMESCREEN,
                ICO_HS_MAX_PROCESS_NAME);
    }

    /* Initialize WindowController */
    CicoHSWindowController::Initialize();
    ICO_DBG("CicoHomeScreen::Initialize: window controller initialized"); 
    
    ICO_DBG("CicoHomeScreen::Initialize: start connect to systemcontroller"); 
    ico_syc_connect(EventCallBack,NULL);
    ICO_DBG("CicoHomeScreen::Initialize: end connect to systemcontroller"); 
    
    /*init lifecycle controller*/
    CicoSCSystemConfig::getInstance()->load(ICO_HS_LIFECYCLE_CONTROLLER_SETTING_PATH);
    life_cycle_controller = CicoSCLifeCycleController::getInstance();
    
    /*initialize system controller*/
    /*my info*/
    ICO_DBG("CicoHomeScreen::Initialize: get package name %s",hs_package_name); 
    hs_app_info = new CicoHSAppInfo(hs_package_name); 
    
    /*start home statusbar and onscreen*/ 
    StartRelations();
    ICO_DBG("CicoHomeScreen::Initialize: start relation apps"); 
 
    /* Get screen size */
    CicoHSWindowController::GetFullScreenSize(orientation,
                                              &full_width,&full_height);

    ICO_DBG("full screen size x=%d y=%d",full_width,full_height);

    /* MenuScreen Window Size */
    menu_width = full_width;
    menu_height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT - 
                  ICO_HS_CONTROLBAR_WINDOW_HEIGHT;

    /* BackGround Window Size */
    bg_width = full_width;
    bg_height = full_height;

    /* Control Bar Window Size */
    ctl_bar_width = full_width;
    ctl_bar_height = ICO_HS_CONTROLBAR_WINDOW_HEIGHT;

    ICO_DBG("CicoHomeScreen::Initialize: end"); 

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::Finalize
 *          Finalize
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHomeScreen::Finalize(void)
{
    /* Finalize WindowController */
    CicoHSWindowController::Finalize();

    ico_syc_disconnect();

    delete hs_app_info;   
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateMenuWindow
 *          create menu window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::CreateMenuWindow(void)
{
    /*create window*/
    menu_window = new CicoHSMenuWindow();
    menu_window->CreateMenuWindow(ICO_HS_WINDOW_POS_X,ICO_HS_WINDOW_POS_Y,
                                  menu_width,menu_height);  
    menu_window->ShowWindow(); 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::DeleteMenuWindow
 *          delete menu window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::DeleteMenuWindow(void)
{
    if(menu_window == NULL){
        return;
    }
    menu_window->FreeMenuWindow();
    delete menu_window;

}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::UpdateMenuWindow
 *          update menu window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::UpDateMenuWindow(void)
{
    DeleteMenuWindow();
    CreateMenuWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateBackWindow
 *          create back window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::CreateBackWindow(void)
{
    /*create window*/
    back_window = new CicoHSBackWindow();
    back_window->CreateBackWindow(ICO_HS_WINDOW_POS_X,ICO_HS_WINDOW_POS_Y,
                                  bg_width,bg_height);  
    back_window->ShowWindow(); 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateBackWindow
 *          create back window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::DeleteBackWindow(void)
{
    if(back_window == NULL){
        return;
    }
    back_window->FreeBackWindow();
    delete back_window;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::UpDateBackWindow
 *          update back window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::UpDateBackWindow(void)
{
    DeleteBackWindow();
    CreateBackWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateControlBarWindow
 *          create control bar window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::CreateControlBarWindow(void)
{
    /*create window*/
    ctl_bar_window = new CicoHSControlBarWindow();
    ctl_bar_window->CreateControlBarWindow(ICO_HS_MENU_WINDOW_POS_X,bg_height - 
                                           ctl_bar_height,ctl_bar_width,
                                           ctl_bar_height);  
    ctl_bar_window->ShowWindow(); 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateControlBarWindow
 *          delete control bar window
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::DeleteControlBarWindow(void)
{
    if(ctl_bar_window == NULL){
        return;
    }
    ctl_bar_window->FreeControlBarWindow();
    delete ctl_bar_window;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::StartLoop
 *          start ecore loop
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::StartLoop(void)
{
    CicoHSWindowController::StartEcoreLoop();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::StartHomeScreen
 *          start home screen
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::StartHomeScreen()
{
    /*Get application info*/
    CreateAppInfoList();

    /*Create window*/
    CreateBackWindow();

    /*Create window*/
    CreateControlBarWindow();

    /*Create window*/
    CreateMenuWindow();

    /*Show Home Screen*/
    ShowHomeScreenLayer();

    /* start loop*/
    StartLoop();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::GetMode
 *          Get status
 *
 * @param[in]   mode status
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int
CicoHomeScreen::GetMode(void)
{
    return mode;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::SetMode
 *          Set status
 *
 * @param[in]   mode status
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::SetMode(int mode)
{
    this->mode = mode;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ChangeMode
 *          Change status
 *
 * @param[in]   none 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ChangeMode(int pattern)
{
    if(hs_instance->menu_window == NULL){
        return; 
    }

    if(hs_instance->GetMode() == ICO_HS_MODE_MENU){
        ico_syc_animation_t animation;
        if(pattern == ICO_HS_SHOW_HIDE_PATTERN_SLIDE){
            animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_SLIDE;
        }else{
            animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_FADE;
        }
        animation.time = ICO_HS_MENU_ANIMATION_DURATION;
        hs_instance->menu_window->Hide(&animation);
        hs_instance->SetMode(ICO_HS_MODE_APPLICATION);
    }else if(hs_instance->GetMode() ==ICO_HS_MODE_APPLICATION){
        ico_syc_animation_t animation;
        if(pattern == ICO_HS_SHOW_HIDE_PATTERN_SLIDE){
            animation.name = (char*)ICO_HS_MENU_SHOW_ANIMATION_SLIDE;
        }else{
            animation.name = (char*)ICO_HS_MENU_SHOW_ANIMATION_FADE;
        }
        animation.time = ICO_HS_MENU_ANIMATION_DURATION;
        hs_instance->menu_window->Show(&animation);
        hs_instance->SetMode(ICO_HS_MODE_MENU);
    }
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ExecuteApp
 *          execute application
 *
 * @param[in]   appid
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ExecuteApp(const char*appid)
{
    hs_instance->ExecuteApp_i(appid);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::TerminateApp
 *          teminate application
 *
 * @param[in]   appid
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::TerminateApp(const char*appid)
{
    hs_instance->TerminateApp_i(appid);
}
// vim: set expandtab ts=4 sw=4:
