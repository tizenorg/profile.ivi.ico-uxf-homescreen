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
 *  @file   CicoSysConDaemon.cpp
 *
 *  @brief  This file is implimention of CicoSysConDaemon class
 */
//==========================================================================

#include <exception>
#include <iostream>
#include <string>

#include "CicoSysConDaemon.h"
#include "ico_syc_error.h"
#include "CicoLog.h"
#include "CicoSystemConfig.h"
#include "CicoSCServer.h"
#include "CicoSCWayland.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCUser.h"
#include "CicoSCUserManager.h"
#include "CicoSCResourceManager.h"
#include "Cico_aul_listen_app.h"
#include "CicoSCVInfoManager.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSysConDaemon::CicoSysConDaemon()
    : m_winctrl(NULL), m_inputctrl(NULL), m_resourcemgr(NULL)
{
//    ICO_TRA("CicoSysConDaemon::CicoSysConDaemon Enter");
//    ICO_TRA("CicoSysConDaemon::CicoSysConDaemon Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on create
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 *
 *  @return true on success, false on error
 */
//--------------------------------------------------------------------------
CicoSysConDaemon::~CicoSysConDaemon()
{
//    ICO_TRA("CicoSysConDaemon::~CicoSysConDaemon Enter");
//    ICO_TRA("CicoSysConDaemon::~CicoSysConDaemon Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on terminate
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
bool
CicoSysConDaemon::onCreate(void *user_data)
{
    ICO_TRA("CicoSysConDaemon::onCreate Enter");

    try {
        int ret = ICO_SYC_EOK;
        (void) CicoSystemConfig::getInstance();

        initAulListenXSignal();

        CicoSCVInfoManager::getInstance()->startup();

        CicoSCLifeCycleController *lifecycle =
            CicoSCLifeCycleController::getInstance();
        m_winctrl     = new CicoSCWindowController();
        m_inputctrl   = new CicoSCInputController();

        m_resourcemgr = new CicoSCResourceManager();
        m_resourcemgr->setWindowController(m_winctrl);
        m_resourcemgr->setInputController(m_inputctrl);
        ret = m_resourcemgr->initialize();
        if (ICO_SYC_EOK != ret) {
            return false;
        }

        m_winctrl->setResourceManager(m_resourcemgr);

        CicoSCUserManager *usermgr = CicoSCUserManager::getInstance();
        usermgr->load("/usr/apps/org.tizen.ico.system-controller/res/config/user.xml");

        CicoSCServer *server = CicoSCServer::getInstance();
        server->setWindowCtrl(m_winctrl);
        server->setInputCtrl(m_inputctrl);
        server->setUserMgr(usermgr);
        server->setResourceMgr(m_resourcemgr);
        server->setPolicyMgr(m_resourcemgr->getPolicyManager());

        server->startup(18081, (const char*)"ico_syc_protocol");
        ret = CicoSCWayland::getInstance()->initialize();
        if (ICO_SYC_EOK != ret) {
            return false;
        }
        CicoSCWayland::getInstance()->addEcoreMainWlFdHandler();

        usermgr->initialize();
        if (true == lifecycle->isAppResource()) {
            const CicoSCUser* user = usermgr->getLoginUser();
            if (NULL != user) {
                if (false == lifecycle->startAppResource(user->name)) {
                    lifecycle->createAppResourceFile(user->name);
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("catch exception %s", e.what());
        ICO_DBG("CicoSysConDaemon::onCreate Leave(false)");
        return false;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("catch exception %s", str.c_str());
        ICO_DBG("CicoSysConDaemon::onCreate Leave(false)");
        return false;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        ICO_DBG("CicoSysConDaemon::onCreate Leave(false)");
        return false;
    }

    ICO_TRA("CicoSysConDaemon::onCreate Leave(true)");
    ICO_PRF("SYS_STARTED Initialization is complete");

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on terminate
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoSysConDaemon::onTerminate(void *user_data)
{
    ICO_TRA("CicoSysConDaemon::onTerminate Enter");
    CicoSCUserManager *usermgr = CicoSCUserManager::getInstance();
    usermgr->cloaseUser();
    stop();
    CicoSCServer::getInstance()->teardown();
    CicoSCVInfoManager::getInstance()->teardown();
    ICO_TRA("CicoSysConDaemon::onTerminate Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on pause
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoSysConDaemon::onPause(void *user_data)
{
    ICO_TRA("CicoSysConDaemon::onPause Enter");
    ICO_TRA("CicoSysConDaemon::onPause Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on resume
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoSysConDaemon::onResume(void *user_data)
{
    ICO_TRA("CicoSysConDaemon::onResume Enter");
    ICO_TRA("CicoSysConDaemon::onResume Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on service
 *
 *  @param [in] service     The handle to the service
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoSysConDaemon::onService(service_h service, void *user_data)
{
    ICO_TRA("CicoSysConDaemon::onService Enter");
    ICO_TRA("CicoSysConDaemon::onService Leave");
}
// vim: set expandtab ts=4 sw=4:
