/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   CicoSysConDaemon.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <iostream>
#include <string>

#include "CicoSysConDaemon.h"
#include "ico_syc_error.h"
#include "CicoLog.h"
#include "CicoSCSystemConfig.h"
#include "CicoSCServer.h"
#include "CicoSCWayland.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCUserManager.h"
#include "CicoSCResourceManager.h"
#include "Cico_aul_listen_app.h"

/**
 *  Default Constructor
 */
CicoSysConDaemon::CicoSysConDaemon()
{
//    ICO_DBG("CicoSysConDaemon::CicoSysConDaemon Enter");
//    ICO_DBG("CicoSysConDaemon::CicoSysConDaemon Leave");
}

/**
 *  Destructor
 */
CicoSysConDaemon::~CicoSysConDaemon()
{
//    ICO_DBG("CicoSysConDaemon::~CicoSysConDaemon Enter");
//    ICO_DBG("CicoSysConDaemon::~CicoSysConDaemon Leave");
}

bool
CicoSysConDaemon::onCreate(void *user_data)
{
    ICO_DBG("CicoSysConDaemon::onCreate Enter");

    try {
        int ret = ICO_SYC_EOK;
        CicoSCSystemConfig::getInstance()->load("/usr/apps/org.tizen.ico.system-controller/res/config/system.xml");

        initAulListenXSignal();

        CicoSCLifeCycleController *lifecycle =
            CicoSCLifeCycleController::getInstance();
        CicoSCWindowController *winctrl     = new CicoSCWindowController();
        CicoSCInputController  *inputctrl   = new CicoSCInputController();

        CicoSCResourceManager  *resourcemgr = new CicoSCResourceManager();
        resourcemgr->setWindowController(winctrl);
        resourcemgr->setInputController(inputctrl);
        ret = resourcemgr->initialize();
        if (ICO_SYC_EOK != ret) {
            return false;
        }

        winctrl->setResourceManager(resourcemgr);

        CicoSCUserManager *usermgr = CicoSCUserManager::getInstance();
        usermgr->load("/usr/apps/org.tizen.ico.system-controller/res/config/user.xml");

        CicoSCServer *server = CicoSCServer::getInstance();
        server->setWindowCtrl(winctrl);
        server->setInputCtrl(inputctrl);
        server->setUserMgr(usermgr);
        server->setResourceMgr(resourcemgr);
        server->setPolicyMgr(resourcemgr->getPolicyManager());

        server->startup(18081, (const char*)"ico_syc_protocol");
        ret = CicoSCWayland::getInstance()->intialize();
        if (ICO_SYC_EOK != ret) {
            return false;
        }
        CicoSCWayland::getInstance()->addEcoreMainWlFdHandler();
        
        usermgr->initialize();
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

    ICO_DBG("CicoSysConDaemon::onCreate Leave(true)");

    return true;
}

#if 0
void
CicoSysConDaemon::onTerminate(void *user_data)
{
    _DBG("CicoSysConDaemon::onTerminate entry");
}

void
CicoSysConDaemon::onPause(void *user_data)
{
    _DBG("CicoSysConDaemon::onPause entry");
}

void
CicoSysConDaemon::onResume(void *user_data)
{
    _DBG("CicoSysConDaemon::onResume entry");
}

void
CicoSysConDaemon::onService(service_h service, void *user_data)
{
    _DBG("CicoSysConDaemon::onService entry");
}
#endif
/* vim: set expandtab ts=4 sw=4: */
