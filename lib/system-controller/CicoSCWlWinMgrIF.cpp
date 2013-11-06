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
 *  @file   CicoSCWlWinMgrIF.cpp
 *
 *  @brief  This file implementation of CicoSCWlInputMgrIF class
 */
//==========================================================================

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

#include "CicoSCWlWinMgrIF.h"
#include "CicoLog.h"

//==========================================================================
//  static variables
//==========================================================================

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlWinMgrIF::CicoSCWlWinMgrIF()
    : m_winmgr(NULL), m_wloutput(NULL)
{
    // ico_window_mgr listener
    m_listener.window_created   = wlCreatedCB;
    m_listener.window_name      = wlNameCB;
    m_listener.window_destroyed = wlDestroyedCB;
    m_listener.window_visible   = wlVisibleCB;
    m_listener.window_configure = wlConfigureCB;
    m_listener.window_active    = wlActiveCB;
    m_listener.layer_visible    = wlLayerVisibleCB;
    m_listener.app_surfaces     = wlAppSurfacesCB;
    m_listener.map_surface      = wlMapSurfaceCB;

    // wayland output listener
    m_wlOutputListener.geometry = wlOutputGeometryCB;
    m_wlOutputListener.mode     = wlOutputModeCB;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWlWinMgrIF::~CicoSCWlWinMgrIF()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize ico_window_mgr interfaces
 *
 *  @param [in] data        user data
 *  @param [in] registry    wayland registry
 *  @param [in] name        wayland display id
 *  @parma [in] interface   wayland interface name
 *  @parma [in] version     wayland interface version number
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::initInterface(void               *data,
                                struct wl_registry *registry,
                                uint32_t           name,
                                const char         *interface,
                                uint32_t           version)
{
    ICO_DBG("CicoSCWlWinMgrIF::initInterface : Enter(interface=%s)", interface);

    // check interface name
    if (0 == strcmp(interface, ICO_WL_WIN_MGR_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ico_window_mgr_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("CicoSCWlWinMgrIF::initInterface : Leave(binding failed)");
            return;
        }


        m_winmgr = (struct ico_window_mgr *)wlProxy;
        ico_window_mgr_add_listener(m_winmgr, 
                                    &m_listener,
                                    this);
#if 0
        ico_window_mgr_set_user_data(m_winmgr, NULL/*TODO*/);
#endif

        ICO_DBG("call ico_window_mgr_declare_manager");
        ico_window_mgr_declare_manager(m_winmgr,
                                       ICO_WINDOW_MGR_DECLARE_MANAGER_MANAGER);

    }
    else if (0 == strcmp(interface, ICO_WL_OUTPUT_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &wl_output_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("CicoSCWlWinMgrIF::initInterface : Leave(binding failed)");
            return;
        }

        m_wloutput = (struct wl_output*)wlProxy;
        wl_output_add_listener(m_wloutput, 
                               &m_wlOutputListener,
                               this);
#if 0
        wl_output_set_user_data(m_wloutput, NULL/*TODO*/);
#endif
    }
    else {
        ICO_WRN("initInterface : Leave(unmatch interface)");
        return;
    }

    if((NULL != m_winmgr) && (NULL != m_wloutput)) {
        m_initialized = true;
    }

    ICO_DBG("CicoSCWlWinMgrIF::initInterface : Leave");
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_declare_manager
 *  
 *  @param [in] manager type of manager
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::declareManager(int32_t manager)
{
    // declare manager request to Multi Window Manager
    ICO_DBG("called: ico_window_mgr_declare_manager(manager=%d)", manager);
    ico_window_mgr_declare_manager(m_winmgr, manager);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_window_layer
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           number of layer
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setWindowLayer(uint32_t surfaceid, uint32_t layer)
{
    // set window layer request to Multi Window Manager
    ICO_DBG("called: ico_window_mgr_set_window_layer"
            "(surfaceid=0x%08X layer=%d)", surfaceid, layer);
    ico_window_mgr_set_window_layer(m_winmgr, surfaceid, layer);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_positionsize
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           number of layer
 *  @param [in] x
 *  @param [in] y
 *  @param [in] width
 *  @param [in] height
 *  @param [in] flags
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setPositionsize(uint32_t surfaceid, uint32_t node,
                                  int32_t x, int32_t y, int32_t width,
                                  int32_t height, int32_t flags)
{
    // set position size request to Multi Window Manager
    ICO_DBG("called: ico_window_mgr_set_positionsize"
            "(surfaceid=0x%08X node=%d x=%d y=%d w=%d h=%d flags=%d)",
            surfaceid, node, x, y, width, height, flags);
    ico_window_mgr_set_positionsize(m_winmgr, surfaceid, node,
                                    x, y, width, height, flags);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_visible
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] visible         visible state
 *  @param [in] raise           raise state
 *  @param [in] flags           option on change visible
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setVisible(uint32_t surfaceid, int32_t visible,
                             int32_t raise, int32_t flags)
{
    // set visible request to Multi Window Manager
    ICO_DBG("called: ico_window_mgr_set_visible"
            "(surfaceid=0x%08X visible=%d raise=%d anima=%d)",
            surfaceid, visible, raise, flags);
    ico_window_mgr_set_visible(m_winmgr, surfaceid, visible, raise, flags);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_visible_animation
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] visible         visible state
 *  @param [in] raise           raise state
 *  @param [in] flags           option on change visible
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::visibleAnimation(uint32_t surfaceid, int32_t visible,
                                   int32_t x, int32_t y,
                                   int32_t width, int32_t height)
{
    // visible animation request to Multi Window Manager
    ICO_DBG("called: ico_window_mgr_visible_animation"
            "(surfaceid=0x%08X visible=%d x=%d y=%d w=%d h=%d)",
            surfaceid, visible, x, y, width, height);
    ico_window_mgr_visible_animation(m_winmgr, surfaceid, visible,
                                     x, y, width, height);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_animation
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] type            transition type
 *  @param [in] animation       name of animation
 *  @param [in] time            time of animation
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setAnimation(uint32_t surfaceid, int32_t type,
                               const char *animation, int32_t time)
{
    ICO_DBG("called: ico_window_mgr_set_animation"
            "(surfaceid=0x%08X type=%d anima=%s time=%d)",
            surfaceid, type, animation, time);
    ico_window_mgr_set_animation(m_winmgr, surfaceid, type, animation, time);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_attributes
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] attributes      attributes of surface
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setAttributes(uint32_t surfaceid, uint32_t attributes)
{
    ICO_DBG("called: ico_window_mgr_set_attributes"
            "(surfaceid=0x%08X attributes=%d)", surfaceid, attributes);
    ico_window_mgr_set_attributes(m_winmgr, surfaceid, attributes);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_active
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] active          flags od active device
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setActive(uint32_t surfaceid, int32_t active)
{
    ICO_DBG("called: ico_window_mgr_set_active"
            "(surfaceid=0x%08X active=%d)", surfaceid, active);
    ico_window_mgr_set_active(m_winmgr, surfaceid, active);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_set_layer_visible
 *  
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           id of layer
 *  @param [in] visible         visible state
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setLayerVisible(uint32_t layer, int32_t visible)
{
    ICO_DBG("called: ico_window_mgr_set_layer_visible"
            "(layer=%d visible=%d)", layer, visible);
    ico_window_mgr_set_layer_visible(m_winmgr, layer, visible);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_get_surfaces
 *  
 *  @param [in] appid           id of application
 *  @param [in] pid             id of process
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::getSurfaces(const char *appid, int pid)
{
    ICO_DBG("called: ico_window_mgr_get_surfaces(appid=%d,pid=%d)",
            appid ? appid : " ", pid);
    ico_window_mgr_get_surfaces(m_winmgr, appid ? appid : " ", pid);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_map_surface
 *  
 *  @param [in] surface     id of wayland surface
 *  @param [in] framerate   interval of changed notify[frame per second]
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::mapSurface(uint32_t surfaceid, int32_t framerate)
{
    ICO_DBG("called: ico_window_mgr_map_surface"
            "(surfaceid=0x%08X framerate=%d)", surfaceid, framerate);
    ico_window_mgr_map_surface(m_winmgr, surfaceid, framerate);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wrapper function of ico_window_mgr_unmap_surface
 *  
 *  @param [in] surface     id of wayland surface
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::unmapSurface(uint32_t surfaceid)
{
    ICO_DBG("called: ico_window_mgr_unmap_surface"
            "(surfaceid=0x%08X)", surfaceid);
    ico_window_mgr_unmap_surface(m_winmgr, surfaceid);
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wayland surface create callback
 *  
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] winname         surface window name(title)
 *  @param [in] pid             wayland client process Id
 *  @param [in] appid           wayland client application Id
 *  @param [in] layertype       surface layer type
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::createdCB(void                  *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t              surfaceid,
                            const char            *winname,
                            int32_t               pid,
                            const char            *appid,
                            int32_t               layertype)
{
    ICO_DBG("CicoSCWlWinMgrIF::createdCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland change surface name callback
 *
 * @param [in] data            user data(unused)
 * @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param [in] surfaceid       ico_window_mgr surface Id
 * @param [in] winname         surface window name(title)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::nameCB(void                  *data,
                         struct ico_window_mgr *ico_window_mgr,
                         uint32_t              surfaceid,
                         const char            *winname)
{
    ICO_WRN("CicoSCWlWinMgrIF::nameCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::destroyedCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid)
{
    ICO_WRN("CicoSCWlWinMgrIF::destroyedCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface visible callback
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] visible         surface visible
 *                              (1=visible/0=invisible/other=no change)
 *  @param [in] raise           surface raise
 *                              (1=raise/0=lower/other=no change)
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::visibleCB(void                  *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t              surfaceid,
                            int32_t               visible,
                            int32_t               raise,
                            int32_t               hint)
{
    ICO_WRN("CicoSCWlWinMgrIF::visibleCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface configure callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] node            surface node Id
 *  @param [in] layertype       surface layer type
 *  @param [in] layer           surface layer Id
 *  @param [in] x               surface upper-left X coordinate
 *  @param [in] y               surface upper-left Y coordinate
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::configureCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid,
                              uint32_t              node,
                              int32_t               layertype,
                              uint32_t              layer,
                              int32_t               x,
                              int32_t               y,
                              int32_t               width,
                              int32_t               height,
                              int32_t               hint)
{
    ICO_WRN("CicoSCWlWinMgrIF::configureCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] active          surface active
 *                              (1=active/0=not active)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::activeCB(void                  *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t              surfaceid,
                           int32_t               active)
{
    ICO_WRN("CicoSCWlWinMgrIF::activeCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland layer visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] layer           layer Id
 *  @param [in] visible         layer visible
 *                              (1=visible/0=invisible/other=no change)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::layerVisibleCB(void                  *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 uint32_t              layer,
                                 int32_t               visible)
{
    ICO_WRN("CicoSCWlWinMgrIF::layerVisibleCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  query application surface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] appid           application Id
 *  @param [in] pid             process Id
 *  @param [in] surface         surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::appSurfacesCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                const char            *appid,
                                int32_t               pid,
                                struct wl_array       *surfaces)
{
    ICO_WRN("CicoSCWlWinMgrIF::appSurfacesCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] event           event
 *  @param [in] surfaceid       surface Id
 *  @param [in] type            surface buffer type(EGL buffer/Shared memory)
 *  @param [in] target          surface buffer target(EGL buffer name)
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] stride          surface buffer(frame buffer) stride
 *  @param [in] format          surface buffer format
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::mapSurfaceCB(void                  *data,
                               struct ico_window_mgr *ico_window_mgr,
                               int32_t               event,
                               uint32_t              surfaceid,
                               uint32_t              type,
                               uint32_t              target,
                               int32_t               width,
                               int32_t               height,
                               int32_t               stride,
                               uint32_t              format)
{
    ICO_WRN("CicoSCWlWinMgrIF::mapSurfaceCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland display attribute callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] wl_output       wayland wl_output interface
 *  @param [in] x               display upper-left X coordinate
 *  @param [in] y               display upper-left Y coordinate
 *  @param [in] physical_width  display physical width
 *  @param [in] physical_height display physical height
 *  @param [in] subpixel        display sub pixel
 *  @param [in] make            display maker
 *  @param [in] model           display model
 *  @param [in] transform       transform
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::outputGeometryCB(void             *data,
                                   struct wl_output *wl_output,
                                   int32_t          x,
                                   int32_t          y,
                                   int32_t          physical_width,
                                   int32_t          physical_height,
                                   int32_t          subpixel,
                                   const char       *make,
                                   const char       *model,
                                   int32_t          transform)
{
    ICO_WRN("CicoSCWlWinMgrIF::outputGeometryCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland display mode callback
 *
 *  @param [in] data        user data(unused)
 *  @param [in] wl_output   wayland wl_output interface
 *  @param [in] flags       flags
 *  @param [in] width       display width
 *  @param [in] height      display height
 *  @param [in] refresh     display refresh rate
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::outputModeCB(void             *data,
                               struct wl_output *wl_output,
                               uint32_t         flags,
                               int32_t          width,
                               int32_t          height,
                               int32_t          refresh)
{
    ICO_WRN("CicoSCWlWinMgrIF::outputModeCB called.");
}

//==========================================================================
// private method
//==========================================================================

//--------------------------------------------------------------------------
/** 
 *  @brief   wayland surface create callback
 *  
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] winname         surface window name(title)
 *  @param [in] pid             wayland client process Id
 *  @param [in] appid           wayland client application Id
 *  @param [in] layertype       surface layer type
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlCreatedCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid,
                              const char            *winname,
                              int32_t               pid,
                              const char            *appid,
                              int32_t               layertype)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlCreatedCB Enter");
    if (NULL == data) {
        ICO_WRN("wlCreatedCB : data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->createdCB(data, ico_window_mgr,
                                                    surfaceid, winname,
                                                    pid, appid, layertype);
//    ICO_DBG("CicoSCWlWinMgrIF::wlCreatedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland change surface name callback
 *
 * @param [in] data            user data(unused)
 * @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param [in] surfaceid       ico_window_mgr surface Id
 * @param [in] winname         surface window name(title)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlNameCB(void                  *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t              surfaceid,
                           const char            *winname)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlNameCB Enter");

    if (NULL == data) {
        ICO_WRN("wlNameCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->nameCB(data, ico_window_mgr,
                                                 surfaceid, winname);
//    ICO_DBG("CicoSCWlWinMgrIF::wlNameCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlDestroyedCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                uint32_t              surfaceid)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlDestroyedCB Enter");

    if (NULL == data) {
        ICO_WRN("wlDestroyedCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->destroyedCB(data, ico_window_mgr,
                                                      surfaceid);
//    ICO_DBG("CicoSCWlWinMgrIF::wlDestroyedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] visible         surface visible
 *                              (1=visible/0=invisible/other=no change)
 *  @param [in] raise           surface raise
 *                              (1=raise/0=lower/other=no change)
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlVisibleCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid,
                              int32_t               visible,
                              int32_t               raise,
                              int32_t               hint)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlVisibleCB Enter");

    if (NULL == data) {
        ICO_WRN("wlVisibleCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->visibleCB(data, ico_window_mgr,
                                                    surfaceid, visible,
                                                    raise, hint);
//    ICO_DBG("CicoSCWlWinMgrIF::wlVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface configure callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] node            surface node Id
 *  @param [in] layertype       surface layer type
 *  @param [in] layer           surface layer Id
 *  @param [in] x               surface upper-left X coordinate
 *  @param [in] y               surface upper-left Y coordinate
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlConfigureCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                uint32_t              surfaceid,
                                uint32_t              node,
                                int32_t               layertype,
                                uint32_t              layer,
                                int32_t               x,
                                int32_t               y,
                                int32_t               width,
                                int32_t               height,
                                int32_t               hint)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlConfigureCB Enter");

    if (NULL == data) {
        ICO_WRN("wlConfigureCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->configureCB(data, ico_window_mgr,
                                                      surfaceid, node, layertype,
                                                      layer, x, y, width, height,
                                                      hint);
//    ICO_DBG("CicoSCWlWinMgrIF::wlConfigureCB Leave");
}
//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] active          surface active
 *                              (1=active/0=not active)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlActiveCB(void                  *data,
                             struct ico_window_mgr *ico_window_mgr,
                             uint32_t              surfaceid,
                             int32_t               active)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlActiveCB Enter");

    if (NULL == data) {
        ICO_WRN("wlActiveCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->activeCB(data, ico_window_mgr,
                                                   surfaceid, active);
//    ICO_DBG("CicoSCWlWinMgrIF::wlActiveCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland layer visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] layer           layer Id
 *  @param [in] visible         layer visible
 *                              (1=visible/0=invisible/other=no change)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlLayerVisibleCB(void                  *data,
                                   struct ico_window_mgr *ico_window_mgr,
                                   uint32_t              layer,
                                   int32_t               visible)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlLayerVisibleCB Enter");

    if (NULL == data) {
        ICO_WRN("wlLayerVisibleCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->layerVisibleCB(data, ico_window_mgr,
                                                         layer, visible);
//    ICO_DBG("CicoSCWlWinMgrIF::wlLayerVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  query application surface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] appid           application Id
 *  @param [in] pid             process Id
 *  @param [in] surface         surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlAppSurfacesCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  const char            *appid,
                                  int32_t               pid,
                                  struct wl_array       *surfaces)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlAppSurfacesCB Enter");

    if (NULL == data) {
        ICO_WRN("wlAppSurfacesCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->appSurfacesCB(data, ico_window_mgr,
                                                        appid, pid, surfaces);
//    ICO_DBG("CicoSCWlWinMgrIF::wlAppSurfacesCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [in] data            user data
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] event           event
 *  @param [in] surfaceid       surface Id
 *  @param [IN] type            buffer type(fixed ICO_WINDOW_MGR_MAP_TYPE_EGL)
 *  @param [IN] target          EGL buffer name
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] stride          surface buffer(frame buffer) stride
 *  @param [in] format          surface buffer format
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlMapSurfaceCB(void                  *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 int32_t               event,
                                 uint32_t              surfaceid,
                                 uint32_t              type,
                                 uint32_t              target,
                                 int32_t               width,
                                 int32_t               height,
                                 int32_t               stride,
                                 uint32_t              format)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlMapSurfaceCB Enter");

    if (NULL == data) {
        ICO_WRN("wlMapSurfaceCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->mapSurfaceCB(data, ico_window_mgr,
                                                       event, surfaceid,
                                                       type, target,
                                                       width, height,
                                                       stride, format);
//    ICO_DBG("CicoSCWlWinMgrIF::wlMapSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland display attribute callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] wl_output       wayland wl_output interface
 *  @param [in] x               display upper-left X coordinate
 *  @param [in] y               display upper-left Y coordinate
 *  @param [in] physical_width  display physical width
 *  @param [in] physical_height display physical height
 *  @param [in] subpixel        display sub pixel
 *  @param [in] make            display maker
 *  @param [in] model           display model
 *  @param [in] transform       transform
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlOutputGeometryCB(void             *data,
                                     struct wl_output *wl_output,
                                     int32_t          x,
                                     int32_t          y,
                                     int32_t          physical_width,
                                     int32_t          physical_height,
                                     int32_t          subpixel,
                                     const char       *make,
                                     const char       *model,
                                     int32_t          transform)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlOutputGeometryCB Enter");

    if (NULL == data) {
        ICO_WRN("wlOutputGeometryCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->outputGeometryCB(data, wl_output,
                                                           x, y,
                                                           physical_width,
                                                           physical_height,
                                                           subpixel,
                                                           make,
                                                           model,
                                                           transform);
//    ICO_DBG("CicoSCWlWinMgrIF::wlOutputGeometryCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland display mode callback
 *
 *  @param [in] data        user data(unused)
 *  @param [in] wl_output   wayland wl_output interface
 *  @param [in] flags       flags
 *  @param [in] width       display width
 *  @param [in] height      display height
 *  @param [in] refresh     display refresh rate
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlOutputModeCB(void             *data,
                                 struct wl_output *wl_output,
                                 uint32_t         flags,
                                 int32_t          width,
                                 int32_t          height,
                                 int32_t          refresh)
{
//    ICO_DBG("CicoSCWlWinMgrIF::wlOutputModeCB Enter");

    if (NULL == data) {
        ICO_WRN("wlOutputGeometryCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->outputModeCB(data, wl_output, flags,
                                                       width, height, refresh);
//    ICO_DBG("CicoSCWlWinMgrIF::wlOutputModeCB Leave");
}
// vim:set expandtab ts=4 sw=4:
