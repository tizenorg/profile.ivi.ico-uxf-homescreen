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
 *  @brief  This file is definition of CicoSCWayland class
 */
//==========================================================================
#ifndef __CICO_SC_WAYLAND_H__
#define __CICO_SC_WAYLAND_H__

#include <string>
#include <map>

#include <Ecore.h>

#include <wayland-client.h>

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCWaylandIF;

//==========================================================================
/*
 *  @brief  This class controls wayland
 */
//==========================================================================
class CicoSCWayland {
public:
    // get instance of CicoSCWayland
    static CicoSCWayland* getInstance();

    // initialize wayland connection
    int initialize(void);

    // flush display
    void flushDisplay(void);

    // dispatch display
    void dispatchDisplay(void);

    // get wayland error
    int getError(void);

    // get wayland file descriptor
    int getWlFd(void);

    // app ecore main wayland file descriptor handler
    int addEcoreMainWlFdHandler(void);

    // add wayland interface
    void addWaylandIF(const char *name, CicoSCWaylandIF* interface);

    // waylnad file descriptor handler
    static Eina_Bool waylandFdHandler(void *data, Ecore_Fd_Handler *handler);

    // wayland global callback function
    void globalCB(void               *data,
                  struct wl_registry *registry,
                  uint32_t           name,
                  const char         *interface,
                  uint32_t           version);

private:
    // wayland standard callback function
    static void wlGlobalCB(void               *data,
                           struct wl_registry *registry,
                           uint32_t           name,
                           const char         *interface,
                           uint32_t           version);

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
    /// my instance
    static CicoSCWayland *ms_myInstance;

    /// wayland's display
    struct wl_display *m_wlDisplay;

    /// wayland's registry
    struct wl_registry *m_wlRegistry;

    /// wayland's file descriptor
    int m_wlFd;

    /// ecore file descriptor handler
    Ecore_Fd_Handler *m_ecoreFdHandler;

    /// interface list
    std::map<std::string, CicoSCWaylandIF*> m_wlInterfaceList;

    // wayland common callbacks
    struct wl_registry_listener m_wlListener;
};
#endif  // __CICO_SC_WAYLAND_H__
// vim:set expandtab ts=4 sw=4:
