/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full tWlExt of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCWlInputMgrIF.cpp
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_WL_INPUT_MGR_IF_H__
#define __CICO_SC_WL_INPUT_MGR_IF_H__

#include <ico_input_mgr-client-protocol.h>
#include "CicoSCWaylandIF.h"

class CicoSCWlInputMgrIF : public CicoSCWaylandIF
{
public:
    void initInterface(void               *data,
                       struct wl_registry *registry,
                       uint32_t           name,
                       const char         *interface,
                       uint32_t           version);

    void capabilitiesCB(void               *data,
                        struct ico_exinput *ico_exinput,
                        const char         *device,
                        int32_t            type,
                        const char         *swname,
                        int32_t            input,
                        const char          *codename,
                        int32_t            code);

    void codeCB(void               *data,
                struct ico_exinput *ico_exinput,
                const char         *device,
                int32_t            input,
                const char         *codename,
                int32_t            code);

    void inputCB(void               *data,
                 struct ico_exinput *ico_exinput,
                 uint32_t           time,
                 const char         *device,
                 int32_t            input,
                 int32_t            code,
                 int32_t            state);

    void regionCB(void                        *data,
                  struct ico_input_mgr_device *ico_input_mgr_device,
                  struct wl_array             *region);

protected:
    // default constructor
    CicoSCWlInputMgrIF();

    // destructor
    ~CicoSCWlInputMgrIF();

    // assignment operator
    CicoSCWlInputMgrIF& operator=(const CicoSCWlInputMgrIF &object);

    // copy constructor
    CicoSCWlInputMgrIF(const CicoSCWlInputMgrIF &object);

private:
    /* ico_input_mgr(Multi Input Manager) callback functions    */
    static void wlCapabilitiesCB(void               *data,
                                 struct ico_exinput *ico_exinput,
                                 const char         *device,
                                 int32_t            type,
                                 const char         *swname,
                                 int32_t            input,
                                 const char         *codename,
                                 int32_t            code);

    static void wlCodeCB(void               *data,
                         struct ico_exinput *ico_exinput,
                         const char         *device,
                         int32_t            input,
                         const char         *codename,
                         int32_t            code);

    static void wlInputCB(void               *data,
                          struct ico_exinput *ico_exinput,
                          uint32_t           time,
                          const char         *device,
                          int32_t            input,
                          int32_t            code,
                          int32_t            state);

    static void wlRegionCB(void                        *data,
                           struct ico_input_mgr_device *ico_input_mgr_device,
                           struct wl_array             *region);

protected:
    // Input Manager PulgIn interface
    struct ico_input_mgr_control *m_inputmgr;
 
    // extra input event interface
    struct ico_exinput *m_exinput;

    // Input Manage Device interface
    struct ico_input_mgr_device *m_inputmgrdev;

private:
    // ico_exinput listener
    static struct ico_exinput_listener ms_exInputListener;

    // ico_input_mgr_device listener
    static struct ico_input_mgr_device_listener ms_devListener;

};
#endif	// __CICO_SC_WL_INPUT_MGR_IF_H__
// vim:set expandtab ts=4 sw=4:
