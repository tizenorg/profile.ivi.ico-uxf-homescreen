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
 *  @file   CicoSCWayland.cpp
 *
 *  @brief  This file implementation of CicoSCWayland class
 */
//==========================================================================    

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <Ecore.h>

#include <map>
using namespace std;

#include <wayland-client.h>
#include <ilm/ilm_control.h>

#include "CicoSCWayland.h"
#include "CicoSCWlWinMgrIF.h"
#include "CicoSCWindowController.h"
#include "CicoLog.h"
#include "ico_syc_error.h"

//==========================================================================    
//  private static variable
//==========================================================================    
CicoSCWayland* CicoSCWayland::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWayland::CicoSCWayland()
    : m_wlDisplay(NULL),
      m_wlRegistry(NULL),
      m_wlFd(-1),
      m_ecoreFdHandler(NULL)
{
    // wayland callbacks
    m_wlListener.global        = wlGlobalCB;
    m_wlListener.global_remove = NULL;

    CicoSCWayland::ms_myInstance = this;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWayland::~CicoSCWayland()
{
    CicoSCWayland::ms_myInstance = NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get instance of CicoSCWayland
 *
 *  @return instance of CicoSCWayland
 */
//--------------------------------------------------------------------------
CicoSCWayland*
CicoSCWayland::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCWayland();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize wayland connection
 *
 *  @return ICO_SYC_EOK on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSCWayland::initialize(void)
{
    ICO_TRA("CicoSCWayland::initialize Enter");

    // initialize genivi ivi-shell
    if (ilm_init() != ILM_SUCCESS)  {
        ICO_ERR("ilm_init failed.");
        ICO_TRA("CicoSCWayland::initialize Leave(EIO)");
        return ICO_SYC_EIO;
    }

    for (int i = 0; i < (5000/50); ++i)  {
        m_wlDisplay = wl_display_connect(NULL);
        if (NULL != m_wlDisplay) {
            break;
        }
        usleep(50*1000);
    }

    if (NULL == m_wlDisplay) {
        ICO_ERR("wl_display_connect failed.");
        ICO_TRA("CicoSCWayland::initialize Leave(EIO)");
        return ICO_SYC_EIO;
    }

    ICO_DBG("called: wl_display_get_registry(wlDisplay=0x%08x)", m_wlDisplay);
    m_wlRegistry = wl_display_get_registry(m_wlDisplay);
    if (NULL == m_wlRegistry) {
        ICO_ERR("wl_display_get_registry failed.");
        ICO_TRA("CicoSCWayland::initialize Leave(EIO)");
        return ICO_SYC_EIO;
    }

    // add wayland global callbacks
    ICO_DBG("called: wl_registry_add_listener");
    wl_registry_add_listener(m_wlRegistry, &m_wlListener, this);

    int initCount = 0;
    int ifcount = m_wlInterfaceList.size();
    for (int i = 0; i < (500/20); ++i) {
        initCount = 0;
        map<string, CicoSCWaylandIF*>::iterator itr;
        itr = m_wlInterfaceList.begin();
        for (; itr != m_wlInterfaceList.end(); ++itr) {
            if(true == itr->second->isInitialized()) {
                initCount++;
            }
        }
        if (ifcount == initCount) {
            break;
        }
        ICO_DBG("called: wl_display_dispatch"
                "(wlDisplay=0x%08x)", (int)m_wlDisplay);
        wl_display_dispatch(m_wlDisplay);
        usleep(20*1000);
    }

    if (ifcount != initCount) {
        ICO_ERR("initialize interface time up NG");
        ICO_TRA("CicoSCWayland::initialize Leave(EIO)");
        return ICO_SYC_EIO;
    }
    else {
        ICO_DBG("initialize interface all OK");
    }

    // flush display 
    ICO_DBG("called: wl_display_flush(wlDisplay=0x%08x)", (int)m_wlDisplay);
    wl_display_flush(m_wlDisplay);

    ICO_DBG("called: wl_display_get_fd(wlDisplay=0x%08x)", (int)m_wlDisplay);
    m_wlFd = wl_display_get_fd(m_wlDisplay);
    ICO_DBG("CicoSCWayland::initialize: Wayland/Weston fd=%d", m_wlFd);

    ICO_DBG("CicoSCWayland::initialize: Wayland/Weston connect OK");

    // initialize genivi callbacks
    CicoSCWindowController::getInstance()->initializeGeniviLMS();

    ICO_TRA("CicoSCWayland::initialize: Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  flush display
 */
//--------------------------------------------------------------------------
void
CicoSCWayland::flushDisplay(void)
{
    if (NULL == m_wlDisplay) {
        ICO_ERR("wlDisplay == NULL");
        return;
    }
//    ICO_DBG("called: wl_display_flush(0x%08x)", (int)m_wlDisplay);
    int wlret = wl_display_flush(m_wlDisplay);
    if (-1 == wlret) {
        int error = wl_display_get_error(m_wlDisplay);
        ICO_ERR("wayland error(%d)", error);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  dispatch display
 */
//--------------------------------------------------------------------------
void
CicoSCWayland::dispatchDisplay(void)
{
    if (NULL == m_wlDisplay) {
        ICO_ERR("wlDisplay == NULL");
        return;
    }
//    ICO_DBG("call wl_display_dispatch(0x%08x)", (int)m_wlDisplay);
    int wlret = wl_display_dispatch(m_wlDisplay);
    if (-1 == wlret) {
        int error =  wl_display_get_error(m_wlDisplay);
        ICO_ERR("wl_display_dispatch failed. error(%d)", error);

        if (error == EINVAL)    {
            exit(9);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get wayland error
 */
//--------------------------------------------------------------------------
int
CicoSCWayland::getError(void)
{
    if (NULL == m_wlDisplay) {
        ICO_ERR("wlDisplay == NULL");
        return -1;
    }

    ICO_DBG("fd=%d", wl_display_get_fd(m_wlDisplay));
    return wl_display_get_error(m_wlDisplay);
}

//--------------------------------------------------------------------------
/**
 *  @brief  get wayland file destructor
 *
 *  @return wayland file destructor
 */
//--------------------------------------------------------------------------
int CicoSCWayland::getWlFd(void)
{
    return m_wlFd;
}

//--------------------------------------------------------------------------
/**
 *  @brief  add wayland file destructor handler to ecore main 
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EIO ecore_main_fd_handler_add failed
 */
//--------------------------------------------------------------------------
int
CicoSCWayland::addEcoreMainWlFdHandler(void)
{
    ICO_TRA("CicoSCWayland::addEcoreMainWlFdHandler Enter");
    ICO_DBG("called: ecore_main_fd_handler_add(fd=%d)", m_wlFd);
    m_ecoreFdHandler = ecore_main_fd_handler_add(m_wlFd, 
                                                 ECORE_FD_READ,
                                                 &waylandFdHandler,
                                                 NULL,
                                                 NULL,
                                                 NULL);
    if (NULL == m_ecoreFdHandler) {
        ICO_ERR("ecore_main_fd_handler_add failed.");
        ICO_TRA("CicoSCWayland::addEcoreMainWlFdHandler Leave(EIO)");
        return ICO_SYC_EIO;
    }
    ecore_main_loop_iterate();

    int arg = 0;
    if (ioctl(m_wlFd, FIONREAD, &arg) < 0) {
        ICO_WRN("ioclt(FIONREAD) failed. errno=%d:%s", errno, strerror(errno));
        arg = 0;
    }
    if (arg > 0)   {
        CicoSCWayland::getInstance()->dispatchDisplay();
    }

    ICO_TRA("CicoSCWayland::addEcoreMainWlFdHandler Leave");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   add wayland interface
 *
 *  @param [in] name        interface name
 *  @param [in] interface   interface instance
 */
//--------------------------------------------------------------------------
void
CicoSCWayland::addWaylandIF(const char *name, CicoSCWaylandIF* interface)
{
    m_wlInterfaceList[name] = interface;
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland global callback
 *
 *  @param [in] data        user data(unused)
 *  @param [in] registry    wayland registry
 *  @param [in] name        wayland display Id(unused)
 *  @param [in] interface   wayland interface name
 *  @param [in] version     wayland interface version number(unused)
 */
//--------------------------------------------------------------------------
void
CicoSCWayland::globalCB(void               *data,
                        struct wl_registry *registry,
                        uint32_t           name,
                        const char         *interface,
                        uint32_t           version)
{
    map<string, CicoSCWaylandIF*>::iterator itr;
    itr = m_wlInterfaceList.find(interface);

    if (m_wlInterfaceList.end() == itr) {
        ICO_DBG("skip interface=%s", interface);
        return;
    }

    CicoSCWaylandIF *wlIF = itr->second;
    wlIF->initInterface(data, registry, name, interface, version);

    // flush display 
    wl_display_flush(m_wlDisplay);
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland global callback
 *
 *  @param [in] data        user data
 *  @param [in] registry    wayland registry
 *  @param [in] name        wayland display Id(unused)
 *  @param [in] interface   wayland interface name
 *  @param [in] version     wayland interface version number(unused)
 */
//--------------------------------------------------------------------------
void
CicoSCWayland::wlGlobalCB(void               *data,
                          struct wl_registry *registry,
                          uint32_t           name,
                          const char         *interface,
                          uint32_t           version)
{
    if (NULL == data) {
        ICO_ERR("data == NULL");
        return;
    }

    static_cast<CicoSCWayland*>(data)->globalCB(data, registry, name,
                                                interface, version);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland global callback
 *
 *  @param [in] data        user data
 *  @param [in] handler     ecore file destructor handler
 *
 *  @return ECORE_CALLBACK_RENEW on retry , ECORE_CALLBACK_CANCEL on cancel
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoSCWayland::waylandFdHandler(void *data, Ecore_Fd_Handler *handler)
{
//    ICO_TRA("waylandFdHandler: Enter");

    CicoSCWayland::getInstance()->flushDisplay();

    int arg = 0;
    if (ioctl(CicoSCWayland::getInstance()->getWlFd(), FIONREAD, &arg) < 0) {
        ICO_WRN("ioclt(FIONREAD) failed. errno=%d:%s", errno, strerror(errno));
        arg = 0;
    }

//    ICO_DBG("waylandFdHandler: arg(%d))", arg);
    if (arg > 0) {
        CicoSCWayland::getInstance()->dispatchDisplay();
    }

    if (0 == arg) {
        ICO_ERR("wayland fd read error.");
        ICO_DBG("called: ecore_main_loop_quit()");
        ecore_main_loop_quit();
        return ECORE_CALLBACK_CANCEL;
    }

    CicoSCWayland::getInstance()->flushDisplay();
//    ICO_TRA("waylandFdHandler: Leave");
    return ECORE_CALLBACK_RENEW ;
}
// vim:set expandtab ts=4 sw=4:
