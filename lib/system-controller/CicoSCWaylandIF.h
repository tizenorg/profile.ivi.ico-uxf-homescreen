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
 *  @file   CicoSCWaylandIF.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_WAYLAND_IF_H__
#define __CICO_SC_WAYLAND_IF_H__

#include <string>
#include <wayland-client.h>

using namespace std;

class CicoSCWaylandIF {
public:
    // intialize interface
    virtual void initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version);
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
    static const char * ICO_WL_WIN_MGR_IF;
    static const char * ICO_WL_INPUT_MGR_CTRL_IF;
    static const char * ICO_WL_EXINPUT_IF;
    static const char * ICO_WL_INPUT_MGR_DEV_IF;
    static const char * ICO_WL_OUTPUT_IF;
    static const char * ICO_WL_SHM_IF;

    // initialized flag
    bool m_initialized;
};
#endif	// __CICO_SC_WAYLAND_IF_H__
// vim:set expandtab ts=4 sw=4:
