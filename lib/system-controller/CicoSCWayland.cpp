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
 *  @file   CicoSCWayland.cpp
 *
 *  @brief  
 */
/*========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>


#include <sys/ioctl.h>
#include <Ecore.h>

#include <algorithm>

#include "wayland-client.h"
#include "ico_window_mgr-client-protocol.h"
#include "CicoSCWayland.h"
#include "CicoSCWlWinMgrIF.h"
#include "CicoLog.h"

CicoSCWayland* CicoSCWayland::ms_myInstance = NULL;

CicoSCWayland::CicoSCWayland()
    : m_wlDisplay(NULL),
      m_wlOutput(NULL),
      m_wlFd(-1),
      m_ecoreFdHandler(NULL)
{
}

CicoSCWayland::~CicoSCWayland()
{
}

CicoSCWayland*
CicoSCWayland::getInstance(void)
{
	if (NULL == ms_myInstance) {
		ms_myInstance = new CicoSCWayland();
	}

	return ms_myInstance;
}

int
CicoSCWayland::intialize(void)
{
    ICO_DBG("CicoSCWayland::initialize: Enter");
    // 
    for (int i = 0; i < (5000/50); ++i)  {
        m_wlDisplay = wl_display_connect(NULL);
        if (NULL != m_wlDisplay) {
            break;
        }
        usleep(50*1000);
    }

    if (NULL == m_wlDisplay) {
        ICO_ERR("wl_display_connect failed.");
        return -1; /* TODO */
    }

    m_wlRegistry = wl_display_get_registry(m_wlDisplay);
    if (NULL == m_wlRegistry) {
        ICO_ERR("wl_display_get_registry failed.");
        return -1; /* TODO */
    }

    // wayland callbacks
    struct wl_registry_listener wlListener = {
        .global        = wlGlobalCB,
        .global_remove = NULL
    };

    // add wayland global callbacks
    wl_registry_add_listener(m_wlRegistry, &wlListener, this);

    // 
    int initCount = 0;
    for (int i = 0; i < (500/20); ++i) {
        initCount = 0;
        map<string, CicoSCWaylandIF*>::iterator itr;
        itr = m_wlInterfaceList.begin();
        int ifcount = m_wlInterfaceList.size();
        for (; itr != m_wlInterfaceList.end(); ++itr) {
            if(true == itr->second->isInitialized()) {
                initCount++;
            }
        }
        if (ifcount == initCount) {
            ICO_DBG("initializ all OK");
            break;
        }
        ICO_DBG("call wl_display_dispatch(%08x)", (int)m_wlDisplay);
        wl_display_dispatch(m_wlDisplay);
        usleep(20*1000);
    }

    // flush display 
    wl_display_flush(m_wlDisplay);

    // 
    ICO_DBG("call wl_display_get_fd(%08x)", (int)m_wlDisplay);
    m_wlFd = wl_display_get_fd(m_wlDisplay);
    ICO_DBG("CicoSCWayland::initialize: Wayland/Weston fd(%d)", m_wlFd);

    ICO_DBG("CicoSCWayland::initialize: Wayland/Weston connect OK");


    ICO_DBG("CicoSCWayland::initialize: Leave");
    return 0; // TODO
}

void CicoSCWayland::flushDisplay(void)
{
    if (NULL == m_wlDisplay) {
        return;
    }
    ICO_DBG("call wl_display_flush(%08x)", (int)m_wlDisplay);
    wl_display_flush(m_wlDisplay);
}

void CicoSCWayland::dispatchDisplay(void)
{
    if (NULL == m_wlDisplay) {
        return;
    }
    ICO_DBG("call wl_display_dispatch(%08x)", (int)m_wlDisplay);
    wl_display_dispatch(m_wlDisplay);
}

int CicoSCWayland::getWlFd(void)
{
    return m_wlFd;
}

int
CicoSCWayland::addEcoreMainWlFdHandler(void)
{
    ICO_DBG("waylandFdHandler: Enter");
    m_ecoreFdHandler = ecore_main_fd_handler_add(m_wlFd, 
                                                 ECORE_FD_READ,
                                                 &waylandFdHandler,
                                                 NULL, /* data */
                                                 NULL,
                                                 NULL);
    ecore_main_loop_iterate();
    waylandFdHandler(NULL, m_ecoreFdHandler);
    ICO_DBG("waylandFdHandler: Leave");
    return 0;
}


void
CicoSCWayland::addWaylandIF(const char *name, CicoSCWaylandIF* interface)
{
    m_wlInterfaceList[name] = interface;
}

/* wayland standard callback functions      */
void
CicoSCWayland::globalCB(void *data,
                        struct wl_registry *registry,
                        uint32_t name,
                        const char *interface,
                        uint32_t version)
{
//    ICO_DBG("globalCB: Enter(Event=%s DispId=%08x)", interface, name);

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
//    ICO_DBG("globalCB: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_wayland_globalcb: wayland global callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   registry        wayland registry
 * @param[in]   name            wayland display Id(unused)
 * @param[in]   interface       wayland interface name
 * @param[in]   version         wayland interface version number(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoSCWayland::wlGlobalCB(void       *data,
                          struct     wl_registry *registry,
                          uint32_t   name,
                          const char *interface,
                          uint32_t   version)
{
//    ICO_DBG("wlGlobalCB: Enter(Event=%s DispId=%08x)", interface, name);

    if (NULL == data) {
        ICO_DBG("data is null");
        return;
    }

    static_cast<CicoSCWayland*>(data)->globalCB(data, registry, name,
                                                interface, version);

//    ICO_DBG("wlGlobalCB : Leave");
}

#if 0
void
CicoSCWayland::wlOutputGeometrycb(void *data,
                                  struct wl_output *wl_output,
                                  int32_t x,
                                  int32_t y,
                                  int32_t physical_width,
                                  int32_t physical_height,
                                  int32_t subpixel,
                                  const char *make,
                                  const char *model,
                                  int32_t transform)
{
    ICO_DBG("ico_uxf_output_geometrycb: Enter(x/y=%d/%d, pwidth/height=%d/%d, trans=%d)",
               x, y, physical_width, physical_height, transform);

    vector<CicoSCDisplay*>::iterator itr = 
    find_if(m_displayList.begin(), m_displayList.end(), FindDisplay(wl_output));

    if (m_displayList.end() == itr) {
        return;
    }

    CicoSCDisplay* scDisplay = *itr;

    if (NULL != scDisplay) {
        if ((transform == WL_OUTPUT_TRANSFORM_90)
            || (transform == WL_OUTPUT_TRANSFORM_270)
            || (transform == WL_OUTPUT_TRANSFORM_FLIPPED_90)
            || (transform == WL_OUTPUT_TRANSFORM_FLIPPED_270))  {
            scDisplay->m_pWidth      = physical_height;
            scDisplay->m_pHeight     = physical_width;
            scDisplay->m_orientation = ICO_UXF_ORIENTATION_VERTICAL;
        }
        else {
            scDisplay->m_pWidth      = physical_width;
            scDisplay->m_pHeight     = physical_height;
            scDisplay->m_orientation = ICO_UXF_ORIENTATION_HORIZONTAL;
        }
    }
    ICO_DBG("ico_uxf_output_geometrycb: Leave");
}

void
CicoSCWayland::wlOutputModecb(void *data,
                              struct wl_output *wl_output,
                              uint32_t flags,
                              int32_t width,
                              int32_t height,
                              int32_t refresh)
{
    ICO_DBG("ico_uxf_output_modecb: Enter(flg=%d, width=%d, height=%d, ref=%d)",
               flags, width, height, refresh);

    
    vector<CicoSCDisplay*>::iterator itr = 
    find_if(m_displayList.begin(), m_displayList.end(), FindDisplay(wl_output));

    if (m_displayList.end() == itr) {
        return;
    }

    CicoSCDisplay* scDisplay = *itr;

    if ((NULL != scDisplay) && (flags & WL_OUTPUT_MODE_CURRENT)) {
        if (scDisplay->m_orientation == ICO_UXF_ORIENTATION_VERTICAL) {
            scDisplay->m_pWidth  = height;
            scDisplay->m_pHeight = width;
        }
        else {
            scDisplay->m_pWidth  = width;
            scDisplay->m_pHeight = height;
        }
    }
    ICO_DBG("ico_uxf_output_modecb: Leave");
}
#endif

Eina_Bool
CicoSCWayland::waylandFdHandler(void *data, Ecore_Fd_Handler *handler)
{
    ICO_DBG("waylandFdHandler: Enter");

    int arg = 0;

    CicoSCWayland* scWayland = CicoSCWayland::getInstance();
    
    scWayland->flushDisplay();

    if (ioctl(scWayland->getWlFd(), FIONREAD, &arg) < 0)   {
        ICO_WRN("ico_uxf_main_loop_iterate: ioclt(FIONREAD,) Error %d", errno);
        arg = 0;
    }

    ICO_DBG("waylandFdHandler: arg(%d))", arg);
    if (arg > 0)   {
        scWayland->dispatchDisplay();
    }

    scWayland->flushDisplay();
    ICO_DBG("waylandFdHandler: Leave)");
    return EINA_TRUE;
}
/* vim:set expandtab ts=4 sw=4: */
