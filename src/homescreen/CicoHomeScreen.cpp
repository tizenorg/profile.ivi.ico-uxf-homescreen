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
#include <string>
#include <vector>
#include <cstdlib>
#include "CicoHSAppHistory.h"
#include "CicoHSAppHistoryExt.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"
#include "CicoHSLifeCycleController.h"
#include "CicoConf.h"
#include "CicoHSCmdOpts.h"
#include "Cico_aul_listen_app.h"
#include "CicoLibString.h"
#include "CicoHSServer.h"
#include "CicoHSAppControl.h"
using namespace std;

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHomeScreen * CicoHomeScreen::hs_instance;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/

/**
 * ecore timer void data typedef
 */
typedef struct t_launcApps_data {
    string filePath;
    string filePathD;
    CicoHomeScreen *hs;
} launcApps_data_t;

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
    active_appinfo = NULL;

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
    num_swipe_input_windows = 0;

    menu_window = NULL;
    back_window = NULL;
    ctl_bar_window = NULL;

    //mode = ICO_HS_MODE_MENU;
    mode = ICO_HS_MODE_APPLICATION;
    config = NULL;
    hs_instance = NULL;

    m_appHis = NULL;

    life_cycle_controller = CicoHSLifeCycleController::getInstance();

    sub_display_appinfo = NULL;
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
    if (NULL != m_appHis) {
        delete m_appHis;
    }
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
    ico_syc_show_layer(HS_LAYER_CONTROLBAR);
    ico_syc_show_layer(HS_LAYER_TOUCHPANEL);
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
    std::vector<CicoAilItems> aillist = life_cycle_controller->getAilList();

    for (int ii = 0; ii < ICO_HS_MAX_APP_NUM; ii++) {
        if (apps_info[ii] != NULL)  {
            delete apps_info[ii];
        }
    }

    /*create instance*/
    application_num = 0;
    for (unsigned int ii = 0;
         (ii < aillist.size()) && (ii < ICO_HS_MAX_APP_NUM);
         ii++)  {
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
    if (NULL == appid) {
        return NULL;
    }

    for (int ii = 0;ii < hs_instance->application_num;ii++) {
        if (strcmp(hs_instance->apps_info[ii]->GetAppId(),appid) == 0)   {
            return hs_instance->apps_info[ii];
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
    for (int ii = 0;ii < hs_instance->application_num;ii++) {
        if (strcmp(hs_instance->apps_info[ii]->GetAppId(),appid) == 0)  {
            return hs_instance->apps_info[ii]->GetStatus();
        }
    }
    return false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change active window
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ChangeActive(const char * appid, int surface)
{
    CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(appid);
    if (appinfo == NULL)    {
        return;
    }

    ico_hs_window_info* wininfo = appinfo->GetWindowInfobySurface(surface);
    if (wininfo == NULL) {
        return;
    }

    appinfo->SetLastSurface(surface);

    // if window invisible, not update active information
    if (0 != wininfo->visible) {
        hs_instance->SetActiveAppInfo(appid);
        hs_instance->setActiveApp(appid);
        ico_syc_show(appid, surface, NULL);
        ico_syc_change_active(appid, surface);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change application display zone for active window
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ChangeZone(void)
{
    ICO_TRA("CicoHomeScreen::ChangeZone Enter");

    hs_instance->controlRegulation();

    CicoHSAppInfo* appinfo = hs_instance->GetSubDisplayAppInfo();
    // if sub display not exist showed appliction,
    // get main display showed application informaiton
    if (NULL == appinfo) {
        appinfo = hs_instance->GetActiveAppInfo();
        if (NULL == appinfo) {
            ICO_TRA("CicoHomeScreen::ChangeZone Leave(not found appinfo)");
            return;
        }
    }
    hs_instance->requestChangeZone(appinfo);
    ICO_TRA("CicoHomeScreen::ChangeZone Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change application display zone for appinfo
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::requestChangeZone(CicoHSAppInfo* appinfo)
{
    int surface = appinfo->GetLastSurface();
    ICO_TRA("Enter appid=%s, lastsurface=%d", appinfo->GetAppId(), surface);
    ico_hs_window_info* wininfo = appinfo->GetWindowInfobySurface(surface);
    if (NULL == wininfo) {
        ICO_TRA("Leave(not found wininfo)");
        return;
    }
    ICO_DBG("wininfo=%p", wininfo);

    const char *dispzone = moveZoneName;
    int layer = HS_LAYER_2NDDISP_APP;
    if (NULL != GetSubDisplayAppInfo()) {
        dispzone = appinfo->GetDefaultZone();
        layer = HS_LAYER_APPLICATION;
    }

    if (NULL == dispzone) {
        ICO_TRA("Leave(not found dispzone)");
        return;
    }

    ICO_DBG("change zone: \"%s\"->\"%s\"", wininfo->zone, dispzone);
    ico_syc_win_move_t move = {
        .zone   = (char*)dispzone,
        .layer  = layer,
        .pos_x  = 0,
        .pos_y  = 0,
        .width  = 0,
        .height = 0
    };

    ico_syc_move(wininfo->appid, wininfo->surface, &move,
                 &hs_instance->moveZoneAnimation);

    ICO_TRA("Leave");
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
 * @brief   CicoHomeScreen::GetWindowInfo
 *          get window info
 *
 * @param[in]   appinfo    application info
 * @param[in]   surface    surface id
 * @return      Window info
 */
/*--------------------------------------------------------------------------*/
ico_hs_window_info *
CicoHomeScreen::GetWindowInfo(CicoHSAppInfo *appinfo, int surface)
{
    return appinfo->GetWindowInfobySurface(surface);
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
    if (appinfo == NULL)    {
        return;
    }
    if (appinfo->GetStatus() == false)  {
        //show icon
        ICO_DBG("CicoHomeScreen::UpdateTile: show menuIcon(%s)", appid);
        menu_window->ValidMenuIcon(appid);
    }
    else    {
        //show thumbnail
        ICO_DBG("CicoHomeScreen::UpdateTile: show app thumbnail(%s)", appid);
        menu_window->ValidThumbnail(appid, appinfo->GetLastSurface());
    }
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  set current active application information
 *
 *  @param [in] appid    application id
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::SetActiveAppInfo(const char *appid)
{
    CicoHSAppInfo* appinfo = NULL;
    if (NULL != appid) {
        appinfo = GetAppInfo(appid);

        // if active window is HomeScreen/StatusBar/OnScreen,
        // not update active application information
        ICO_DBG("appinfo=%08x hs_app_info=%08x sb_app_info=%08x os_app_info=%08x",
                appinfo, hs_app_info, sb_app_info, os_app_info);
        if ((appinfo == hs_app_info) ||
            (appinfo == sb_app_info) ||
            (appinfo == os_app_info)) {
            return;
        }
    }
    ICO_DBG("active application changed. \"%s\"->\"%s\"",
            active_appinfo ? active_appinfo->GetAppId() : "unknown",
            appinfo ? appinfo->GetAppId() : "unknown");

    active_appinfo = appinfo;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  update current active application information
 *
 *  @param [in] appid    application id
 */
/*--------------------------------------------------------------------------*/
CicoHSAppInfo *
CicoHomeScreen::GetActiveAppInfo(void)
{
    return active_appinfo;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  set current sub display application information
 *
 *  @param [in] appid    application id
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::SetSubDisplayAppInfo(const char *appid)
{
    if (NULL == appid) {
        if (NULL == sub_display_appinfo) {
            ICO_WRN("already not use");
            return;
        }
        ICO_DBG("sub display appinfo changed. \"%s\"->\"none\"",
                sub_display_appinfo->GetAppId());
        sub_display_appinfo = NULL;
        return;
    }

    // if sub_display_appinfo is not NULL,
    // not update sub display application information
    if (NULL != sub_display_appinfo) {
        ICO_WRN("sub display use. appid=%s",
                sub_display_appinfo->GetAppId());
        return;
    }

    CicoHSAppInfo* appinfo = GetAppInfo(appid);

    // if sub display appinfo is HomeScreen/StatusBar/OnScreen,
    // not update sub display application information
    ICO_DBG("appinfo=%08x hs_app_info=%08x sb_app_info=%08x os_app_info=%08x",
            appinfo, hs_app_info, sb_app_info, os_app_info);
    if ((appinfo == hs_app_info) ||
        (appinfo == sb_app_info) ||
        (appinfo == os_app_info)) {
        return;
    }

    if (NULL != appinfo) {
        ICO_DBG("sub display appinfo changed. \"none\"->\"%s\"",
                appinfo->GetAppId());
        sub_display_appinfo = appinfo;
    }
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  update current sub display application information
 *
 */
/*--------------------------------------------------------------------------*/
CicoHSAppInfo *
CicoHomeScreen::GetSubDisplayAppInfo(void)
{
    return sub_display_appinfo;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  update current sub display application appid
 *
 *  @return appid    application id
 *  @retval NULL : sub display application nothing
 *  @retval not NULL : appid
 */
/*--------------------------------------------------------------------------*/
const char*
CicoHomeScreen::GetSubDisplayAppid(void)
{
    if (NULL == sub_display_appinfo) {
        return NULL;
    }
    const char* pR = sub_display_appinfo->GetAppId();
    if ((NULL == pR) || (0 == pR) || (0 == strlen(pR))) {
        return NULL;
    }
    return pR;
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
    ICO_TRA("start %s", appid);
    CicoHSAppInfo *appinfo = GetAppInfo(appid);
    if (appinfo == NULL)    {
        ICO_TRA("end get appinfo is NULL");
        return;
    }
    if (sub_display_appinfo != appinfo) {
        if (appinfo->GetStatus() == false)  {
            //execute
            appinfo->Execute();
            ICO_DBG("execute app %s", appid);
        }
        else {
            //raise
            RaiseApplicationWindow(appinfo->GetAppId(),
                                   appinfo->GetLastSurface());
            ICO_DBG("raise app %s", appid);
            m_appHis->moveHistoryHead(appid);
        }
    }
    /* hide HomeScreen layer                        */
    if (GetMode() == ICO_HS_MODE_MENU)  {
        ChangeMode(ICO_HS_SHOW_HIDE_PATTERN_FADE);
    }
    ICO_TRA("end");
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
    if (appid != NULL)  {
        CicoHSAppInfo *appinfo = GetAppInfo(appid);
        if (appinfo == NULL)    {
            return;
        }

        //terminate
        appinfo->Terminate();
    }
    /* hide HomeScreen layer                        */
    if (GetMode() == ICO_HS_MODE_MENU)  {
        ChangeMode(ICO_HS_SHOW_HIDE_PATTERN_FADE);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowHomeScreenWindow
 *          show window (home screen)
 *
 * @param[in]   win info    window information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowHomeScreenWindow(ico_syc_win_info_t *win_info)
{
    ico_syc_win_move_t move;
    int layer;
    move.zone = NULL;
    move.pos_x = 0;
    move.width = full_width;

    if (strncmp(win_info->name,ICO_HS_BACK_WINDOW_TITLE,
                ICO_MAX_TITLE_NAME_LEN) == 0) {
        move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
        move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                      ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_BACKGROUND;
    }
    else if (strncmp(win_info->name,ICO_HS_CONTROL_BAR_WINDOW_TITLE,
                     ICO_MAX_TITLE_NAME_LEN) == 0) {
        move.pos_y = full_height - ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        move.height = ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_CONTROLBAR;
    }
    else if (strncmp(win_info->name,ICO_HS_MENU_WINDOW_TITLE,
                     ICO_MAX_TITLE_NAME_LEN) == 0) {
        move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
        move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                      ICO_HS_CONTROLBAR_WINDOW_HEIGHT;
        layer = HS_LAYER_HOMESCREEN;
    }
    else    {
        /*do nothing*/
        return;
    }

    ico_syc_change_layer(win_info->appid,win_info->surface,layer);
    ICO_DBG("CicoHomeScreen::ShowHomeScreenWindow: id(%s) name(%s) surface(%d) "
            "pos(%d,%d) size(%d,%d)", win_info->appid,
            win_info->name, win_info->surface, move. pos_x, move.pos_y,
            move.width, move.height);
    ico_syc_move(win_info->appid, win_info->surface, &move, NULL);

    /*first time menu is unvisible*/
    if ((strncmp(win_info->name,ICO_HS_MENU_WINDOW_TITLE,
                 ICO_MAX_TITLE_NAME_LEN) == 0) &&
        (GetMode() == ICO_HS_MODE_APPLICATION)) {
        return;
    }
    ico_syc_show(win_info->appid, win_info->surface,NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowStatusBarWindow
 *          show window (home screen)
 *
 * @param[in]   win info    window information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowStatusBarWindow(ico_syc_win_info_t *win_info)
{
    ico_syc_win_move_t move;
    int layer;
    move.zone = NULL;
    move.pos_x = 0;
    move.pos_y = 0;
    move.width = full_width;
    move.height = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
    layer = HS_LAYER_CONTROLBAR;

    ico_syc_change_layer(win_info->appid,win_info->surface,layer);
    ICO_DBG("CicoHomeScreen::ShowStatusBarWindow: id(%s) name(%s) surface(%d) "
            "pos(%d,%d) size(%d,%d)",win_info->appid,
            win_info->name,win_info->surface,move.pos_x,move.pos_y,
            move.width,move.height);
    ico_syc_move(win_info->appid,win_info->surface,&move,NULL);
    ico_syc_show(win_info->appid, win_info->surface,NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowApplicationWindow
 *          show window (application)
 *
 * @param[in]   win info    window information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowApplicationWindow(ico_syc_win_info_t *win_info)
{
    ICO_TRA("CicoHomeScreen::ShowApplicationWindow Enter");

    CicoHSLifeCycleController *lifecycle =
        CicoHSLifeCycleController::getInstance();
    const CicoAilItems* ailItem = lifecycle->findAIL(win_info->appid);
    if (NULL == ailItem) {
        ICO_WRN("ailItem not found.");
        ICO_TRA("CicoHomeScreen::ShowApplicationWindow Leave");
        return;
    }
    int zoneid = ailItem->m_displayZone;

    ICO_DBG("zoneid=%d", zoneid);
    const CicoSCDisplayZoneConf* dispZoneConf =
        CicoSystemConfig::getInstance()->findDisplayZoneConfbyId(zoneid);
    if (NULL == dispZoneConf) {
        ICO_WRN("display zone config not found.");
        ICO_TRA("CicoHomeScreen::ShowApplicationWindow Leave");
        return;
    }
    ICO_DBG("appid=%s zoneid=%d:%s",
            win_info->appid, zoneid, dispZoneConf->fullname.c_str());

    ico_syc_win_move_t move;

    move.zone  = (char*)dispZoneConf->fullname.c_str();
    move.layer = HS_LAYER_APPLICATION;
    if (ailItem->m_display != 0) {
        move.layer = HS_LAYER_2NDDISP_APP;
    }
    move.pos_x = 0;
    move.width = full_width;
    move.pos_y = ICO_HS_STATUSBAR_WINDOW_HEIGHT;
    move.height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                  ICO_HS_CONTROLBAR_WINDOW_HEIGHT;

    ico_syc_move(win_info->appid,win_info->surface,&move,NULL);
    ico_syc_animation_t animation;
    animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_FADE;
    animation.time = ICO_HS_MENU_ANIMATION_DURATION;
#if 0
    ico_syc_show(win_info->appid, win_info->surface, &animation);
#else
    const string& lastStartupApp = m_appHis->getLastStartupAppid();
    if (true == lastStartupApp.empty()) {
        ICO_DBG("show (empty) %s", win_info->appid);
        ico_syc_show(win_info->appid, win_info->surface, &animation);
    }
    else {
        bool bShow = false;
#if 0
        const string& subDispApp = m_appHis->getSubDispAppid();
#endif
        if (0 == lastStartupApp.compare(win_info->appid)) {
            bShow = true;
        }
#if 0
        if (0 == subDispApp.compare(win_info->appid)) {
            bShow = true;
        }
#endif
        if (true == bShow) {
            ICO_DBG("show (last) %s", win_info->appid);
            ico_syc_show(win_info->appid, win_info->surface, &animation);
        }
        else {
            ICO_DBG("hide (no last) %s", win_info->appid);
            ico_syc_hide(win_info->appid, win_info->surface, &animation);
        }
    }
#endif
    ICO_TRA("CicoHomeScreen::ShowApplicationWindow Leave");
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
    ico_syc_show(appid, surface, NULL);
    ico_syc_change_active(appid, surface);
    hs_instance->SetActiveAppInfo(appid);
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
    int     idx;

//    ICO_DBG("CicoHomeScreen::EventCallBack: start (event %d)",event);

    if (event == ICO_SYC_EV_WIN_CREATE) {
        ico_syc_win_info_t *win_info =
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("Event:ICO_SYC_EV_WIN_CREATE(appid=%s surface=0x%08x)",
                win_info->appid, win_info->surface);
        /*only save information*/
        if (strncmp(win_info->appid, hs_instance->GetHsPackageName(),
                    ICO_HS_MAX_PROCESS_NAME) == 0)  {
            /*Home Screen*/
            hs_instance->hs_app_info->AddWindowInfo(win_info);

            /*when Menu window*/
            if (strncmp(win_info->name,ICO_HS_MENU_WINDOW_TITLE,
                        ICO_MAX_TITLE_NAME_LEN) == 0) {
                hs_instance->menu_window->SetMenuWindowID(win_info->appid,
                                                          win_info->surface);
            }
            else if (strncmp(win_info->name,ICO_HS_CONTROL_BAR_WINDOW_TITLE,
                             ICO_MAX_TITLE_NAME_LEN) == 0) {
                hs_instance->ctl_bar_window->SetWindowID(win_info->appid,
                                                         win_info->surface);
            }
            else    {
                if (win_info->name[0] == 0) {
                  /* null name, nothing to do */
                    return;
                }
                for (idx = 0; idx < hs_instance->num_swipe_input_windows; idx++)    {
                    if (hs_instance->swipe_input_windows[idx]->
                                         isMyWindowName(win_info->name)) {
                        hs_instance->swipe_input_windows[idx]->
                                         SetWindowID(win_info->appid, win_info->surface);
                        hs_instance->swipe_input_windows[idx]->SetupSwipeWindow();
                        break;
                    }
                }
            }
            /*show window*/
            hs_instance->ShowHomeScreenWindow(win_info);
        }
        else if (strncmp(win_info->appid,
                         hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*Status Bar*/
            hs_instance->sb_app_info->AddWindowInfo(win_info);

            /*show window*/
            hs_instance->ShowStatusBarWindow(win_info);
        }
        else if (strncmp(win_info->appid,
                         hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*On Screen*/
            hs_instance->os_app_info->AddWindowInfo(win_info);
        }
        else    {
            /*Application*/
            CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(win_info->appid);
            if (appinfo == NULL) {
                return;
            }
            appinfo->AddWindowInfo(win_info);

            appinfo->SetLastSurface(win_info->surface);
            hs_instance->ShowApplicationWindow(win_info);
            ico_syc_change_active(win_info->appid,win_info->surface);
            hs_instance->SetActiveAppInfo(win_info->appid);
            hs_instance->UpdateTile(win_info->appid);

            hs_instance->startupCheck(win_info->appid);
        }
    }
    else if (event == ICO_SYC_EV_WIN_NAME) {
        ico_syc_win_info_t *win_info =
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("Event:ICO_SYC_EV_WIN_NAME(appid=%s surface=0x%08x name=%s)",
                win_info->appid, win_info->surface, win_info->name);

        if (strncmp(win_info->appid, hs_instance->GetHsPackageName(),
                    ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*Home Screen*/
            hs_instance->hs_app_info->AddWindowInfo(win_info);

            /*when Menu window*/
            if (strncmp(win_info->name,ICO_HS_MENU_WINDOW_TITLE,
                        ICO_MAX_TITLE_NAME_LEN) == 0) {
                hs_instance->menu_window->SetMenuWindowID(win_info->appid,
                                                          win_info->surface);
            }
            else if (strncmp(win_info->name,ICO_HS_CONTROL_BAR_WINDOW_TITLE,
                             ICO_MAX_TITLE_NAME_LEN) == 0) {
                hs_instance->ctl_bar_window->SetWindowID(win_info->appid,
                                                         win_info->surface);
            }
            else    {
                for (idx = 0; idx < hs_instance->num_swipe_input_windows; idx++)    {
                    if (hs_instance->swipe_input_windows[idx]->
                                         isMyWindowName(win_info->name)) {
                        hs_instance->swipe_input_windows[idx]->
                                         SetWindowID(win_info->appid, win_info->surface);
                        hs_instance->swipe_input_windows[idx]->SetupSwipeWindow();
                        break;
                    }
                }
            }

            /*show window*/
            hs_instance->ShowHomeScreenWindow(win_info);
        }
        else if (strncmp(win_info->appid, hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*Status Bar*/
            hs_instance->sb_app_info->AddWindowInfo(win_info);
        }
        else if (strncmp(win_info->appid, hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*On Screen*/
            hs_instance->os_app_info->AddWindowInfo(win_info);
        }
        else {
            /*Application*/
            CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(win_info->appid);
            if (appinfo == NULL) {
                return;
            }
            appinfo->AddWindowInfo(win_info);
        }
    }
    else if (event == ICO_SYC_EV_WIN_DESTROY) {
        ico_syc_win_info_t *win_info =
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("Event:ICO_SYC_EV_WIN_DESTROY(appid=%s surface=0x%08x)",
                win_info->appid, win_info->surface);
        /*only save information*/
        if (strncmp(win_info->appid, hs_instance->GetHsPackageName(),
                    ICO_HS_MAX_PROCESS_NAME) == 0)  {
            hs_instance->hs_app_info->FreeWindowInfo(win_info->name);
        }
        else if (strncmp(win_info->appid,
                         hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            hs_instance->sb_app_info->FreeWindowInfo(win_info->name);
        }
        else if (strncmp(win_info->appid,
                         hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            hs_instance->os_app_info->FreeWindowInfo(win_info->name);
        }
        else    {
            /*Application*/
            CicoHSAppInfo *appinfo =
                       hs_instance->GetAppInfo(win_info->appid);
            if (appinfo == NULL) {
                return;
            }
            appinfo->FreeWindowInfo(win_info->surface);
            hs_instance->UpdateTile(win_info->appid);

            // update active application information
            CicoHSAppInfo *active_appinfo = hs_instance->GetActiveAppInfo();
            if (appinfo == active_appinfo) {
                hs_instance->SetActiveAppInfo(NULL);
            }

            // update sub displayed application information
            CicoHSAppInfo *subdisp_appinfo = hs_instance->GetSubDisplayAppInfo();
            if (appinfo == subdisp_appinfo) {
                hs_instance->SetSubDisplayAppInfo(NULL);
            }
        }
    }
    else if (event == ICO_SYC_EV_WIN_ACTIVE) {
        ico_syc_win_info_t *win_info =
            reinterpret_cast<ico_syc_win_info_t*>(const_cast<void*>(detail));
        ICO_DBG("Event:ICO_SYC_EV_WIN_ACTIVE(appid=%s surface=0x%08x)",
                win_info->appid, win_info->surface);
        hs_instance->ChangeActive(win_info->appid, win_info->surface);
    }
    else if (event == ICO_SYC_EV_WIN_ATTR_CHANGE)   {
        ico_syc_win_attr_t *win_attr =
            reinterpret_cast<ico_syc_win_attr_t*>(const_cast<void*>(detail));
        ICO_DBG("Event:ICO_SYC_EV_WIN_ATTR_CHANGE(appid=%s surface=0x%08x)",
                win_attr->appid, win_attr->surface);
        if (strncmp(win_attr->appid, hs_instance->GetHsPackageName(),
                    ICO_HS_MAX_PROCESS_NAME) == 0)  {
            /*Home Screen*/
            hs_instance->hs_app_info->AddWindowAttr(win_attr);
        }
        else if (strncmp(win_attr->appid, hs_instance->GetSbPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*Status Bar*/
            hs_instance->sb_app_info->AddWindowAttr(win_attr);
        }
        else if (strncmp(win_attr->appid, hs_instance->GetOsPackageName(),
                         ICO_HS_MAX_PROCESS_NAME) == 0) {
            /*On Screen*/
            hs_instance->os_app_info->AddWindowAttr(win_attr);
        }
        else {
            ICO_DBG("CicoHomeScreen::EventCallBack : application window %s %s",
                    win_attr->appid,win_attr->name);
            /*Application*/
            CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(win_attr->appid);
            if (appinfo == NULL) {
                return;
            }

            // change layer when moved zone in second display
            ico_hs_window_info *winfo = NULL;
            winfo = appinfo->GetWindowInfobySurface(win_attr->surface);
            if (NULL != winfo) {
                if (win_attr->nodeid != winfo->nodeid) {
                    if (0 == win_attr->nodeid) {
                        if (NULL != hs_instance->GetSubDisplayAppInfo()) {
                            if (hs_instance->m_appHis) {
                                const string& wapp = hs_instance->m_appHis->getSelectApp();
                                ICO_DBG("\"%s\"->\"%s\"", wapp.c_str(), winfo->appid);
                                if ((false == wapp.empty()) &&
                                    (0 != wapp.compare(winfo->appid))) {
                                    hs_instance->requestHideAppid(wapp);
                                    hs_instance->m_appHis->clearSelectApp();
                                    hs_instance->m_appHis->homeSwipe();
                                }
                            }
                            ICO_DBG("reset sub display");
                            hs_instance->SetSubDisplayAppInfo(NULL);
                            hs_instance->ChangeActive(winfo->appid, winfo->surface);
                        }
                    }
                    else {
                        ICO_DBG("set sub display");
                        hs_instance->SetSubDisplayAppInfo(winfo->appid);
                    }

                    if (hs_instance->m_appHis) {
                        if (true == hs_instance->m_appHis->chgChk()) {
                            hs_instance->m_appHis->writeAppHistory();
                        }
                        hs_instance->m_appHis->homeSwipe();
                    }
                }
            }

            // update window information
            appinfo->AddWindowAttr(win_attr);
            hs_instance->UpdateTile(win_attr->appid);
        }
    }
    else if (event == ICO_SYC_EV_THUMB_CHANGE)  {
        ico_syc_thumb_info_t *thumb_info =
            reinterpret_cast<ico_syc_thumb_info_t*>(const_cast<void*>(detail));
        CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(thumb_info->appid);
        if (appinfo == NULL)    {
            ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_CHANGE %s(%02x) "
                    "no appinfo", thumb_info->appid, thumb_info->surface);
            return;
        }
//      ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_CHANGE %s(%02x)",
//              thumb_info->appid, thumb_info->surface);
        // show thumbnail icon
        hs_instance->menu_window->SetThumbnail(thumb_info->appid, thumb_info);
    }
    else if (event == ICO_SYC_EV_THUMB_UNMAP)  {
        ico_syc_thumb_info_t *thumb_info =
            reinterpret_cast<ico_syc_thumb_info_t*>(const_cast<void*>(detail));
        CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(thumb_info->appid);
        if (appinfo == NULL)    {
            ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_UNMAP %s(%02x) "
                    "no appinfo", thumb_info->appid, thumb_info->surface);
            return;
        }
        ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_UNMAP %s(%02x)",
                thumb_info->appid, thumb_info->surface);
        hs_instance->menu_window->SetThumbnail(thumb_info->appid, NULL);
    }
    else if (event == ICO_SYC_EV_THUMB_ERROR)  {
        ico_syc_thumb_info_t *thumb_info =
            reinterpret_cast<ico_syc_thumb_info_t*>(const_cast<void*>(detail));
        CicoHSAppInfo *appinfo = hs_instance->GetAppInfo(thumb_info->appid);
        if (appinfo == NULL)    {
            ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_ERROR %s(%02x) "
                    "no appinfo", thumb_info->appid, thumb_info->surface);
            return;
        }
        ICO_DBG("CicoHomeScreen::EventCallBack : ICO_SYC_EV_THUMB_ERROR %s(%02x)",
                thumb_info->appid, thumb_info->surface);
        hs_instance->menu_window->SetThumbnail(thumb_info->appid, NULL);
    }
    else if (event == ICO_SYC_EV_LAYER_ATTR_CHANGE) {

    }
    else if (event == ICO_SYC_EV_USERLIST)  {

    }
    else if (event == ICO_SYC_EV_AUTH_FAIL) {

    }
    else if (event == ICO_SYC_EV_RES_ACQUIRE) {

    }
    else if (event == ICO_SYC_EV_RES_DEPRIVE)   {

    }
    else if (event == ICO_SYC_EV_RES_WAITING)   {

    }
    else if (event == ICO_SYC_EV_RES_REVERT)    {

    }
    else if (event == ICO_SYC_EV_RES_RELEASE)   {

    }
    else if (event == ICO_SYC_EV_INPUT_SET) {

    }
    else if (event == ICO_SYC_EV_INPUT_UNSET)   {

    }
    else if (event == ICO_SYC_EV_STATE_CHANGE)  {
        ico_syc_state_info_t *state_info =
            reinterpret_cast<ico_syc_state_info_t*>(const_cast<void*>(detail));
        ICO_DBG("EVENT:ICO_SYC_EV_STATE_CHANGE(id=%d state=%d)",
                state_info->id, state_info->state);
        if (ICO_SYC_STATE_REGULATION == state_info->id) {
            // set regulation state
            CicoHSSystemState::getInstance()->setRegulation(
                (state_info->state == ICO_SYC_STATE_ON) ? true : false);
            ecore_main_loop_thread_safe_call_async(CicoHomeScreen::SetRegulation, NULL);
        }
        else if (ICO_SYC_STATE_NIGHTMODE == state_info->id) {
            // set night mode state
            CicoHSSystemState::getInstance()->setNightMode(
                (state_info->state == ICO_SYC_STATE_ON) ? true : false);
            ecore_main_loop_thread_safe_call_async(CicoHomeScreen::SetNightMode, NULL);
        }
    }
//    ICO_DBG("CicoHomeScreen::EventCallBack: end");
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
// TODO
#if 1
    /* start onscreen & statusbar apps */
    os_app_info = GetAppInfo(os_package_name);
    ret = os_app_info->Execute();
    if (ret < 0) {
        ICO_WRN("execute failed(%s) err=%d", os_package_name, ret);
    }
#endif

    sb_app_info = GetAppInfo(sb_package_name);
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
CicoHomeScreen::Initialize(int orientation, CicoGKeyFileConfig *config)
{

    ICO_DBG("CicoHomeScreen::Initialize: start");
    /*save instance pointer */
    hs_instance = this;

    /*config copy*/
    this->config = new CicoGKeyFileConfig();
    this->config->Initialize(ICO_HOMESCREEN_CONFIG_FILE);

    /*Get application info*/
    CreateAppInfoList();

    /* get pkg name (home screen,status bar,on screen)*/
    char *pkg;
    ICO_DBG("CicoHomeSceen::Initialize: %s: %s",
            getenv("HOME"), getenv("PKG_NAME"));
    pkg = getenv("PKG_NAME");
    memset(hs_package_name, 0, ICO_HS_MAX_PROCESS_NAME);
    if (pkg)    {
        strncpy(hs_package_name, pkg, ICO_HS_MAX_PROCESS_NAME);
    }
    else    {
        strncpy(hs_package_name, ICO_HS_PROC_DEFAULT_HOMESCREEN,
                ICO_HS_MAX_PROCESS_NAME);
    }

    /* Initialize WindowController */
    CicoHSWindowController::Initialize();
    ICO_DBG("CicoHomeScreen::Initialize: window controller initialized");

    ICO_DBG("CicoHomeScreen::Initialize: start connect to systemcontroller");
    ico_syc_connect(EventCallBack,NULL);
    ICO_DBG("CicoHomeScreen::Initialize: end connect to systemcontroller");

    /*initialize system controller*/
    /*my info*/
    ICO_DBG("CicoHomeScreen::Initialize: get package name %s",hs_package_name);
    hs_app_info = GetAppInfo(hs_package_name);

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

    // load switch display zone config
    moveZoneName = config->ConfigGetString("switchzone", "movezone", NULL);

    moveZoneAnimation.name = (char*)config->ConfigGetString("switchzone",
                                                            "animation",
                                                            ICO_HS_ANIMATION_FADE);

    moveZoneAnimation.time = config->ConfigGetInteger("switchzone",
                                                      "animatime", 400);

    // debug log
    ICO_DBG("moveZoneName=%s animation=%s time=%d",
            moveZoneName, moveZoneAnimation.name, moveZoneAnimation.time);

    ICO_DBG("CicoHomeScreen::Initialize: end");

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::InitializeAppHistory
 *          Initialize application History Manager
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::InitializeAppHistory(void)
{
    ICO_DBG("start");
    if (NULL != m_appHis) {
        ICO_DBG("end");
        return;
    }
    CicoHSCmdOpts* cmdOpts = CicoHSCmdOpts::getInstance();
    m_appHis = new CicoHSAppHistoryExt(cmdOpts->getLoginUser(),
                                       cmdOpts->getHistoryFilePath(),
                                       cmdOpts->getDefaultHistoryFilePath(),
                                       cmdOpts->getFlagPath());
    m_appHis->force_flagoff();
    m_appHis->setHomeScreen(this);

    const char* val;
    val = config->ConfigGetString(ICO_HS_CONFIG_HISTORY,
                                  ICO_HS_CONFIG_HSTRY_KEY1,
                                  ICO_HS_CONFIG_HSTRY_DEF1);
    ICO_DBG("timer = %s", val);
    double dV = atof(val);
    m_appHis->setWaitTime(dV);

    val = config->ConfigGetString(ICO_HS_CONFIG_HISTORY,
                                  ICO_HS_CONFIG_HSTRY_KEY2,
                                  ICO_HS_CONFIG_HSTRY_DEF2);
    vector<string> vvM;
    if ((NULL != val) && (0 != val) && (0 != strlen(val))) {
        string tmpM(val);
        split(tmpM, ICO_HS_CONFIG_HSTRY_SPL, vvM);
    }
    if (0 != vvM.size()) {
        ICO_DBG("manage off = %s split=%d", val, (int)vvM.size());
        m_appHis->setFilterManage(vvM);
    }
    val = config->ConfigGetString(ICO_HS_CONFIG_HISTORY,
                                  ICO_HS_CONFIG_HSTRY_KEY3,
                                  ICO_HS_CONFIG_HSTRY_DEF3);
    vector<string> vvW;
    if ((NULL != val) && (0 != val) && (0 != strlen(val))) {
        string tmpW(val);
        split(tmpW, ICO_HS_CONFIG_HSTRY_SPL, vvW);
    }
    if (0 != vvW.size()) {
        ICO_DBG("write off = %s, split=%d", val, (int)vvW.size());
        m_appHis->setFilterWrite(vvW);
    }
    ICO_DBG("end");
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
    ICO_DBG("CicoHomeScreen::CreateMenuWindow: start");
    menu_window = new CicoHSMenuWindow();
    menu_window->CreateMenuWindow(ICO_HS_WINDOW_POS_X,ICO_HS_WINDOW_POS_Y,
                                  menu_width,menu_height);
    menu_window->ShowWindow();
    ICO_DBG("CicoHomeScreen::CreateMenuWindow: end");
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
    if (menu_window == NULL)    {
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
    ICO_DBG("CicoHomeScreen::CreateBackWindow");
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
    if (back_window == NULL)    {
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
    ICO_DBG("CicoHomeScreen::CreateControlBarWindow");
    ctl_bar_window = new CicoHSControlBarWindow();
    ctl_bar_window->CreateControlBarWindow(ICO_HS_MENU_WINDOW_POS_X,bg_height -
                                           ctl_bar_height,ctl_bar_width,
                                           ctl_bar_height);
    ctl_bar_window->ShowWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::DeleteControlBarWindow
 *          delete control bar window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::DeleteControlBarWindow(void)
{
    if (ctl_bar_window == NULL) {
        return;
    }
    ctl_bar_window->FreeControlBarWindow();
    delete ctl_bar_window;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::CreateSwipeInputWindow
 *          create swipe input windows
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::CreateSwipeInputWindow(void)
{
    ICO_DBG("CicoHomeScreen::CreateSwipeInputWindow: start");
    /* TODO: cullentry SwipeInput windows is fixed size, need configuration ?   */
    /* left side window     */
    swipe_input_windows[0] = new CicoHSSwipeInputWindow();
    swipe_input_windows[0]->
            CreateSwipeInputWindow(ICO_HS_WINDOW_POS_X,
                                   ICO_HS_WINDOW_POS_Y + ICO_HS_SWIPE_TOUCH_DISTANCE_XY,
                                   ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH,
                                   full_height - (ICO_HS_SWIPE_TOUCH_DISTANCE_XY*2), "left");
    swipe_input_windows[0]->ShowWindow();

    /* right side window    */
    swipe_input_windows[1] = new CicoHSSwipeInputWindow();
    swipe_input_windows[1]->
            CreateSwipeInputWindow(full_width - ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH,
                                   ICO_HS_WINDOW_POS_Y + ICO_HS_SWIPE_TOUCH_DISTANCE_XY,
                                   ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH,
                                   full_height - (ICO_HS_SWIPE_TOUCH_DISTANCE_XY*2), "right");
    swipe_input_windows[1]->ShowWindow();

#if 0       /* currently not support(not fix specification) */
    /* buttom side window   */
    swipe_input_windows[2] = new CicoHSSwipeInputWindow();
    swipe_input_windows[2]->
            CreateSwipeInputWindow(ICO_HS_WINDOW_POS_X,
                                   full_height - ICO_HS_SWIPE_TOUCH_SWIPE_HEIGHT,
                                   full_width, ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH, "buttom");

    swipe_input_windows[2]->ShowWindow();
    num_swipe_input_windows = 3;
#else
    num_swipe_input_windows = 2;
#endif
    ICO_DBG("CicoHomeScreen::CreateSwipeInputWindow: end");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::DeleteSwipeInputWindow
 *          delete swipe input windows
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::DeleteSwipeInputWindow(void)
{
    int idx;

    for (idx = 0; idx < num_swipe_input_windows; idx++) {
        if (swipe_input_windows[idx])   {
            swipe_input_windows[idx]->FreeSwipeInputWindow();
            delete swipe_input_windows[idx];
            swipe_input_windows[idx] = NULL;
        }
    }
    num_swipe_input_windows = 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   launch applications
 *
 * @param   filepath  start applications list file path
 * @param   filepathD defalt start applications list file path
 * @return  bool
 * @retval  true  success
 * @retval  false fail
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool launchApps(void* data)
{   
    ICO_DBG("start"); 
    launcApps_data_t* x = (launcApps_data_t*) data;
    if ((NULL == x) || (NULL == x->hs)) {
        ICO_DBG("end fail");
        return ECORE_CALLBACK_CANCEL;
    }
    
    vector<pairAppidSubd> apps;
    x->hs->readStartupApp(apps);
    
    int sz = apps.size();
    for (int i =sz; i > 0; i--) {
        string appid = apps[i-1].first;
        bool bFLAG = apps[i-1].second;
        const char* app = appid.c_str();
        ICO_PRF("CHG_APP_STA 1.request app=%s", app);
        int pid = aul_launch_app(appid.c_str(), NULL);
        ICO_DBG("            1.request [%d]%d:%s:%d", i, pid, app, (int)bFLAG);
        if ((0 < pid) && (NULL != x->hs)) {
            x->hs->startupCheckAdd(pid, appid, bFLAG);
        }
    }
    ICO_DBG("end success");
    return ECORE_CALLBACK_CANCEL;
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
int
CicoHomeScreen::StartHomeScreen(int orientation)
{
    ICO_TRA("CicoHomeScreen::StartHomeScreen Enter");

    /*save instance pointer */
    hs_instance = this;

    /*config copy*/
    this->config = new CicoGKeyFileConfig();
    this->config->Initialize(ICO_HOMESCREEN_CONFIG_FILE);

    /*Get application info*/
    CreateAppInfoList();

    /* get pkg name (home screen,status bar,on screen)*/
    char *pkg;
    ICO_DBG("CicoHomeSceen::StartHomeScreen: %s: %s",
            getenv("HOME"), getenv("PKG_NAME"));
    pkg = getenv("PKG_NAME");
    memset(hs_package_name, 0, ICO_HS_MAX_PROCESS_NAME);
    if (pkg)    {
        strncpy(hs_package_name, pkg, ICO_HS_MAX_PROCESS_NAME);
    }
    else    {
        strncpy(hs_package_name, ICO_HS_PROC_DEFAULT_HOMESCREEN,
                ICO_HS_MAX_PROCESS_NAME);
    }

    /* create application control instance */
    CicoHSAppControl *m_appctrl = CicoHSAppControl::getInstance();

    /* create server instance */
    CicoHSServer *server = CicoHSServer::getInstance();
    server->setAppCtrl(m_appctrl);
    server->startup(10001, (const char*)"ico_hs_protocol");

    /* Initialize WindowController */
    CicoHSWindowController::Initialize();
    ICO_DBG("CicoHomeScreen::StartHomeScreen: window controller initialized");

    ICO_DBG("CicoHomeScreen::StartHomeScreen: start connect to systemcontroller");
    ico_syc_connect(EventCallBack,NULL);
    ICO_DBG("CicoHomeScreen::StartHomeScreen: end connect to systemcontroller");

    /*initialize system controller*/
    /*my info*/
    ICO_DBG("CicoHomeScreen::StartHomeScreen: get package name %s",hs_package_name);
    hs_app_info = GetAppInfo(hs_package_name);

    /*start home statusbar and onscreen*/
    StartRelations();
    ICO_DBG("CicoHomeScreen::StartHomeScreen: start relation apps");

    /* Get screen size */
    CicoHSWindowController::GetFullScreenSize(orientation,
                                              &full_width,&full_height);

    ICO_DBG("full screen size x=%d y=%d",full_width,full_height);

    /* MenuScreen Window Size */
    menu_width = full_width;
    menu_height = full_height - ICO_HS_STATUSBAR_WINDOW_HEIGHT -
                  ICO_HS_CONTROLBAR_WINDOW_HEIGHT;

    /* BackGround Window Size */
    bg_width  = full_width;
    bg_height = full_height;

    /* Control Bar Window Size */
    ctl_bar_width  = full_width;
    ctl_bar_height = ICO_HS_CONTROLBAR_WINDOW_HEIGHT;

    // load switch display zone config
    moveZoneName = config->ConfigGetString("switchzone", "movezone", NULL);

    moveZoneAnimation.name = (char*)config->ConfigGetString("switchzone",
                                                            "animation",
                                                            ICO_HS_ANIMATION_FADE);

    moveZoneAnimation.time = config->ConfigGetInteger("switchzone",
                                                      "animatime", 400);

    // debug log
    ICO_DBG("moveZoneName=%s animation=%s time=%d",
            moveZoneName, moveZoneAnimation.name, moveZoneAnimation.time);

    // AUL Listen Signal set(launch/dead)
    initAulListenXSignal();

    // application history class init. before call launchApps
    InitializeAppHistory();

    /* application history launch */
    t_launcApps_data* x = new t_launcApps_data;
    CicoHSCmdOpts* cmdOpts = CicoHSCmdOpts::getInstance();
    x->filePath  = cmdOpts->getHistoryFilePath();
    x->filePathD = cmdOpts->getDefaultHistoryFilePath();
    x->hs        = this;
    ecore_timer_add(0.01, launchApps, x);

    /*Create window*/
    CreateControlBarWindow();

    /*Create window*/
    CreateBackWindow();

    /*Create window*/
    CreateMenuWindow();

    /*Create SwipeInput window*/
    CicoHSSwipeTouch::Initialize(hs_instance->ctl_bar_window, hs_instance->m_appHis,
                                 full_width, full_height);
    CreateSwipeInputWindow();

    /*Show Home Screen*/
    ShowHomeScreenLayer();

    ICO_TRA("CicoHomeScreen::StartHomeScreen Leave");
    return 0;
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
    int idx;

    if (hs_instance->menu_window == NULL)   {
        return;
    }

    if (hs_instance->GetMode() == ICO_HS_MODE_MENU) {
        ico_syc_animation_t animation;
        ICO_DBG("CicoHomeScreen::ChangeMode: MENU->APP");
        if (pattern == ICO_HS_SHOW_HIDE_PATTERN_SLIDE)  {
            animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_SLIDE;
        }
        else    {
            animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_FADE;
        }
        animation.time = ICO_HS_MENU_ANIMATION_DURATION;
        hs_instance->menu_window->Hide(&animation);
        for (idx = 0; idx < hs_instance->num_swipe_input_windows; idx++)    {
            hs_instance->swipe_input_windows[idx]->Show();
        }
        hs_instance->SetMode(ICO_HS_MODE_APPLICATION);
        CicoHSAppInfo *appinfo = hs_instance->GetActiveAppInfo();
        if (NULL != appinfo) {
            ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
        }
    }
    else if (hs_instance->GetMode() ==ICO_HS_MODE_APPLICATION)  {
        ico_syc_animation_t animation;
        ICO_DBG("CicoHomeScreen::ChangeMode: APP->MENU");
        if (pattern == ICO_HS_SHOW_HIDE_PATTERN_SLIDE)  {
            animation.name = (char*)ICO_HS_MENU_SHOW_ANIMATION_SLIDE;
        }
        else    {
            animation.name = (char*)ICO_HS_MENU_SHOW_ANIMATION_FADE;
        }
        animation.time = ICO_HS_MENU_ANIMATION_DURATION;
        hs_instance->menu_window->Show(&animation);
        for (idx = 0; idx < hs_instance->num_swipe_input_windows; idx++)    {
            hs_instance->swipe_input_windows[idx]->Hide();
        }
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::setActiveApp
 *          active(touch operate app) notice
 *
 * @param[in]   appid
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::setActiveApp(const char* appid)
{
    if ((NULL == appid) || (0 == appid) || (0 == strlen(appid))) {
        return;
    }
    if (NULL != m_appHis) {
        m_appHis->activeApp(appid);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::SetNightMode
 *          set night mode theme
 *
 * @param[in]   data    user data
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::SetNightMode(void* data)
{
    ICO_TRA("CicoHomeScreen::SetNightMode Enter");
   // night mode action
    hs_instance->ctl_bar_window->SetNightMode();
    hs_instance->menu_window->SetNightMode();
    ICO_TRA("CicoHomeScreen::SetNightMode Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::SetRegulation
 *          regulation action and set regualtion theme
 *
 * @param[in]   data    user data
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::SetRegulation(void* data)
{
    ICO_TRA("CicoHomeScreen::SetRegulation Enter");
    // window control
    hs_instance->controlRegulation();
    // regulation action
    hs_instance->ctl_bar_window->SetRegulation();
    ico_syc_animation_t animation;
    animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_SLIDE;
    animation.time = ICO_HS_MENU_ANIMATION_DURATION;
    hs_instance->menu_window->Hide(&animation);

    // force focus change
    if (false == CicoHSSystemState::getInstance()->getRegulation()) {
        CicoHSAppInfo *active_appinfo = hs_instance->GetActiveAppInfo();
        if (NULL != active_appinfo) {
            ico_syc_change_active(active_appinfo->GetAppId(),
                                  active_appinfo->GetLastSurface());
            ico_syc_change_active(active_appinfo->GetAppId(), 0);
        }
    }
    else {
        if (NULL != hs_instance->ctl_bar_window) {
            const char *ctl_bar_appid = hs_instance->ctl_bar_window->GetAppId();
            int ctl_bar_surface = hs_instance->ctl_bar_window->GetSurfaceId();
            ico_syc_change_active(ctl_bar_appid, ctl_bar_surface);
            ico_syc_change_active(ctl_bar_appid, 0);
        }
    }

    ICO_TRA("CicoHomeScreen::SetRegulation Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::RenewAppInfoList_i
 *          renewal application info list
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::RenewAppInfoList_i(void)
{
    int ii = 0;
    int kk = 0;
    CicoHSAppInfo *tmp_apps_info[ICO_HS_MAX_APP_NUM];
    int tmp_application_num = 0;
    const char *appid_p = 0;

    for(ii = 0; ii < ICO_HS_MAX_APP_NUM; ii++){
        tmp_apps_info[ii] = NULL;
    }

    /* get APP information*/
    std::vector<CicoAilItems> aillist = life_cycle_controller->getAilList();

    /*create instance*/
    for ( ii = 0;
         (ii < (int)aillist.size()) && (ii < ICO_HS_MAX_APP_NUM);
         ii++)  {
        ICO_DBG("aillist[%d].m_appid.c_str() = %s",
                 ii, aillist[ii].m_appid.c_str());

        for (kk = 0; kk < ICO_HS_MAX_APP_NUM; kk++) {
            if (apps_info[kk] == NULL) {
                continue;
            }

            if (strcmp(aillist[ii].m_appid.c_str(),
                apps_info[kk]->GetAppId()) == 0) {

                tmp_apps_info[ii] = apps_info[kk];
                ICO_DBG("shift aillist[%d].m_appid.c_str() = %s",
                         ii, aillist[ii].m_appid.c_str());
                break;
            }
        }
        if (kk == ICO_HS_MAX_APP_NUM) {
            tmp_apps_info[ii] = new CicoHSAppInfo(aillist[ii].m_appid.c_str());
            ICO_DBG("create aillist[%d].m_appid.c_str() = %s",
                     ii, aillist[ii].m_appid.c_str());
        }
        tmp_application_num++;
    }

    /* delete instance */
    for (ii = 0; ii < ICO_HS_MAX_APP_NUM; ii++) {
        if (apps_info[ii] == NULL) {
            continue;
        }
        appid_p = apps_info[ii]->GetAppId();
        for ( kk = 0; kk < tmp_application_num; kk++) {
            if (strcmp(appid_p, tmp_apps_info[kk]->GetAppId()) == 0) {
                break;
            }
        }
        if (kk == tmp_application_num) {
            delete apps_info[ii];
            ICO_DBG("delete apps_info[%d]->GetAppId() = %s",
                    ii, appid_p);
        }
    }

    /* set apps info */
    for ( ii = 0; ii < ICO_HS_MAX_APP_NUM; ii++) {
        apps_info[ii] = tmp_apps_info[ii];
    }

    application_num = tmp_application_num;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::RenewAppInfoList
 *          renewal appinfolist
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::RenewAppInfoList(void)
{
    ICO_TRA("CicoHomeScreen::RenewAppInfoList Enter");
    hs_instance->RenewAppInfoList_i();
    ICO_TRA("CicoHomeScreen::RenewAppInfoList Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::startupCheckAdd
 *          start-up application infomation add
 *
 * @param[in]  pid
 * @param[in]  appid
 * @return     none
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::startupCheckAdd(int pid, const std::string& appid,
                                     bool bSubDisp)
{
    if (NULL != m_appHis) {
        m_appHis->startupCheckAdd(pid, appid, bSubDisp);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::startupCheck
 *          start-up application check
 *
 * @param[in]  appid check target
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::startupCheck(const char* appid)
{
    ICO_TRA("CicoHomeScreen::startupCheck Enter");
    if (NULL == life_cycle_controller) {
        ICO_ERR("life_cycle_controller is null");
        ICO_TRA("CicoHomeScreen::startupCheck Leave");
        return;
    }
    if (NULL == m_appHis) {
        ICO_ERR("m_appHis is null");
        ICO_TRA("CicoHomeScreen::startupCheck Leave");
        return;
    }
    if (false == m_appHis->isStartupChecking()) {
        ICO_TRA("CicoHomeScreen::startupCheck Leave");
        return;
    }
    m_appHis->update_appid();
    m_appHis->startupEntryFinish(appid);
    if (false == m_appHis->isFinish()) {
        ICO_TRA("CicoHomeScreen::startupCheck Leave");
        return;
    }
    finishStartup();
    ICO_TRA("CicoHomeScreen::startupCheck Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::finishStartup
 *          start-up application check
 *
 * @param[in]  appid check target
 */
/*--------------------------------------------------------------------------*/

void CicoHomeScreen::finishStartup(void)
{
    ICO_TRA("CicoHomeScreen::finishStartup Enter");

    string last = m_appHis->getLastStartupAppid();
    const string& subDisp = m_appHis->getSubDispAppid();

    list<string> h = m_appHis->getAppHistory();
    // last appid is empty then get new last appid
    if (true == last.empty()) {
        list<string>::iterator it_h = h.begin();
        while (it_h != h.end()) {
            if (0 != subDisp.compare(*it_h)) {
                last = *it_h;
            }
            ++it_h;
        }
    }

    list<string>::reverse_iterator rit_h = h.rbegin();
    while(rit_h != h.rend()) {
        CicoHSAppInfo *ai = GetAppInfo((*rit_h).c_str());
        if (ai == NULL) {
            ++rit_h;
            continue;
        }
        ICO_DBG("Raise %s", (*rit_h).c_str());
        {
            const char* appid = ai->GetAppId();
            int surface = ai->GetLastSurface();
            if ((0 != last.compare(appid)) && (0 != subDisp.compare(appid))) {
                ico_syc_show(appid, surface, NULL);
                ico_syc_hide(appid, surface, NULL);
            }
        }
        ++rit_h;
    }

    CicoHSAppInfo *ai = GetAppInfo(subDisp.c_str());
    if (ai != NULL) {
        requestChangeZone(ai);
    }

    ai = GetAppInfo(last.c_str());
    if (ai != NULL) {
        const char* appid = ai->GetAppId();
        int surface = ai->GetLastSurface();
        ico_syc_show(appid, surface, NULL);
        ico_syc_change_active(appid, surface);
        hs_instance->SetActiveAppInfo(appid);
    }

    m_appHis->stopStartupCheck();
    ICO_TRA("CicoHomeScreen::finishStartup Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::readStartupApp
 *          read start-up application data
 *
 * @param[in]  appid check target
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::readStartupApp(std::vector<pairAppidSubd>& apps)
{
    ICO_TRA("CicoHomeScreen::readStartupApp Enter");
    if (NULL == m_appHis) {
        apps.clear();
    }
    else {
        m_appHis->readAppHistory(apps);
    }
    ICO_TRA("CicoHomeScreen::readStartupApp Leave(%d)", (int)apps.size());
    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::requestHideAppid
 *          request Hide appid
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::requestHideAppid(const std::string& app)
{
    const char* appid = app.c_str();
    ICO_TRA("start %s", appid);
    CicoHSAppInfo *ai = GetAppInfo(appid);
    if (NULL == ai) {
        ICO_DBG("end %d", appid);
        return;
    }
    int surface = ai->GetLastSurface();
    ico_syc_hide(appid, surface, NULL);
    ICO_TRA("end");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::requestShowAppid
 *          request Show appid
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::requestShowAppid(const std::string& app)
{
    const char* appid = app.c_str();
    ICO_TRA("start %s", appid);
    CicoHSAppInfo *ai = GetAppInfo(appid);
    if (NULL == ai) {
        ICO_DBG("end %d", appid);
        return;
    }
    int surface = ai->GetLastSurface();
    ico_syc_show(appid, surface, NULL);
    ICO_TRA("end");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::requestActivationAppid
 *          request Activation
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::requestActivationAppid(const std::string& app)
{
    const char* appid = app.c_str();
    ICO_TRA("CicoHomeScreen::requestActivationAppid Enter(%s)", appid);
    CicoHSAppInfo *ai = GetAppInfo(appid);
    if (NULL == ai) {
        ICO_DBG("end %d", appid);
        return;
    }
    int surface = ai->GetLastSurface();
    hs_instance->SetActiveAppInfo(app.c_str());
    ico_syc_show(appid, surface, NULL);
    ico_syc_change_active(appid, surface);
    ICO_TRA("CicoHomeScreen::requestActivationAppid Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::controlRegulation
 *          controlRegulation
 *
 * @param  reqStt true / false
 */
/*--------------------------------------------------------------------------*/
void CicoHomeScreen::controlRegulation(bool regStt)
{
    ICO_TRA("CicoHomeScreen::controlRegulation Enter(%s)",
            regStt? "true": "false");
    if (NULL == m_appHis) {
        ICO_ERR("m_appHis is null");
        ICO_TRA("CicoHomeScreen::controlRegulation Leave");
        return;
    }
    const string& wapp = m_appHis->getSelectApp(); // select appid get
    if (false == wapp.empty()) { // select appid nothing
        const string& napp = m_appHis->getNearHistory(); // history top appid get
        if (0 != wapp.compare(napp)) { // history top, wait app is Eqale
            requestHideAppid(napp);
            requestActivationAppid(wapp);
            m_appHis->moveHistoryHead(wapp);
        }
        m_appHis->clearSelectApp(); // select appid clear
        m_appHis->homeSwipe();
    }
    ICO_TRA("CicoHomeScreen::controlRegulation Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ActivationUpdate
 *          history update by swipe app
 *
 * @return bool
 * @retval true history update
 * @retval false no history updath
 */
/*--------------------------------------------------------------------------*/
bool
CicoHomeScreen::ActivationUpdate(void)
{
    return hs_instance->ActivationUpdate_i();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ActivationUpdate_i
 *          history update by swipe app
 *
 * @return bool
 * @retval true history update
 * @retval false no history updath
 */
/*--------------------------------------------------------------------------*/
bool
CicoHomeScreen::ActivationUpdate_i(void)
{
    ICO_TRA("CicoHomeScreen::ActivationUpdate_i Enter");
    if (NULL == m_appHis) {
        ICO_ERR("m_appHis is null");
        ICO_TRA("CicoHomeScreen::ActivationUpdate_i Leave(false)");
        return false;
    }
    bool bR = false;
    const string& wapp = m_appHis->getSelectApp(); // select appid get
    if (false == wapp.empty()) { // select appid nothing
        const string& napp = m_appHis->getNearHistory(); // history top appid get
        if (0 != wapp.compare(napp)) { // history top, wait app is Eqale
            requestActivationAppid(wapp);
            m_appHis->moveHistoryHead(wapp);
            bR = true;
        }
        m_appHis->clearSelectApp(); // select appid clear
        m_appHis->homeSwipe();
    }
    ICO_TRA("CicoHomeScreen::ActivationUpdate_i Leave(%s)",
            bR? "true": "false");
    return bR;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowApp_i
 *          show app
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowApp_i(const std::string& app)
{
    CicoHSAppInfo *appinfo = GetAppInfo(app.c_str());
    if (appinfo == NULL)    {
        ICO_TRA("end get appinfo is NULL");
        return;
    }

    ico_syc_animation_t animation;
    animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_FADE;
    animation.time = ICO_HS_MENU_ANIMATION_DURATION;

    ico_syc_show(appinfo->GetAppId(), appinfo->GetLastSurface(), &animation);
    ico_syc_change_active(appinfo->GetAppId(), appinfo->GetLastSurface());
    hs_instance->SetActiveAppInfo(appinfo->GetAppId());
    //raise
//    RaiseApplicationWindow(appinfo->GetAppId(),
//                           appinfo->GetLastSurface());
    ICO_DBG("raise app %s", app.c_str());
    m_appHis->moveHistoryHead(app);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::ShowApp
 *          show app
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::ShowApp(const std::string& app)
{
    hs_instance->ShowApp_i(app);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::HideAppid
 *          hide app
 *
 * @param  app appid
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::HideApp(const std::string& app)
{
    const char* appid = app.c_str();
    ICO_TRA("start %s", appid);
    CicoHSAppInfo *ai = GetAppInfo(appid);
    if (NULL == ai) {
        ICO_DBG("end %d", appid);
        return;
    }

    ico_syc_animation_t animation;
    animation.name = (char*)ICO_HS_MENU_HIDE_ANIMATION_FADE;
    animation.time = ICO_HS_MENU_ANIMATION_DURATION;

    int surface = ai->GetLastSurface();
    ico_syc_hide(appid, surface, &animation);
    ICO_TRA("end");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreen::MoveApp
 *          move app
 *
 * @param  app appid
 * @param  zone zone 
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreen::MoveApp(const std::string& app, const std::string& zone)
{
    CicoHSAppInfo *appinfo = GetAppInfo((const char*)app.c_str());
    if ((CicoHSAppInfo*)0 == appinfo) {
        ICO_DBG("GetAppInfo(%s) is NULL" , app.c_str());
        return;
    }

    int surface = appinfo->GetLastSurface();
    ico_hs_window_info* wininfo = appinfo->GetWindowInfobySurface(surface);
    int layer = HS_LAYER_APPLICATION;
    const char *dispzone = (const char*)zone.c_str();
    ico_syc_win_move_t move = {
        .zone   = (char*)dispzone,
        .layer  = layer,
        .pos_x  = 0,
        .pos_y  = 0,
        .width  = 0,
        .height = 0
    };

    ico_syc_move(wininfo->appid, wininfo->surface, &move,
                 &hs_instance->moveZoneAnimation);
}
// vim: set expandtab ts=4 sw=4:
