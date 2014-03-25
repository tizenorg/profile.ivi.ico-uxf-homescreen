/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCWaylandIF.h
 *
 *  @brief  This file is definition of CicoSCWaylandIF class
 */
//==========================================================================
#ifndef __CICO_SC_WAYLAND_IF_H__
#define __CICO_SC_WAYLAND_IF_H__

#include <wayland-client.h>

//==========================================================================
/*
 *  @brief  This class is abstract class of wayland interface
 */
//==========================================================================
class CicoSCWaylandIF {
public:
    // initialize interface
    virtual void initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version);

    // inquire whether initialized
    bool isInitialized(void);

protected:
    // default constructor
    CicoSCWaylandIF();

    // destructor
    virtual ~CicoSCWaylandIF();

    // assignment operator
    CicoSCWaylandIF& operator=(const CicoSCWaylandIF &object);

    // copy constructor
    CicoSCWaylandIF(const CicoSCWaylandIF &object);

protected:
    /// interface name of wayland multi window manager
    static const char * ICO_WL_WIN_MGR_IF;
    /// interface name of wayland input manager control
    static const char * ICO_WL_INPUT_MGR_CTRL_IF;
    /// interface name of wayland exinput
    static const char * ICO_WL_EXINPUT_IF;
    /// interface name of wayland input manager device
    static const char * ICO_WL_INPUT_MGR_DEV_IF;
    /// interface name of genivi ivi_application
    static const char * ICO_WL_IVI_APPLICATION_IF;
    /// interface name of genivi ivi_controller
    static const char * ICO_WL_IVI_CONTROLLER_IF;
    /// interface name of wayland output
    static const char * ICO_WL_OUTPUT_IF;

    // initialized flag
    bool m_initialized;
};
#endif  // __CICO_SC_WAYLAND_IF_H__
// vim:set expandtab ts=4 sw=4:
