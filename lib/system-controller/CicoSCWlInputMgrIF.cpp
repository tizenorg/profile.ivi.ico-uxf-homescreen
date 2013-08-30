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

#include <cstring>
#include "CicoSCWlInputMgrIF.h"
#include "CicoLog.h"

//==========================================================================
//  static variables
//==========================================================================

// ico_exinput listener
struct ico_exinput_listener CicoSCWlInputMgrIF::ms_exInputListener = {
    .capabilities = CicoSCWlInputMgrIF::wlCapabilitiesCB,
    .code         = CicoSCWlInputMgrIF::wlCodeCB,
    .input        = CicoSCWlInputMgrIF::wlInputCB
};

// ico_input_mgr_device listener
struct ico_input_mgr_device_listener CicoSCWlInputMgrIF::ms_devListener = {
    .input_region = CicoSCWlInputMgrIF::wlRegionCB
};

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlInputMgrIF::CicoSCWlInputMgrIF()
    : m_inputmgr(NULL), m_exinput(NULL), m_inputmgrdev(NULL)
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
 *  @brief  initialize ico_input_mgr interfaces
 *
 *  @param [IN] data        user data
 *  @param [IN] registry    wayland registry
 *  @param [IN] name        wayland display id
 *  @parma [IN] interface   wayland interface name
 *  @parma [IN] version     wayland interface version number
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
            ICO_WRN("initInterface : Leave(binding failed");
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
            ICO_WRN("initInterface : Leave(binding failed");
            return;
        }

        m_exinput = (struct ico_exinput*)wlProxy;
        ico_exinput_add_listener(m_exinput,
                                 &ms_exInputListener,
                                 this);
#if 0
        ico_exinput_set_user_data(m_exinput, NULL/*user data*/);
#endif
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
            ICO_WRN("initInterface : Leave(binding failed");
            return;
        }

        m_inputmgrdev = (struct ico_input_mgr_device*)wlProxy;
        ico_input_mgr_device_add_listener(m_inputmgrdev,
                                          &ms_devListener,
                                          this);
#if 0
        ico_input_mgr_device_set_user_data(m_inputmgrdev, NULL/*user data*/);
#endif
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
 *  @brief  callback to application for input switch information
 *
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] type        input device type (as enum type)
 *  @param [IN] swname      input switch name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] time        input time of miri-sec
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] code        input switch code
 *  @param [IN] state       Of/Off status
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
 *  @param [IN] data                user data
 *  @param [IN] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [IN] region              input regions
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] type        input device type (as enum type)
 *  @param [IN] swname      input switch name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] time        input time of miri-sec
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] code        input switch code
 *  @param [IN] state       Of/Off status
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
 *  @param [IN] data                user data
 *  @param [IN] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [IN] region              input regions
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
