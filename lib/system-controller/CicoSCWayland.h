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
 *  @file   CicoSCWayland.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_WAYLAND_H__
#define __CICO_SC_WAYLAND_H__

#include <vector>
#include <map>
using namespace std;

#include <Ecore.h>

#include <wayland-client.h>
#include <ico_window_mgr-client-protocol.h>
#include "CicoSCDisplay.h"

class CicoSCWaylandIF;

class CicoSCWayland {
public:
    static CicoSCWayland* getInstance();

    int intialize(void);

    void flushDisplay(void);
    void dispatchDisplay(void);
    int getWlFd(void);
    int addEcoreMainWlFdHandler(void);
    void addWaylandIF(const char *name, CicoSCWaylandIF* interface);

    static Eina_Bool waylandFdHandler(void *data, Ecore_Fd_Handler *handler);

    /* wayland standard callback functions      */
    void globalCB(void *data,
                  struct wl_registry *registry,
                  uint32_t name,
                  const char *interface,
                  uint32_t version);

private:
    static void wlGlobalCB(void       *data,
                           struct     wl_registry *registry,
                           uint32_t   name,
                           const char *interface,
                           uint32_t   version);

private:
    // default constructor
    CicoSCWayland();

    // destructor
    ~CicoSCWayland();

    // assignment operator
    CicoSCWayland& operator=(const CicoSCWayland &object);

    // copy constructor
    CicoSCWayland(const CicoSCWayland &object);

private:
    /* my instance */
    static CicoSCWayland *ms_myInstance;

    /* Wayland's display                */
    struct wl_display *m_wlDisplay;

    struct wl_registry *m_wlRegistry;

    /* Wayland's output describes       */
    struct wl_output *m_wlOutput;

    /* Wayland's file descriptor        */
    int m_wlFd;

    Ecore_Fd_Handler         *m_ecoreFdHandler;

    map<string, CicoSCWaylandIF*> m_wlInterfaceList;
};
#endif	// __CICO_SC_WAYLAND_H__
// vim:set expandtab ts=4 sw=4:
