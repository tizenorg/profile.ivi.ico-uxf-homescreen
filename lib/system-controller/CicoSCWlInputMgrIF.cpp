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
#include "CicoSCWayland.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlInputMgrIF::CicoSCWlInputMgrIF()
{
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
 *  @brief  initialize input interfaces
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
    ICO_TRA("CicoSCWlInputMgrIF::initInterface : Enter(interface=%s)", interface);
    m_initialized = true;
    ICO_TRA("CicoSCWlInputMgrIF::initInterface Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function add_input_app
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
    ICO_DBG("called: add_input_app"
            "(appid=%s device=%s input=%d fix=%d keycode=%d)",
            appid.c_str(), device.c_str(), input, fix, keycode);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function del_input_app
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
    ICO_DBG("called: del_input_app"
            "(appid=%s device=%s input=%d)",
            appid.c_str(), device.c_str(), input);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function set_input_region
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
    ICO_DBG("called: set_input_region"
            "(target=%s x/y=%d/%d w/h=%d/%d hot=%d/%d "
            "cursor=%d/%d-%d/%d attr=%d)",
            target.c_str(), x, y, width, height, hotspot_x, hotspot_y,
            cursor_x, cursor_y, cursor_width, cursor_height, attr);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wrapper function unset_input_region
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
    ICO_DBG("called: unset_input_region"
            "(target=%s x=%d y=%d width=%d height=%d",
            target.c_str(), x, y, width, height);
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input switch information
 *
 *  @param [in] data        user data
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
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::codeCB(void               *data,
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
 *  @param [in] time        input time of millisecond
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] code        input switch code
 *  @param [in] state       Of/Off status
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::inputCB(void               *data,
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
 *  @param [in] region              input regions
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::regionCB(void                        *data,
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
                                     const char         *device,
                                     int32_t            type,
                                     const char         *swname,
                                     int32_t            input,
                                     const char         *codename,
                                     int32_t            code)
{
    if (NULL == data) {
        ICO_WRN("wlCapabilitiesCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->capabilitiesCB(data, device, type,
                                                           swname, input,
                                                           codename, code);
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input code information
 *
 *  @param [in] data        user data
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlCodeCB(void               *data,
                             const char         *device,
                             int32_t            input,
                             const char         *codename,
                             int32_t            code)
{
    if (NULL == data) {
        ICO_WRN("wlCodeCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->codeCB(data, device, input,
                                                   codename, code);
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for switch input
 *
 *  @param [in] data        user data
 *  @param [in] time        input time of millisecond
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] code        input switch code
 *  @param [in] state       Of/Off status
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlInputCB(void               *data,
                              uint32_t           time,
                              const char         *device,
                              int32_t            input,
                              int32_t            code,
                              int32_t            state)
{
    if (NULL == data) {
        ICO_WRN("wlInputCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->inputCB(data, time, device, input,
                                                    code, state);
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for change input region
 *
 *  @param [in] data                user data
 *  @param [in] region              input regions
 */
//--------------------------------------------------------------------------
void
CicoSCWlInputMgrIF::wlRegionCB(void                        *data,
                               struct wl_array             *region)
{
    if (NULL == data) {
        ICO_WRN("wlRegionCB: data is null");
        return;
    }
    static_cast<CicoSCWlInputMgrIF*>(data)->regionCB(data, region);
}
// vim:set expandtab ts=4 sw=4:
