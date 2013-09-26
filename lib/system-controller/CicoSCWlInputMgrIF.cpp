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
 *  @brief  This file implementation of CicoSCInputDev class
 */
//==========================================================================

#include <cstring>

#include <string>
using namespace std;

#include "CicoSCWlInputMgrIF.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlInputMgrIF::CicoSCWlInputMgrIF()
    : m_inputmgr(NULL), m_exinput(NULL), m_inputmgrdev(NULL)
{
    // ico_exinput listener
    m_exInputListener.capabilities = wlCapabilitiesCB;
    m_exInputListener.code         = wlCodeCB;
    m_exInputListener.input        = wlInputCB;

    // ico_input_mgr_device listener
    m_devListener.input_regions    = wlRegionCB;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWlInputMgrIF::~CicoSCWlInputMgrIF()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize ico_input_mgr interfaces
 *
 *  @param [in] data        user data
 *  @param [in] registry    wayland registry
 *  @param [in] name        wayland display id
 *  @parma [in] interface   wayland interface name
 *  @parma [in] version     wayland interface version number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::initInterface(void               *data,
                                  struct wl_registry *registry,
                                  uint32_t           name,
                                  const char         *interface,
                                  uint32_t           version)
{
    ICO_DBG("CicoSCWlInputMgrIF::initInterface : Enter(interface=%s)",
            interface);

    if (0 == strcmp(interface, ICO_WL_INPUT_MGR_CTRL_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ico_input_mgr_control_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("initInterface : Leave(binding failed)");
            return;
        }

        m_inputmgr = (struct ico_input_mgr_control*)wlProxy;
    }
    else if (0 == strcmp(interface, ICO_WL_EXINPUT_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ico_exinput_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("initInterface : Leave(binding failed)");
            return;
        }

        m_exinput = (struct ico_exinput*)wlProxy;
        ico_exinput_add_listener(m_exinput,
                                 &m_exInputListener,
                                 this);
    }
    else if (0 == strcmp(interface, ICO_WL_INPUT_MGR_DEV_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ico_input_mgr_device_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("initInterface : Leave(binding failed)");
            return;
        }

        m_inputmgrdev = (struct ico_input_mgr_device*)wlProxy;
        ico_input_mgr_device_add_listener(m_inputmgrdev,
                                          &m_devListener,
                                          this);
    }

    if ((NULL != m_inputmgrdev) &&
        (NULL != m_inputmgr) &&
        (NULL != m_exinput)) {
        m_initialized = true;
    }
    ICO_DBG("CicoSCWlInputMgrIF::initInterface : Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function ico_input_mgr_control_add_input_app
 *
 *  @param [in] appid   application id
 *  @param [in] device  input device name
 *  @param [in] input   input number
 *  @parma [in] fix     fixed assign flag
 *  @parma [in] keycode assigned keycode value
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::addInputApp(const string &appid,
                                const string &device,
                                int          input,
                                int          fix,
                                int          keycode)
{
    ICO_DBG("called: ico_input_mgr_control_add_input_app"
            "(appid=%s device=%s input=%d fix=%d keycode=%d)",
            appid.c_str(), device.c_str(), input, fix, keycode);
    ico_input_mgr_control_add_input_app(m_inputmgr,appid.c_str(),
                                        device.c_str(), input, fix, keycode);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function ico_input_mgr_control_del_input_app
 *
 *  @param [in] appid   application id
 *  @param [in] device  input device name
 *  @param [in] input   input number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::delInputApp(const string &appid,
                                const string &device,
                                int          input)
{
    ICO_DBG("called: ico_input_mgr_control_del_input_app"
            "(appid=%s device=%s input=%d)",
            appid.c_str(), device.c_str(), input);
    ico_input_mgr_control_del_input_app(m_inputmgr, appid.c_str(),
                                        device.c_str(), input);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function ico_input_mgr_control_send_input_event
 *
 *  @param [in] appid     application id
 *  @param [in] surfaceid surface id
 *  @param [in] type      device type
 *  @param [in] deviceno  input device number
 *  @param [in] time      event time
 *  @param [in] code      event code
 *  @param [in] value     event value
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::sendInputEvent(const string &appid,
                                   int          surfaceid,
                                   int          type,
                                   int          deviceno,
                                   int          time,
                                   int          code,
                                   int          value)
{
    ICO_DBG("called: ico_input_mgr_control_send_input_event"
            "(appid=%s surfaceid=0x%08X type=%d deviceno=%d time=%d code=%d value=%d)",
            appid.c_str(), surfaceid, type, deviceno, time, code, value);
    ico_input_mgr_control_send_input_event(m_inputmgr, appid.c_str(), surfaceid,
                                           type, deviceno, time, code, value);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function ico_exinput_set_input_region
 *
 *  @param [in] target    target window name(winname@appid)
 *  @param [in] x         region x positon
 *  @param [in] y         region y positon
 *  @param [in] width     region width
 *  @param [in] height    region height
 *  @param [in] hotspot_x hotspot x position
 *  @param [in] hotspot_y hotspot y position
 *  @param [in] cursor_x  cursor x position
 *  @param [in] cursor_y  cursor y position
 *  @param [in] cursor_width  cursor width
 *  @param [in] cursor_height cursor height
 *  @param [in] attr      region attribute
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::setInputRegion(const string &target,
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
                                   int attr)
{
    ICO_DBG("called: ico_exinput_set_input_region"
            "(target=%s x/y=%d/%d w/h=%d/%d hot=%d/%d "
            "cursor=%d/%d-%d/%d attr=%d)",
            target.c_str(), x, y, width, height, hotspot_x, hotspot_y,
            cursor_x, cursor_y, cursor_width, cursor_height, attr);
    ico_exinput_set_input_region(m_exinput, target.c_str(), x, y,
                                 width, height, hotspot_x, hotspot_y, cursor_x,
                                 cursor_y, cursor_width, cursor_height,attr);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function ico_exinput_unset_input_region
 *
 *  @param [in] target    target window name(winname@appid)
 *  @param [in] x         region x positon
 *  @param [in] y         region y positon
 *  @param [in] width     region width
 *  @param [in] height    region height
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::unsetInputRegion(const string &target,
                                     int x,
                                     int y,
                                     int width,
                                     int height)
{
    ICO_DBG("called: ico_exinput_unset_input_region"
            "(target=%s x=%d y=%d width=%d height=%d",
            target.c_str(), x, y, width, height);
    ico_exinput_unset_input_region(m_exinput, target.c_str(),
                                   x, y, width, height);
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input switch information
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] type        input device type (as enum type)
 *  @param [in] swname      input switch name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::capabilitiesCB(void               *data,
                                   struct ico_exinput *ico_exinput,
                                   const char         *device,
                                   int32_t            type,
                                   const char         *swname,
                                   int32_t            input,
                                   const char         *codename,
                                   int32_t            code)
{
    ICO_WRN("CicoSCWlInputMgrIF::capabilitiesCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input code information
 *  
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::codeCB(void               *data,
                           struct ico_exinput *ico_exinput,
                           const char         *device,
                           int32_t            input,
                           const char         *codename,
                           int32_t            code)
{
    ICO_WRN("CicoSCWlInputMgrIF::codeCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for switch input 
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] time        input time of millisecond
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] code        input switch code
 *  @param [in] state       Of/Off status
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::inputCB(void               *data,
                            struct ico_exinput *ico_exinput,
                            uint32_t           time,
                            const char         *device,
                            int32_t            input,
                            int32_t            code,
                            int32_t            state)
{
    ICO_WRN("CicoSCWlInputMgrIF::inputCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for change input region
 *
 *  @param [in] data                user data
 *  @param [in] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [in] region              input regions
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::regionCB(void                        *data,
                             struct ico_input_mgr_device *ico_input_mgr_dev,
                             struct wl_array             *region)
{
    ICO_WRN("CicoSCWlInputMgrIF::regionCB called.");
}

//==========================================================================
// private method
//==========================================================================

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input switch information
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] type        input device type (as enum type)
 *  @param [in] swname      input switch name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlCapabilitiesCB(void               *data,
                                     struct ico_exinput *ico_exinput,
                                     const char         *device,
                                     int32_t            type,
                                     const char         *swname,
                                     int32_t            input,
                                     const char         *codename,
                                     int32_t            code)
{
    ICO_DBG("CicoSCWlInputMgrIF::wlCapabilitiesCB Enter");

    if (NULL == data) {
        ICO_WRN("wlCapabilitiesCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->capabilitiesCB(data, ico_exinput,
                                                           device, type,
                                                           swname, input,
                                                           codename, code);
    ICO_DBG("CicoSCWlInputMgrIF::wlCapabilitiesCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input code information
 *  
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlCodeCB(void               *data,
                             struct ico_exinput *ico_exinput,
                             const char         *device,
                             int32_t            input,
                             const char         *codename,
                             int32_t            code)
{
    ICO_DBG("CicoSCWlInputMgrIF::wlCodeCB Enter");

    if (NULL == data) {
        ICO_WRN("wlCodeCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->codeCB(data, ico_exinput,
                                                   device, input,
                                                   codename, code);

    ICO_DBG("CicoSCWlInputMgrIF::wlCodeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for switch input 
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] time        input time of millisecond
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] code        input switch code
 *  @param [in] state       Of/Off status
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlInputCB(void               *data,
                              struct ico_exinput *ico_exinput,
                              uint32_t           time,
                              const char         *device,
                              int32_t            input,
                              int32_t            code,
                              int32_t            state)
{
    ICO_DBG("CicoSCWlInputMgrIF::wlInputCB Enter");

    if (NULL == data) {
        ICO_WRN("wlInputCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->inputCB(data, ico_exinput,
                                                    time, device, input,
                                                    code, state);

    ICO_DBG("CicoSCWlInputMgrIF::wlInputCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for change input region
 *
 *  @param [in] data                user data
 *  @param [in] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [in] region              input regions
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlRegionCB(void                        *data,
                               struct ico_input_mgr_device *ico_input_mgr_dev,
                               struct wl_array             *region)
{
    ICO_DBG("CicoSCWlInputMgrIF::wlRegionCB Enter");

    if (NULL == data) {
        ICO_WRN("wlRegionCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->regionCB(data, ico_input_mgr_dev,
                                                     region);

    ICO_DBG("CicoSCWlInputMgrIF::wlRegionCB Leave");
}
// vim:set expandtab ts=4 sw=4:
