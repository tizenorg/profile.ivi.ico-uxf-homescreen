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
 *  @brief  This file is definition of CicoSCInputDevConf class
 */
//==========================================================================
#ifndef __CICO_SC_WL_INPUT_MGR_IF_H__
#define __CICO_SC_WL_INPUT_MGR_IF_H__

#include <string>
#include <ico_input_mgr-client-protocol.h>
#include "CicoSCWaylandIF.h"

//--------------------------------------------------------------------------
/**
 *  @brief  This class is wayland interface of multi input manager
 */
//--------------------------------------------------------------------------
class CicoSCWlInputMgrIF : public CicoSCWaylandIF
{
public:
    // initialize interface
    void initInterface(void               *data,
                       struct wl_registry *registry,
                       uint32_t           name,
                       const char         *interface,
                       uint32_t           version);

    // capabilities callback function
    void capabilitiesCB(void               *data,
                        struct ico_exinput *ico_exinput,
                        const char         *device,
                        int32_t            type,
                        const char         *swname,
                        int32_t            input,
                        const char          *codename,
                        int32_t            code);

    // code callback function
    void codeCB(void               *data,
                struct ico_exinput *ico_exinput,
                const char         *device,
                int32_t            input,
                const char         *codename,
                int32_t            code);

    // input callback function
    void inputCB(void               *data,
                 struct ico_exinput *ico_exinput,
                 uint32_t           time,
                 const char         *device,
                 int32_t            input,
                 int32_t            code,
                 int32_t            state);

    // region callback function
    void regionCB(void                        *data,
                  struct ico_input_mgr_device *ico_input_mgr_device,
                  struct wl_array             *region);

protected:
    // default constructor
    CicoSCWlInputMgrIF();

    // destructor
    virtual ~CicoSCWlInputMgrIF();

    // assignment operator
    CicoSCWlInputMgrIF& operator=(const CicoSCWlInputMgrIF &object);

    // copy constructor
    CicoSCWlInputMgrIF(const CicoSCWlInputMgrIF &object);

    // wrapper function ico_input_mgr_control_add_input_app
    void addInputApp(const std::string &appid,
                     const std::string &device,
                     int               input,
                     int               fix,
                     int               keycode);

    // wrapper function ico_input_mgr_control_del_input_app
    void delInputApp(const std::string &appid,
                     const std::string &device,
                     int               input);

    // wrapper function ico_input_mgr_control_send_input_event
    void sendInputEvent(const std::string &appid,
                        int               surfaceid,
                        int               type,
                        int               deviceno,
                        int               time,
                        int               code,
                        int               value);

    // wrapper function ico_exinput_set_input_region
    void setInputRegion(const std::string &target,
                        int x,
                        int y,
                        int width,
                        int height,
                        int hotspot_x,
                        int hotspot_y,
                        int cursor_x,
                        int cursor_y,
                        int cursor_width,
                        int cursor_height,
                        int attr);

    // wrapper function ico_exinput_unset_input_region
    void unsetInputRegion(const std::string &target,
                          int x,
                          int y,
                          int width,
                          int height);

private:
    // ico_input_mgr(Multi Input Manager) callback functions

    // wayland capabilities callback function
    static void wlCapabilitiesCB(void               *data,
                                 struct ico_exinput *ico_exinput,
                                 const char         *device,
                                 int32_t            type,
                                 const char         *swname,
                                 int32_t            input,
                                 const char         *codename,
                                 int32_t            code);

    // wayland code callback function
    static void wlCodeCB(void               *data,
                         struct ico_exinput *ico_exinput,
                         const char         *device,
                         int32_t            input,
                         const char         *codename,
                         int32_t            code);

    // wayland input callback function
    static void wlInputCB(void               *data,
                          struct ico_exinput *ico_exinput,
                          uint32_t           time,
                          const char         *device,
                          int32_t            input,
                          int32_t            code,
                          int32_t            state);

    // wayland region callback function
    static void wlRegionCB(void                        *data,
                           struct ico_input_mgr_device *ico_input_mgr_device,
                           struct wl_array             *region);

protected:
    // input manager interface
    struct ico_input_mgr_control *m_inputmgr;

    // extra input event interface
    struct ico_exinput *m_exinput;

    // input manage device interface
    struct ico_input_mgr_device *m_inputmgrdev;

private:
    // exinput listener
    struct ico_exinput_listener m_exInputListener;

    // input manager device listener
    struct ico_input_mgr_device_listener m_devListener;

};
#endif  // __CICO_SC_WL_INPUT_MGR_IF_H__
// vim:set expandtab ts=4 sw=4:
