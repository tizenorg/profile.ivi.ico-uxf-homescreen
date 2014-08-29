/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoHSAppInfo
 *
 *  @brief  This file is implimention of CicoHSAppInfo class
 */
//==========================================================================

#include "CicoHSAppInfo.h"
#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoHSLifeCycleController.h"
#include "CicoHSWindow.h"

//==========================================================================
//  public functions
//==========================================================================
//--------------------------------------------------------------------------
/**
 *  @brief  constractor
 *
 *  @param [in] appid   application id
 */
//--------------------------------------------------------------------------
CicoHSAppInfo::CicoHSAppInfo(const char *appid)
    : m_appid(appid), m_category(0), m_window_num(0), m_last_surface(0)
{
    memset(m_window_info_i, 0, sizeof(m_window_info_i));
    m_life_cycle_controller = CicoHSLifeCycleController::getInstance();
}

//--------------------------------------------------------------------------
/**
 *  @brief   destractor
 */
//--------------------------------------------------------------------------
CicoHSAppInfo::~CicoHSAppInfo(void)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window information
 *
 *  @param [in] wininfo     window information
 *  @return OK or ERROR
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::AddWindowInfo(ico_syc_win_info_t *wininfo)
{
    ico_syc_win_info_t  wk_wininfo;

    if (NULL == wininfo) {
        ICO_ERR("wininfo is null.");
        return ICO_ERROR;
    }

    ICO_TRA("CicoHSAppInfo::AddWindowInfo Enter(appid=%s surface=%x)",
            wininfo->appid, wininfo->surface);

    // if exist window information, update current window information
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = this->m_window_info.begin();
    for (; itr != this->m_window_info.end(); ++itr) {
        if (wininfo->surface == (*itr)->surface) {
            /*update*/
            SetWindowInfo(*itr, wininfo);
            ICO_TRA("CicoHSAppInfo::AddWindowInfo Leave(OK)");
            return ICO_OK;
        }
    }

    // if not exit window information, create window information
    ico_hs_window_info *tmp_win_info = GetFreeWindowInfoBuffer();
    if (tmp_win_info == NULL) {
        ICO_ERR("CicoHSAppInfo::AddWindowInfo Leave(ERR)");
        return ICO_ERROR;
    }
    memcpy(&wk_wininfo, wininfo, sizeof(wk_wininfo));
    if ((wk_wininfo.name == NULL) || (*wk_wininfo.name == 0))   {
        wk_wininfo.name = (char *)CicoHSWindow::getWindowName(m_window_num);
        ICO_TRA("CicoHSAppInfo::AddWindowInfo surface=%d idx=%d name=<%s>",
                wk_wininfo.surface, m_window_num,
                wk_wininfo.name ? wk_wininfo.name : "(null)");
    }
    SetWindowInfo(tmp_win_info, &wk_wininfo);
    this->m_window_info.push_back(tmp_win_info);
    ++ m_window_num;

    ICO_TRA("CicoHSAppInfo::AddWindowInfo Leave(OK)");
    return ICO_OK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set window attribute
 *
 *  @param [in]   winattr   window attribute
 *  @return OK or ERROR
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::AddWindowAttr(ico_syc_win_attr_t *winattr)
{
    ico_syc_win_attr_t  wk_winattr;

    if (NULL == winattr) {
        ICO_ERR("winattr is null.");
        return ICO_ERROR;
    }

    ICO_TRA("CicoHSAppInfo::AddWindowAttr Enter(appid=%s surface=%x",
            winattr->appid, winattr->surface);

    // if exist window information, update current window information
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = m_window_info.begin();
    for (; itr != m_window_info.end(); itr++){
        if (winattr->surface == (*itr)->surface) {
            /*update*/
            SetWindowAttr(*itr, winattr);
            ICO_TRA("CicoHSAppInfo::AddWindowAttr Leave(OK)");
            return ICO_OK;
        }
    }

    // if not exit window information, create window information
    ico_hs_window_info *tmp_win_info = GetFreeWindowInfoBuffer();
    if (tmp_win_info == NULL){
        ICO_TRA("CicoHSAppInfo::AddWindowAttr Leave(ERR)");
        return ICO_ERROR;
    }
    memcpy(&wk_winattr, winattr, sizeof(wk_winattr));
    if ((wk_winattr.name == NULL) || (*wk_winattr.name == 0))   {
        wk_winattr.name = (char *)CicoHSWindow::getWindowName(m_window_num);
        ICO_TRA("CicoHSAppInfo::AddWindowAttr surface=%d idx=%d name=<%s>",
                wk_winattr.surface, m_window_num,
                wk_winattr.name ? wk_winattr.name : "(null)");
    }

    SetWindowAttr(tmp_win_info, &wk_winattr);
    m_window_info.push_back(tmp_win_info);
    ++ m_window_num;

    ICO_TRA("CicoHSAppInfo::AddWindowAttr Leave(OK)");
    return ICO_OK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  free window information
 *
 *  @param [in] name    window name
 */
//--------------------------------------------------------------------------
void
CicoHSAppInfo::FreeWindowInfo(const char *name)
{
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = m_window_info.begin();
    for (; itr != m_window_info.end(); ++itr) {
        if ((0 == strncmp(name,(*itr)->name, ICO_HS_MAX_WINDOW_NAME)) &&
            ((*itr)->valid))    {
            ICO_DBG("CicoHSAppInfo::FreeWindowInfo(name=%s)", name ? name : "(NULL)");
            if ((*itr)->surface == m_last_surface)  {
                m_last_surface = 0;
            }
            (*itr)->valid = false;
            m_window_info.erase(itr);
            -- m_window_num;
            break;
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  free window information
 *
 *  @param [in] surface surface id
 */
//--------------------------------------------------------------------------
void
CicoHSAppInfo::FreeWindowInfo(int surface)
{
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = m_window_info.begin();
    for (; itr != m_window_info.end(); ++itr) {
        if (((*itr)->surface == surface) && ((*itr)->valid))    {
            ICO_DBG("CicoHSAppInfo::FreeWindowInfo(surface=%08x)", surface);
            if ((*itr)->surface == m_last_surface)  {
                m_last_surface = 0;
            }
            (*itr)->valid = false;
            m_window_info.erase(itr);
            -- m_window_num;
            break;
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application id
 *
 *  @return application id
 */
//--------------------------------------------------------------------------
const char*
CicoHSAppInfo::GetAppId(void)
{
    return m_appid.c_str();
}

//--------------------------------------------------------------------------
/**
 *  @brief  get number of window
 *
 *  @return number of window
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::GetWindowNum(void)
{
    return m_window_num;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get window information
 *
 *  @param[in]   name   window name
 *  @return window information
 */
//--------------------------------------------------------------------------
ico_hs_window_info*
CicoHSAppInfo::GetWindowInfo(const char* name)
{
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = m_window_info.begin();
    for (; itr != m_window_info.end(); ++itr) {
        if((0 == strncmp(name,(*itr)->name, ICO_HS_MAX_WINDOW_NAME)) &&
           ((*itr)->valid)) {
            return *itr;
        }
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetWindowInfo
 *          get window information
 *
 * @param[in]   idx index(0-n)
 * @return      window information
 */
/*--------------------------------------------------------------------------*/
ico_hs_window_info*
CicoHSAppInfo::GetWindowInfo(int idx)
{
    int     i = 0;

    for (std::vector<ico_hs_window_info*>::iterator it_window_info
                = m_window_info.begin();
         it_window_info != m_window_info.end();
         it_window_info++) {
        if (! (*it_window_info)->valid) continue;
        if (i >= idx)   {
            return *it_window_info;
        }
        i ++;
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get window information
 *
 *  @param[in]   surface    surface id
 *  @return window information
 */
//--------------------------------------------------------------------------
ico_hs_window_info*
CicoHSAppInfo::GetWindowInfobySurface(int surface)
{
    std::vector<ico_hs_window_info*>::iterator itr;
    itr = m_window_info.begin();
    for (; itr != m_window_info.end(); ++itr) {
        if (((*itr)->surface == surface) &&
            ((*itr)->valid))    {
            return *itr;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   launch application
 *
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::Execute(void)
{
    // call launch api
    int ret = m_life_cycle_controller->launch(m_appid);
    if (ret < 0) {
        return ret;
    }
    return ret;
}

//--------------------------------------------------------------------------
/*
 *  @brief  terminate application
 *
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::Terminate(void)
{
    // call terminate api
    int ret = m_life_cycle_controller->terminate(m_appid);
    if(ret < 0){
        return ret;
    }
    return ret;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get status
 *
 *  @return application running status
 *  @retval true    running
 *  @retval false   not running
 */
//--------------------------------------------------------------------------
bool
CicoHSAppInfo::GetStatus(void)
{
    return m_life_cycle_controller->isRunning(m_appid);
}

//--------------------------------------------------------------------------
/**
 *  @brief  set last surface id
 *
 *  @param [in] last surface id
 */
//--------------------------------------------------------------------------
void
CicoHSAppInfo::SetLastSurface(int last_surface)
{
    m_last_surface = last_surface;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get last surface id
 *
 *  @return surface id
 */
//--------------------------------------------------------------------------
int
CicoHSAppInfo::GetLastSurface(void)
{
    return m_last_surface;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get default zone name
 *
 *  @return zone name
 */
//--------------------------------------------------------------------------
const char *
CicoHSAppInfo::GetDefaultZone(void)
{
    m_life_cycle_controller = CicoHSLifeCycleController::getInstance();
    const CicoAilItems* ailItem = m_life_cycle_controller->findAIL(m_appid);

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCDisplayZoneConf *zoneConf = NULL;
    if (NULL != ailItem) {
        zoneConf = sysConf->findDisplayZoneConfbyId(ailItem->m_displayZone);
    }
    else {
        const CicoSCDefaultConf* defConf = sysConf->getDefaultConf();
        zoneConf = sysConf->findDisplayZoneConfbyId(defConf->displayzone);
    }

    if (NULL == zoneConf) {
        return NULL;
    }
    return zoneConf->fullname.c_str();
}

//==========================================================================
//  private method
//==========================================================================
//--------------------------------------------------------------------------
/**
 *  @brief   set window information
 *
 *  @param [in] hs_wininfo  window information
 *  @param [in] wininfo     window information from system-controller
 */
//--------------------------------------------------------------------------
void
CicoHSAppInfo::SetWindowInfo(ico_hs_window_info *hs_wininfo,
                             ico_syc_win_info_t *wininfo)
{
    ICO_TRA("CicoHSAppInfo::SetWindowInfo Enter"
            "(winifo=%p appid=%s winname=%s surface=%x)",
            hs_wininfo,
            wininfo->appid, wininfo->name, wininfo->surface);

    // set application id
    if(wininfo->appid != NULL){
        strncpy(hs_wininfo->appid, wininfo->appid, ICO_HS_MAX_PROCESS_NAME);
    }

    // set window name(window title)
    if((wininfo->name != NULL) && (*wininfo->name != 0)) {
        strncpy(hs_wininfo->name, wininfo->name, ICO_HS_MAX_WINDOW_NAME);
    }
    // set surface id
    hs_wininfo->surface = wininfo->surface;

    ICO_TRA("CicoHSAppInfo::SetWindowInfo Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window attribute
 *
 *  @param [in] hs_wininfo  window information
 *  @param [in] winattr     window attribute
 */
//--------------------------------------------------------------------------
void
CicoHSAppInfo::SetWindowAttr(ico_hs_window_info *hs_wininfo,
                             ico_syc_win_attr_t *winattr)
{
    ICO_TRA("CicoHSAppInfo::SetWindowAttr Enter"
            "(winifo=%p "
            "appid=%s winname=%s zone=%s surface=%x nodeid=%d layer=%d "
            "x/y=%d/%d w/h=%d/%d raise=%d visible=%d active=%d)",
            hs_wininfo, winattr->appid,
            ((winattr->name != NULL) && (*winattr->name != 0)) ?
                winattr->name : hs_wininfo->name,
            winattr->zone, winattr->surface, winattr->nodeid, winattr->layer,
            winattr->pos_x, winattr->pos_y, winattr->width, winattr->height,
            winattr->raise, winattr->visible, winattr->active);

    if (winattr->appid != NULL) {
        strncpy(hs_wininfo->appid, winattr->appid, ICO_HS_MAX_PROCESS_NAME);
    }

    if ((winattr->name != NULL) && (*winattr->name != 0)) {
        strncpy(hs_wininfo->name, winattr->name, ICO_HS_MAX_WINDOW_NAME);
    }

    if (winattr->zone != NULL) {
        strncpy(hs_wininfo->zone, winattr->zone, ICO_HS_MAX_ZONE_NAME);
    }

    hs_wininfo->surface = winattr->surface;
    hs_wininfo->nodeid  = winattr->nodeid;
    hs_wininfo->layer   = winattr->layer;
    hs_wininfo->pos_x   = winattr->pos_x;
    hs_wininfo->pos_y   = winattr->pos_y;
    hs_wininfo->width   = winattr->width;
    hs_wininfo->height  = winattr->height;
    hs_wininfo->raise   = winattr->raise;
    hs_wininfo->visible = winattr->visible;
    hs_wininfo->active  = winattr->active;

    ICO_TRA("CicoHSAppInfo::SetWindowAttr Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoHSAppInfo::GetFreeWindowInfoBuffer
 *          get internal window info buffer
 *
 * @param[in]   none
 * @return      buffer
 */
//--------------------------------------------------------------------------
ico_hs_window_info *
CicoHSAppInfo::GetFreeWindowInfoBuffer(void)
{
    ico_hs_window_info *tmp_win_info;
    for (int ii = 0; ii < ICO_HS_APP_MAX_WINDOW_NUM; ii++) {
        if (m_window_info_i[ii].valid == false) {
            tmp_win_info = &m_window_info_i[ii];
            memset(tmp_win_info, 0, sizeof(ico_hs_window_info));
            tmp_win_info->valid = true;
            return tmp_win_info;
        }
    }
    return NULL;
}
// vim: set expandtab ts=4 sw=4:
