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
 *  @brief  
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

// ico_window_mgr listener
struct ico_window_mgr_listener CicoSCWlWinMgrIF::ms_listener = {
    .window_created   = CicoSCWlWinMgrIF::wlCreatedCB,
    .window_name      = CicoSCWlWinMgrIF::wlNameCB,
    .window_destroyed = CicoSCWlWinMgrIF::wlDestroyedCB,
    .window_visible   = CicoSCWlWinMgrIF::wlVisibleCB,
    .window_configure = CicoSCWlWinMgrIF::wlConfigureCB,
    .window_active    = CicoSCWlWinMgrIF::wlActiveCB,
    .layer_visible    = CicoSCWlWinMgrIF::wlLayerVisibleCB,
    .app_surfaces     = CicoSCWlWinMgrIF::wlAppSurfacesCB,
    .map_surface      = CicoSCWlWinMgrIF::wlMapSurfaceCB 
};

// wayland output listener
struct wl_output_listener CicoSCWlWinMgrIF::ms_wlOutputListener = {
    .geometry = CicoSCWlWinMgrIF::wlOutputGeometryCB,
    .mode     = CicoSCWlWinMgrIF::wlOutputModeCB
};

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlWinMgrIF::CicoSCWlWinMgrIF()
    : m_winmgr(NULL), m_wloutput(NULL), m_wlshm(NULL)
{
	strcpy(m_shmName, "/tmp/ico/thumbnail-shm-XXXXXX");
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
 *  @param [IN] data        user data
 *  @param [IN] registry    wayland registry
 *  @param [IN] name        wayland display id
 *  @parma [IN] interface   wayland interface name
 *  @parma [IN] version     wayland interface version number
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
                                    &ms_listener,
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
                               &ms_wlOutputListener,
                               this);
#if 0
        wl_output_set_user_data(m_wloutput, NULL/*TODO*/);
#endif
    }
    else if (0 == strcmp(interface, ICO_WL_SHM_IF)) {
        // get interface instance
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &wl_shm_interface,
                                         1);
		m_wlshm = (struct wl_shm*)wlProxy;
        if (NULL == wlProxy) {
            ICO_WRN("initInterface : interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_WRN("CicoSCWlWinMgrIF::initInterface : Leave(binding failed)");
            return;
        }
        // TODO mkdir
        int fd = mkostemp(m_shmName, O_CLOEXEC);
        if (fd < 0) {
            ICO_ERR("CicoSCWlWinMgrIF::initInterface : Leave(mkostemp failed)");
            return;
        }
        if (ftruncate(fd, ICO_WL_SHM_SIZE) < 0) {
            ICO_ERR("CicoSCWlWinMgrIF::initInterface : Leave(ftruncate failed)");
            close(fd);
            return;
        }
        m_wlshmpool = wl_shm_create_pool(m_wlshm, fd, ICO_WL_SHM_SIZE);
        close(fd);
        if (NULL == m_wlshmpool) {
            ICO_ERR("CicoSCWlWinMgrIF::initInterface : Leave(wl_shm_create_pool failed)");
            return;
        }
    }
    else {
        ICO_WRN("initInterface : Leave(unmatch interface)");
        return;
    }

    if((NULL != m_winmgr) && (NULL != m_wloutput) && (NULL != m_wlshm)) {
        m_initialized = true;
    }

    ICO_DBG("CicoSCWlWinMgrIF::initInterface : Leave");
}

//--------------------------------------------------------------------------
/** 
 *  @brief   wayland surface create callback
 *  
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] winname         surface window name(title)
 *  @param [IN] pid             wayland client process Id
 *  @param [IN] appid           wayland client application Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::createdCB(void                  *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t              surfaceid,
                            const char            *winname,
                            int32_t               pid,
                            const char            *appid)
{
    ICO_DBG("CicoSCWlWinMgrIF::createdCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland change surface name callback
 *
 * @param [IN] data            user data(unused)
 * @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param [IN] surfaceid       ico_window_mgr surface Id
 * @param [IN] winname         surface window name(title)
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
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
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
 *  @brief  wayland surface visible callback(static fu *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] visible         surface visible
 *                              (1=visible/0=unvisible/other=nochange)
 *  @param [IN] raise           surface raise
 *                              (1=raise/0=lower/other=nochange)
 *  @param [IN] hint            client request
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
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] node            surface node Id
 *  @param [IN] x               surface upper-left X coodinate
 *  @param [IN] y               surface upper-left Y coodinate
 *  @param [IN] width           surface width
 *  @param [IN] height          surface height
 *  @param [IN] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::configureCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid,
                              uint32_t              node,
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
 *  @brief  wayland surface active callback(static func
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] active          surface active
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
 *  @brief  wayland layer visible callback(stati
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] layer           layer Id
 *  @param [IN] visible         layer visible
 *                              (1=visible/0=unvisible/other=nochange)
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
 *  @brief  query applicationsurface callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] appid           application Id
 *  @param [IN] suface          surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::appSurfacesCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                const char            *appid,
                                struct wl_array       *surfaces)
{
    ICO_WRN("CicoSCWlWinMgrIF::appSurfacesCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] event           event
 *  @param [IN] surfaceid       surface Id
 *  @param [IN] width           surface width
 *  @param [IN] height          surface height
 *  @param [IN] stride          surface buffer(frame buffer) stride
 *  @param [IN] format          surface buffer format
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
 *  @param [IN] data            user data(unused)
 *  @param [IN] wl_output       wayland wl_output interface
 *  @param [IN] x               display upper-left X coodinate
 *  @param [IN] y               display upper-left Y coodinate
 *  @param [IN] physical_width  display physical width
 *  @param [IN] physical_height display physical height
 *  @param [IN] subpixel        display sub pixcel
 *  @param [IN] make            display maker
 *  @param [IN] model           diaplay model
 *  @param [IN] transform       transform
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
 *  @param [IN] data        user data(unused)
 *  @param [IN] wl_output   wayland wl_output interface
 *  @param [IN] flags       flags
 *  @param [IN] width       display width
 *  @param [IN] height      display height
 *  @param [IN] refresh     display refresh rate
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
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] winname         surface window name(title)
 *  @param [IN] pid             wayland client process Id
 *  @param [IN] appid           wayland client application Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlCreatedCB(void                  *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t              surfaceid,
                              const char            *winname,
                              int32_t               pid,
                              const char            *appid)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlCreatedCB Enter");
    if (NULL == data) {
        ICO_WRN("wlCreatedCB : data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->createdCB(data, ico_window_mgr,
                                                    surfaceid, winname,
                                                    pid, appid);
    ICO_DBG("CicoSCWlWinMgrIF::wlCreatedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland change surface name callback
 *
 * @param [IN] data            user data(unused)
 * @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param [IN] surfaceid       ico_window_mgr surface Id
 * @param [IN] winname         surface window name(title)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlNameCB(void                  *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t              surfaceid,
                           const char            *winname)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlNameCB Enter");

    if (NULL == data) {
        ICO_WRN("wlNameCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->nameCB(data, ico_window_mgr,
                                                 surfaceid, winname);
    ICO_DBG("CicoSCWlWinMgrIF::wlNameCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlDestroyedCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                uint32_t              surfaceid)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlDestroyedCB Enter");

    if (NULL == data) {
        ICO_WRN("wlDestroyedCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->destroyedCB(data, ico_window_mgr,
                                                      surfaceid);
    ICO_DBG("CicoSCWlWinMgrIF::wlDestroyedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface visible callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] visible         surface visible
 *                              (1=visible/0=unvisible/other=nochange)
 *  @param [IN] raise           surface raise
 *                              (1=raise/0=lower/other=nochange)
 *  @param [IN] hint            client request
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
    ICO_DBG("CicoSCWlWinMgrIF::wlVisibleCB Enter");

    if (NULL == data) {
        ICO_WRN("wlVisibleCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->visibleCB(data, ico_window_mgr,
                                                    surfaceid, visible,
                                                    raise, hint);
    ICO_DBG("CicoSCWlWinMgrIF::wlVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface configure callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] node            surface node Id
 *  @param [IN] x               surface upper-left X coodinate
 *  @param [IN] y               surface upper-left Y coodinate
 *  @param [IN] width           surface width
 *  @param [IN] height          surface height
 *  @param [IN] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlConfigureCB(void                  *data,
                                struct ico_window_mgr *ico_window_mgr,
                                uint32_t              surfaceid,
                                uint32_t              node,
                                uint32_t              layer,
                                int32_t               x,
                                int32_t               y,
                                int32_t               width,
                                int32_t               height,
                                int32_t               hint)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlConfigureCB Enter");

    if (NULL == data) {
        ICO_WRN("wlConfigureCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->configureCB(data, ico_window_mgr,
                                                      surfaceid, node, layer,
                                                      x, y, width, height,
                                                      hint);
    ICO_DBG("CicoSCWlWinMgrIF::wlConfigureCB Leave");
}
//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback(static func
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] surfaceid       ico_window_mgr surface Id
 *  @param [IN] active          surface active
 *                              (1=active/0=not active)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlActiveCB(void                  *data,
                             struct ico_window_mgr *ico_window_mgr,
                             uint32_t              surfaceid,
                             int32_t               active)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlActiveCB Enter");

    if (NULL == data) {
        ICO_WRN("wlActiveCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->activeCB(data, ico_window_mgr,
                                                   surfaceid, active);
    ICO_DBG("CicoSCWlWinMgrIF::wlActiveCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland layer visible callback(stati
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] layer           layer Id
 *  @param [IN] visible         layer visible
 *                              (1=visible/0=unvisible/other=nochange)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlLayerVisibleCB(void                  *data,
                                   struct ico_window_mgr *ico_window_mgr,
                                   uint32_t              layer,
                                   int32_t               visible)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlLayerVisibleCB Enter");

    if (NULL == data) {
        ICO_WRN("wlLayerVisibleCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->layerVisibleCB(data, ico_window_mgr,
                                                         layer, visible);
    ICO_DBG("CicoSCWlWinMgrIF::wlLayerVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  query applicationsurface callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] appid           application Id
 *  @param [IN] suface          surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlAppSurfacesCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  const char            *appid,
                                  struct wl_array       *surfaces)
{
    ICO_DBG("CicoSCWlWinMgrIF::wlAppSurfacesCB Enter");

    if (NULL == data) {
        ICO_WRN("wlAppSurfacesCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->appSurfacesCB(data, ico_window_mgr,
                                                        appid, surfaces);
    ICO_DBG("CicoSCWlWinMgrIF::wlAppSurfacesCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [IN] event           event
 *  @param [IN] surfaceid       surface Id
 *  @param [IN] width           surface width
 *  @param [IN] height          surface height
 *  @param [IN] stride          surface buffer(frame buffer) stride
 *  @param [IN] format          surface buffer format
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
    ICO_DBG("CicoSCWlWinMgrIF::wlMapSurfaceCB Enter");

    if (NULL == data) {
        ICO_WRN("wlMapSurfaceCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->mapSurfaceCB(data, ico_window_mgr,
                                                       event, surfaceid,
                                                       type, target,
                                                       width, height,
                                                       stride, format);
    ICO_DBG("CicoSCWlWinMgrIF::wlMapSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland display attribute callback
 *
 *  @param [IN] data            user data(unused)
 *  @param [IN] wl_output       wayland wl_output interface
 *  @param [IN] x               display upper-left X coodinate
 *  @param [IN] y               display upper-left Y coodinate
 *  @param [IN] physical_width  display physical width
 *  @param [IN] physical_height display physical height
 *  @param [IN] subpixel        display sub pixcel
 *  @param [IN] make            display maker
 *  @param [IN] model           diaplay model
 *  @param [IN] transform       transform
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
    ICO_DBG("CicoSCWlWinMgrIF::wlOutputGeometryCB Enter");

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
    ICO_DBG("CicoSCWlWinMgrIF::wlOutputGeometryCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland display mode callback
 *
 *  @param [IN] data        user data(unused)
 *  @param [IN] wl_output   wayland wl_output interface
 *  @param [IN] flags       flags
 *  @param [IN] width       display width
 *  @param [IN] height      display height
 *  @param [IN] refresh     display refresh rate
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
    ICO_DBG("CicoSCWlWinMgrIF::wlOutputModeCB Enter");

    if (NULL == data) {
        ICO_WRN("wlOutputGeometryCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->outputModeCB(data, wl_output, flags,
                                                       width, height, refresh);
    ICO_DBG("CicoSCWlWinMgrIF::wlOutputModeCB Leave");
}
// vim:set expandtab ts=4 sw=4:
