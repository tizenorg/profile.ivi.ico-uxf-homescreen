/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application control class
 *
 * @date    Dec-18-2013
 */

#include <string>
#include "CicoHSAppControl.h"
#include "CicoHomeScreen.h"
#include "CicoHSCommand.h"
#include "CicoHSLifeCycleController.h"

#include <ico_log.h>
#include "ico_syc_msg_cmd_def.h"
using namespace std;

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSAppControl *CicoHSAppControl::ms_myInstance = NULL;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::CicoHSAppControl
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSAppControl::CicoHSAppControl(void)
{
    ICO_TRA("CicoHSAppControl::CicoHSAppControl Enter");

    ICO_TRA("CicoHSAppControl::CicoHSAppControl Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::~CicoHSMenuWindo
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSAppControl::~CicoHSAppControl(void)
{
    /* Do not somthing to do */
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoHSAppControl::getInstance
 *          Get instance of CicoHSAppControl
 *
 * @param   none
 * @return  pointer of CicoHSAppControl object
 */
//--------------------------------------------------------------------------
CicoHSAppControl*
CicoHSAppControl::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoHSAppControl();
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 * @brief  CicoHSAppControl::handleCommand
 *         Handle command
 *
 * @param  [in] cmd            control command
 * @return none
 */
//--------------------------------------------------------------------------
void
CicoHSAppControl::handleCommand(const CicoHSCommand * cmd)
{
    ICO_TRA("CicoHSAppControl::handleCommand Enter(%d)", cmd->cmdid);

    int ret = -1;
    CicoHSCmdAppCtlOpt *opt = static_cast<CicoHSCmdAppCtlOpt*>(cmd->opt);
    if((opt->arg).empty() == true) {
        ICO_WRN("arg parameter is not set up");
        return;
    }
    vector<CicoHSCommandArg>::iterator it = opt->arg.begin();

    switch (cmd->cmdid) {
    case MSG_CMD_APP_START:
        // start application
        ExecuteApp(it->m_appid);
        break;
    case MSG_CMD_APP_STOP:
        if ((it->m_appid).empty() == true) {
            ret =  GetAppId(it->m_pid, it->m_appid);
            if (ret != 0) {
                break;
            }
        }
        // stop application
        TerminateApp(it->m_appid);
        break;
    case MSG_CMD_WIN_CHANGE:
        WinChgControl(opt->arg);
        break;
    default:
        ICO_WRN("Unknown Command(0x%08x)", cmd->cmdid);
        break;
    }
    ICO_TRA("CicoHSAppControl::handleCommand Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::GetAppId
 *          get application id
 *
 * @param[in]   pid   pid
 * @return      appid none
 */
/*--------------------------------------------------------------------------*/
int
CicoHSAppControl::GetAppId(int pid, string& appid)
{
    ICO_TRA("CicoHSAppControl::GetAppId Enter");

    if (pid == -1 ) {
        ICO_WRN("No pid");
        return -1;
    }

    const CicoAulItems* aul = CicoHSLifeCycleController::getInstance()->findAUL(pid);
    if ((NULL == aul) || (0 == aul)) {
        ICO_TRA("aul items not find");
        return -1;
    }
    appid = aul->m_appid;
    return 0;

    ICO_TRA("CicoHSAppControl::GetAppId Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::WinChangeApp
 *          change application window
 *
 * @param[in]   appid   application id
 * @param[in]   zone    zone
 * @param[in]   visible visible val
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::WinChangeApp(const string& appid, const string& zone, int visible)
{
    ICO_TRA("CicoHSAppControl::WinChangeApp Enter");

    if (visible == 1) {
        MoveApp(appid, zone);
        ShowApp(appid);
    }
    else if (visible == 0) {
        MoveApp(appid, zone);
        HideApp(appid);
    }
    else {
        ICO_ERR("Unknown visible(%d)", visible);
    }

    ICO_TRA("CicoHSAppControl::WinChangeApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::MoveApp
 *          move application
 *
 * @param[in]   appid   application id
 * @param[in]   zone    zone
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::MoveApp(const string& appid, const string& zone)
{
    ICO_TRA("CicoHSAppControl::MoveApp Enter");

    if(appid.empty() == true) {
        ICO_WRN("No appid");
        return;
    }

    if(zone.empty() == true) {
        ICO_WRN("No zone");
        return;
    }

    CicoHomeScreen::MoveApp(appid, zone);

    ICO_TRA("CicoHSAppControl::MoveApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::ShowApp
 *          show application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::ShowApp(const string& appid)
{
    ICO_TRA("CicoHSAppControl::ShowApp Enter");

    if(appid.empty() == true) {
        ICO_WRN("No appid");
        return;
    }

    CicoHomeScreen::ShowApp(appid);

    ICO_TRA("CicoHSAppControl::ShowApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::HideApp
 *          hide application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::HideApp(const string& appid)
{
    ICO_TRA("CicoHSAppControl::HideApp Enter");

    if(appid.empty() == true) {
        ICO_WRN("No appid");
        return;
    }

    CicoHomeScreen::HideApp(appid);

    ICO_TRA("CicoHSAppControl::HideApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::ExecuteApp
 *          execute application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::ExecuteApp(const string& appid)
{
    ICO_TRA("CicoHSAppControl::ExecuteApp Enter");

    if(appid.empty() == true) {
        ICO_WRN("No appid");
        return;
    }

    CicoHomeScreen::ExecuteApp(appid.c_str());

    ICO_TRA("CicoHSAppControl::ExecuteApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::TerminateApp
 *          teminate application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppControl::TerminateApp(const string& appid)
{
    ICO_TRA("CicoHSAppControl::TerminateApp Enter");

    if(appid.empty() == true) {
        ICO_WRN("No appid");
        return;
    }

    CicoHomeScreen::TerminateApp(appid.c_str());

    ICO_TRA("CicoHSAppControl::TerminateApp Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppControl::WinChgControl
 *          WIN_CHANGE command exec.
 *
 * @param[in]   arg  appid,zone,visible data
 * @return      bool
 * @retval      true: success
 * @retval      false: fail
 */
/*--------------------------------------------------------------------------*/
bool
CicoHSAppControl::WinChgControl(vector<CicoHSCommandArg>& arg)
{
    ICO_TRA("start WinChgControl(%d)", arg.size());
    // Check show control and appid get
    bool bShow = false;
    vector<CicoHSCommandArg>::iterator it = arg.begin();
    while (it != arg.end()) {
        if (true == (it->m_appid).empty()) {
            if (0 != GetAppId(it->m_pid, it->m_appid)) {
                ++it;
                continue; // continue of while
            }
            ICO_DBG("%d -> %s getappid", it->m_pid, (it->m_appid).c_str());
        }
        if (1 == it->m_visible) {
            bShow = true;
        }
        ++it;
    }
    if (false == bShow) {
        ICO_TRA("end Nothing show");
        return false;
    }
    // Hide set
    it = arg.begin();
    while ( it != arg.end() ) {
        if ((0 == it->m_visible) && (false == (it->m_appid).empty())) {
            WinChangeApp(it->m_appid, it->m_zone, it->m_visible);
        }
        ++it;
    }
    // Show set
    string lastShowApp;
    it = arg.begin();
    while ( it != arg.end() ) {
        if ((1 == it->m_visible) && (false == (it->m_appid).empty())) {
            WinChangeApp(it->m_appid, it->m_zone, it->m_visible);
            lastShowApp = it->m_appid;
        }
        ++it;
    }
    if (false == lastShowApp.empty()) {
        CicoHomeScreen::ActivationUpdate();
    }
    ICO_TRA("end");
    return true;
}

// vim: set expandtab ts=4 sw=4:
