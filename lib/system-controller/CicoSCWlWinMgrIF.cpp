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

#include "ico_syc_type.h"
#include "CicoSCWlWinMgrIF.h"
#include "CicoLog.h"
#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoSCWindowController.h"

//==========================================================================
//  static variables
//==========================================================================
struct ico_window_mgr *CicoSCWlWinMgrIF::m_winmgr = NULL;
struct ivi_application *CicoSCWlWinMgrIF::m_ivi_app = NULL;
struct ivi_controller *CicoSCWlWinMgrIF::m_ivi_ctrl = NULL;
struct wl_output *CicoSCWlWinMgrIF::m_wloutput = NULL;

int CicoSCWlWinMgrIF::m_id_surface = 0;

struct creation_surface_wait    *CicoSCWlWinMgrIF::m_wait_surface_creation = NULL;
struct creation_surface_wait    *CicoSCWlWinMgrIF::m_free_surface_creation = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWlWinMgrIF::CicoSCWlWinMgrIF()
{
    // ico_window_mgr listener
    m_listener.window_active    = wlActiveCB;
    m_listener.map_surface      = wlMapSurfaceCB;
    m_listener.update_surface   = wlUpdateSurfaceCB;
    m_listener.destroy_surface  = wlDestroySurfaceCB;

    // genivi ivi_application listener
    m_ivi_app_listener.error = wlIviAppErrorCB;
    m_ivi_app_listener.wl_shell_info = wlIviAppNativeShellInfoCB;

    // genivi ivi_controller listener
    m_ivi_ctrl_listener.screen = wlIviCtrlScreenCB;
    m_ivi_ctrl_listener.layer = wlIviCtrlLayerCB;
    m_ivi_ctrl_listener.surface = wlIviCtrlSurfaceCB;
    m_ivi_ctrl_listener.error = wlIviCtrlErrorCB;
    m_ivi_ctrl_listener.native_handle = wlIviCtrlNativeHandleCB;

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
    ICO_TRA("CicoSCWlWinMgrIF::initInterface Enter(interface=%s)", interface);

    // check interface name
    if (0 == strcmp(interface, ICO_WL_WIN_MGR_IF)) {
        // get interface instance
        ICO_DBG("called: wl_registry_bind");
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ico_window_mgr_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave(binding failed)");
            return;
        }


        m_winmgr = (struct ico_window_mgr *)wlProxy;
        ico_window_mgr_add_listener(m_winmgr,
                                    &m_listener,
                                    this);
    }
    else if (0 == strcmp(interface, ICO_WL_IVI_APPLICATION_IF)) {
        // get interface instance
        ICO_DBG("called: wl_registry_bind for ivi_application");
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ivi_application_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave(binding failed)");
            return;
        }
        m_ivi_app = (struct ivi_application *)wlProxy;
        ivi_application_add_listener(m_ivi_app,
                                     &m_ivi_app_listener,
                                     this);
    }
    else if (0 == strcmp(interface, ICO_WL_IVI_CONTROLLER_IF)) {
        // get interface instance
        ICO_DBG("called: wl_registry_bind for ivi_controller");
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &ivi_controller_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave(binding failed)");
            return;
        }
        m_ivi_ctrl = (struct ivi_controller *)wlProxy;
        ivi_controller_add_listener(m_ivi_ctrl,
                                    &m_ivi_ctrl_listener,
                                    this);
    }
    else if (0 == strcmp(interface, ICO_WL_OUTPUT_IF)) {
        // get interface instance
        ICO_DBG("called: wl_registry_bind");
        void *wlProxy = wl_registry_bind(registry,
                                         name,
                                         &wl_output_interface,
                                         1);
        if (NULL == wlProxy) {
            ICO_WRN("interface(%s) wl_registry_bind failed.",
                    interface);
            ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave(binding failed)");
            return;
        }

        m_wloutput = (struct wl_output*)wlProxy;
        wl_output_add_listener(m_wloutput,
                               &m_wlOutputListener,
                               this);
    }
    else {
        ICO_WRN("unmatch interface");
        ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave(unmatch interface)");
        return;
    }

    if((NULL != m_winmgr) && (NULL != m_wloutput)) {
        m_initialized = true;
    }

    ICO_TRA("CicoSCWlWinMgrIF::initInterface Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ilm_layerAddSurface
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           layer id
 *  @param [in] oldlayer        old layer id(if 0xffffffff, no old layer)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setWindowLayer(uint32_t surfaceid, uint32_t layer, uint32_t oldlayer)
{
    // set window layer request to Multi Window Manager
    ICO_DBG("CicoSCWlWinMgrIF::setWindowLayer: "
            "surfaceid=0x%08X layer=%d->%d", surfaceid, oldlayer, layer);

    if (oldlayer == layer)  {
        ICO_DBG("CicoSCWlWinMgrIF::setWindowLayer: new layer same as old, NOP");
        return;
    }

    // remove original layer
    if (oldlayer <= 0x7fffffff) {
        ICO_TRA("CicoSCWlWinMgrIF::setWindowLayer: remove surface %08x "
                "from layer(%d)", surfaceid, oldlayer);
        if (ilm_layerRemoveSurface(oldlayer, surfaceid) != ILM_SUCCESS) {
            ICO_ERR("CicoSCWlWinMgrIF::setWindowLayer ilm_layerRemoveSurface(%d,%08x) "
                    "Error", oldlayer, surfaceid);
        }
        // must need ilm_commitChanges() after ilm_layerRemoveSurface()
        if (ilm_commitChanges() != ILM_SUCCESS) {
            ICO_ERR("CicoSCWlWinMgrIF::setWindowLayer ilm_commitChanges Error");
        }
    }
    else    {
        ICO_TRA("CicoSCWlWinMgrIF::setWindowLayer: surface %08x has no old layer(%d)",
                surfaceid, oldlayer);
    }

    // add new layer
    if (ilm_layerAddSurface(layer, surfaceid) != ILM_SUCCESS)   {
        ICO_ERR("CicoSCWlWinMgrIF::setWindowLayer ilm_layerAddSurface(%d,%08x) Error",
                layer, surfaceid);
    }
    if (ilm_commitChanges() != ILM_SUCCESS) {
        ICO_ERR("CicoSCWlWinMgrIF::setWindowLayer ilm_commitChanges Error");
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ilm_surfaceSetDestinationRectangle
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           number of layer
 *  @param [in] x
 *  @param [in] y
 *  @param [in] width
 *  @param [in] height
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setPositionsize(uint32_t surfaceid, uint32_t node,
                                  int32_t x, int32_t y, int32_t width, int32_t height)
{
    // set position size request to Multi Window Manager
    ICO_DBG("called: ilm_surfaceSetDestinationRectangle"
            "(surfaceid=0x%08X node=%d x=%d y=%d w=%d h=%d)",
            surfaceid, node, x, y, width, height);

    t_ilm_uint  Dimansion[2];
    Dimansion[0] = width;
    Dimansion[1] = height;
    t_ilm_uint  Position[2];
    Position[0] = x;
    Position[1] = y;

    if (ilm_surfaceSetDimension(surfaceid, Dimansion) != ILM_SUCCESS)   {
        ICO_ERR("CicoSCWlWinMgrIF::setPositionsize "
                "ilm_surfaceSetDimension(%08x) Error", surfaceid);
    }
    else if (ilm_surfaceSetPosition(surfaceid, Position) != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWlWinMgrIF::setPositionsize "
                "ilm_surfaceSetPosition(%08x) Error", surfaceid);
    }
    else if (ilm_surfaceSetDestinationRectangle(surfaceid, x, y, width, height)
            != ILM_SUCCESS) {
        ICO_ERR("CicoSCWlWinMgrIF::setPositionsize ilm_surfaceSetDestinationRectangle"
                "(%08x,%d,%d,%d,%d) Error", surfaceid, x, y, width, height);
    }
    else if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWlWinMgrIF::setPositionsize ilm_commitChanges() Error");
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ilm_surfaceSetVisibility
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] visible         visible state
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setVisible(uint32_t surfaceid, int32_t visible)
{
    // set visible request to Multi Window Manager
    ICO_DBG("called: ilm_surfaceSetVisibility(surfaceid=0x%08X visible=%d)",
            surfaceid, visible);
    if ((visible == ICO_SYC_WIN_VISIBLE_SHOW) || (visible == ICO_SYC_WIN_VISIBLE_HIDE)) {
        ilm_surfaceSetVisibility(surfaceid, visible);
        if (ilm_commitChanges() != ILM_SUCCESS) {
            ICO_ERR("CicoSCWlWinMgrIF::setVisible: ilm_commitChanges() Error");
        }
    }
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
 *  @brief   wrapper function of ilm_SetKeyboardFocusOn
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] active          flags or active device(unused)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setActive(uint32_t surfaceid, int32_t active)
{
    ICO_DBG("called: ilm_SetKeyboardFocusOn"
            "(surfaceid=0x%08X active=%d)", surfaceid, active);
    if ((ilm_SetKeyboardFocusOn(surfaceid) != ILM_SUCCESS) ||
        (ilm_commitChanges() != ILM_SUCCESS))   {
        ICO_ERR("CicoSCWlWinMgrIF::setActive ilm_SetKeyboardFocusOn(%08x) Error", surfaceid);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ilm_layerSetVisibility
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           id of layer
 *  @param [in] visible         visible state
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setLayerVisible(uint32_t layer, int32_t visible)
{
    ICO_DBG("called: ilm_layerSetVisibility"
            "(layer=%d visible=%d)", layer, visible);
    if ((ilm_layerSetVisibility(layer, visible) != ILM_SUCCESS) ||
        (ilm_commitChanges() != ILM_SUCCESS))   {
        ICO_ERR("CicoSCWlWinMgrIF::setLayerVisible ilm_layerSetVisibility(%d,%d) Error",
                layer, visible);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ilm_takeSurfaceScreenshot
 *
 *  @param [in] surface     id of wayland surface
 *  @param [in] filepath    surface image pixel file path
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::setmapGet(int surfaceid, const char *filepath)
{
    ICO_DBG("called: ilm_takeSurfaceScreenshot(filepath=%s,, surface=0x%08x)",
            filepath ? filepath : "(null)", surfaceid);
    if ((ilm_takeSurfaceScreenshot(filepath, surfaceid) != ILM_SUCCESS) ||
        (ilm_commitChanges() != ILM_SUCCESS))   {
        ICO_ERR("CicoSCWlWinMgrIF::setmapGet ilm_takeSurfaceScreenshot(%s,%x) Error",
                filepath ? filepath : "(null)", surfaceid);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   wrapper function of ico_window_mgr_map_surface
 *
 *  @param [in] surface     id of wayland surface
 *  @param [in] framerate   interval of changed notify[frame per second]
 *  @param [in] filepath    surface image pixel file path
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::mapSurface(uint32_t surfaceid, int32_t framerate, const char *filepath)
{
    ICO_DBG("called: ico_window_mgr_map_surface(surfaceid=0x%08X framerate=%d file=%s)",
            surfaceid, framerate, filepath ? filepath : "(null)");
    // currently GENIVI genivi-shell not support contents change, so use ico_window_mgr
    if ((filepath != NULL) && (*filepath != 0) && (*filepath != ' '))   {
        ico_window_mgr_map_surface(m_winmgr, surfaceid, framerate, filepath);
    }
    else    {
        ico_window_mgr_map_surface(m_winmgr, surfaceid, framerate, " ");
    }
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
    // currently GENIVI genivi-shell not support contents change, so use ico_window_mgr
    ico_window_mgr_unmap_surface(m_winmgr, surfaceid);
}

//--------------------------------------------------------------------------
/**
 *  @brief   get creation surface window name(title) and pid
 *
 *  @param [in]  id_surface surface id
 *  @param [out] pid        application process id
 */
//--------------------------------------------------------------------------
const char *
CicoSCWlWinMgrIF::wlIviCtrlGetSurfaceWaiting(uint32_t id_surface, int *pid)
{
    struct creation_surface_wait    *tp = m_wait_surface_creation;

    while (tp)   {
        if (tp->id_surface == id_surface)   {
            ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlGetSurfaceWaiting(%x) pid=%d title=<%s>",
                    id_surface, tp->pid, tp->title);
            *pid = tp->pid;
            return tp->title;
        }
        tp = tp->next;
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlGetSurfaceWaiting(%x) dose not exist", id_surface);
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   remove surface window
 *
 *  @param [in]  id_surface surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlRemoveSurface(uint32_t id_surface)
{
    struct creation_surface_wait    *tp = m_wait_surface_creation;
    struct creation_surface_wait    *bp = NULL;

    while (tp)   {
        if (tp->id_surface == id_surface)   {
            if (bp) {
                bp->next = tp->next;
            }
            else    {
                m_wait_surface_creation = tp->next;
            }
            tp->next = m_free_surface_creation;
            m_free_surface_creation = tp;

            ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlRemoveSurface(%x) removed", id_surface);
            return;
        }
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlRemoveSurface(%x) dose not exist", id_surface);
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] select          select device(unused)
 *                              (0=not active/1=pointer/2=touch)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::activeCB(void                  *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t              surfaceid,
                           int32_t               select)
{
    ICO_WRN("CicoSCWlWinMgrIF::activeCB called.");
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
                               int32_t               width,
                               int32_t               height,
                               int32_t               stride,
                               uint32_t              format)
{
    ICO_WRN("CicoSCWlWinMgrIF::mapSurfaceCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface update event callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       surface Id
 *  @param [in] visible         visibility
 *  @param [in] srcwidth        application buffer width
 *  @param [in] srcheight       application buffer height
 *  @param [in] x               X
 *  @param [in] y               Y
 *  @param [in] width           width
 *  @param [in] height          height
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::updateSurfaceCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t              surfaceid,
                                  int                   visible,
                                  int                   srcwidth,
                                  int                   srcheight,
                                  int                   x,
                                  int                   y,
                                  int                   width,
                                  int                   height)
{
    ICO_WRN("CicoSCWlWinMgrIF::updateSurfaceCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::destroySurfaceCB(void                  *data,
                                   struct ico_window_mgr *ico_window_mgr,
                                   uint32_t              surfaceid)
{
    ICO_WRN("CicoSCWlWinMgrIF::destroySurfaceCB called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland update surface name callback
 *
 *  @param [in] surfaceid       surface Id
 *  @param [in] winname         surface name (title)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::updateWinnameCB(uint32_t surfaceid,
                                  const char *winname)
{
    ICO_WRN("CicoSCWlWinMgrIF::updateWinnameCB called.");
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
};

//--------------------------------------------------------------------------
/**
 *  @brief   wayland genivi ivi-surface create callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller plugin interface
 *  @param [in] id_surface      surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::createSurfaceCB(void                  *data,
                                  struct ivi_controller *ivi_controller,
                                  uint32_t id_surface)
{
    ICO_WRN("CicoSCWlWinMgrIF::createSurfaceCB called.");
}

//==========================================================================
// private method
//==========================================================================

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] select          select device(unused)
 *                              (0=not active/1=pointer/2=touch)
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlActiveCB(void                  *data,
                             struct ico_window_mgr *ico_window_mgr,
                             uint32_t              surfaceid,
                             int32_t               select)
{
//    ICO_TRA("CicoSCWlWinMgrIF::wlActiveCB Enter");

    if (NULL == data) {
        ICO_WRN("wlActiveCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->activeCB(data, ico_window_mgr,
                                                   surfaceid, select);
//    ICO_TRA("CicoSCWlWinMgrIF::wlActiveCB Leave");
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
                                 int32_t               width,
                                 int32_t               height,
                                 int32_t               stride,
                                 uint32_t              format)
{
//    ICO_TRA("CicoSCWlWinMgrIF::wlMapSurfaceCB Enter");

    if (NULL == data) {
        ICO_WRN("wlMapSurfaceCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->mapSurfaceCB(data, ico_window_mgr,
                                                       event, surfaceid,
                                                       type,
                                                       width, height,
                                                       stride, format);
//    ICO_TRA("CicoSCWlWinMgrIF::wlMapSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface update event callback
 *
 *  @param [in] data            user data
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       surface Id
 *  @param [in] visible         visibility
 *  @param [in] srcwidth        application buffer width
 *  @param [in] srcheight       application buffer height
 *  @param [in] x               X
 *  @param [in] y               Y
 *  @param [in] width           width
 *  @param [in] height          height
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlUpdateSurfaceCB(void                  *data,
                                    struct ico_window_mgr *ico_window_mgr,
                                    uint32_t              surfaceid,
                                    int                   visible,
                                    int                   srcwidth,
                                    int                   srcheight,
                                    int                   x,
                                    int                   y,
                                    int                   width,
                                    int                   height)
{
//    ICO_TRA("CicoSCWlWinMgrIF::wlUpdateSurfaceCB Enter");

    if (NULL == data) {
        ICO_WRN("wlUpdateSurfaceCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->updateSurfaceCB(data, ico_window_mgr,
                                                          surfaceid, visible,
                                                          srcwidth, srcheight,
                                                          x, y, width, height);
//    ICO_TRA("CicoSCWlWinMgrIF::wlUpdateSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface destroy event callback
 *
 *  @param [in] data            user data
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlDestroySurfaceCB(void                  *data,
                                     struct ico_window_mgr *ico_window_mgr,
                                     uint32_t             surfaceid)
{
//    ICO_TRA("CicoSCWlWinMgrIF::wlDestroySurfaceCB Enter");

    if (NULL == data) {
        ICO_WRN("wlDestroySurfaceCB: data is null");
        return;
    }
    wlIviCtrlRemoveSurface(surfaceid);

    static_cast<CicoSCWlWinMgrIF*>(data)->destroySurfaceCB(data, ico_window_mgr,
                                                           surfaceid);
//    ICO_TRA("CicoSCWlWinMgrIF::wlDestroySurfaceCB Leave");
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
//    ICO_TRA("CicoSCWlWinMgrIF::wlOutputGeometryCB Enter");

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
//    ICO_TRA("CicoSCWlWinMgrIF::wlOutputGeometryCB Leave");
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
//    ICO_TRA("CicoSCWlWinMgrIF::wlOutputModeCB Enter");

    if (NULL == data) {
        ICO_WRN("wlOutputGeometryCB: data is null");
        return;
    }
    static_cast<CicoSCWlWinMgrIF*>(data)->outputModeCB(data, wl_output, flags,
                                                       width, height, refresh);
//    ICO_TRA("CicoSCWlWinMgrIF::wlOutputModeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-application protocol error callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_application wayland ivi-application interface
 *  @param [in] error_code      error code
 *  @param [in] error_text      error message
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviAppErrorCB(void *data,
                                 struct ivi_application *ivi_application,
                                 int32_t error_code, const char *error_text)
{
    ICO_TRA("CicoSCWlWinMgrIF::wlIviAppErrorCB: Enter(%d,%s)",
            error_code, error_text ? error_text : "(null)");

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviAppErrorCB: data is null");
        return;
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviAppErrorCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-application protocol create wl_surface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_application wayland ivi-application interface
 *  @param [in] pid             application process id
 *  @param [in] title           surface title name
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviAppNativeShellInfoCB(void *data,
                                            struct ivi_application *ivi_application,
                                            int32_t pid, const char *title)
{
    struct creation_surface_wait    *tp;
    struct creation_surface_wait    *bp;
    uint32_t        nowtime;

    ICO_TRA("CicoSCWlWinMgrIF::wlIviAppNativeShellInfoCB: Enter(%d,<%s>)",
            pid, title ? title : "(null)");

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviAppNativeShellInfoCB: data is null");
        return;
    }

    // bind wl_surface to ivi_surface
    ivi_controller_get_native_handle(m_ivi_ctrl, pid, title ? title : "");

    // save pid and title(window name)
    nowtime = (time(NULL) >> 2) & 0x3fffffff;
    tp = m_wait_surface_creation;
    while (tp)  {
        if ((tp->pid == pid) && (tp->title[0] == 0))    break;
        tp = tp->next;
    }
    if (! tp)   {
        tp = m_free_surface_creation;
        if (tp)  {
            m_free_surface_creation = tp->next;
        }
        else    {
            tp = m_wait_surface_creation;
            bp = NULL;
            while (tp)  {
                if (((nowtime >= tp->create_time)
                         && ((nowtime - tp->create_time) > 15)) ||
                    ((nowtime < tp->create_time)
                         && ((nowtime + 0x40000000 - tp->create_time) > 15)))   {
                    if (bp) {
                        bp->next = tp->next;
                    }
                    else    {
                        m_wait_surface_creation = tp->next;
                    }
                    break;
                }
                bp = tp;
                tp = tp->next;
            }
            if (! tp)   {
                tp = (struct creation_surface_wait *)
                       malloc(sizeof(struct creation_surface_wait));
                if (! tp)    {
                    ICO_ERR("CicoSCWlWinMgrIF::wlIviAppNativeShellInfoCB: out of memory");
                    return;
                }
            }
        }
        memset(tp, 0, sizeof(struct creation_surface_wait));
        tp->next = m_wait_surface_creation;
        m_wait_surface_creation = tp;
    }
    tp->pid = pid;
    strncpy(tp->title, title, ICO_SYC_MAX_WINNAME_LEN-1);
    tp->create_time = nowtime;

    if (tp->id_surface) {
        static_cast<CicoSCWlWinMgrIF*>(data)->updateWinnameCB(tp->id_surface, tp->title);
    }

    ICO_TRA("CicoSCWlWinMgrIF::wlIviAppNativeShellInfoCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-controller protocol create screen callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller interface
 *  @param [in] id_screen       screen id
 *  @param [in] screen          screen information
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlScreenCB(void *data,
                                    struct ivi_controller *ivi_controller,
                                    uint32_t id_screen,
                                    struct ivi_controller_screen *screen)
{
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlScreenCB: Enter(%x)", id_screen);

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviCtrlScreenCB: data is null");
        return;
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlScreenCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-controller protocol create layer callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller interface
 *  @param [in] id_layer        layer id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlLayerCB(void *data,
                                   struct ivi_controller *ivi_controller,
                                   uint32_t id_layer)
{
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlLayerCB: Enter(%x)", id_layer);

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviCtrlLayerCB: data is null");
        return;
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlLayerCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-controller protocol create surface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller interface
 *  @param [in] id_surface      surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlSurfaceCB(void *data,
                                     struct ivi_controller *ivi_controller,
                                     uint32_t id_surface)
{
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlSurfaceCB: Enter(%x)", id_surface);

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviCtrlSurfaceCB: data is null");
        return;
    }

    static_cast<CicoSCWlWinMgrIF*>(data)->createSurfaceCB(data, ivi_controller, id_surface);

    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlSurfaceCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-controller protocol error callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller interface
 *  @param [in] error_code      error code
 *  @param [in] error_text      error message
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlErrorCB(void *data,
                                   struct ivi_controller *ivi_controller,
                                   int32_t object_id, int32_t object_type,
                                   int32_t error_code, const char *error_text)
{
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlErrorCB: Enter(%d[%d],%d,<%s>)",
            object_id, object_type, error_code, error_text ? error_text : "(null)");

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviCtrlErrorCB: data is null");
        return;
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlErrorCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland ivi-shell ivi-controller protocol error callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ivi_controller  wayland ivi-controller interface
 *  @param [in] error_code      error code
 *  @param [in] error_text      error message
 */
//--------------------------------------------------------------------------
void
CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB(void *data,
                                          struct ivi_controller *ivi_controller,
                                          struct wl_surface *surface)
{
    uint32_t                        id_surface;
    struct creation_surface_wait    *tp;

    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: Enter(surface=%08x)", (int)surface);

    if (NULL == data) {
        ICO_WRN("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: data is null");
        return;
    }

    // check same surface
    tp = m_wait_surface_creation;
    while (tp)  {
        if (tp->surface == surface) {
            ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: Leave(same surface)");
            return;
        }
        tp = tp->next;
    }

    // create ivi-surface and bind to wl_surface
    m_id_surface ++;
    if (m_id_surface >= 0x00ffffff)     m_id_surface = 1;
    id_surface = m_id_surface | 0x40000000;

    if (ivi_application_surface_create(m_ivi_app, id_surface, surface) == NULL) {
        ICO_ERR("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: "
                "ivi_application_surface_create(%x) Error", id_surface);
        if (m_wait_surface_creation)    {
            tp = m_wait_surface_creation;
            m_wait_surface_creation = tp->next;
            tp->next = m_free_surface_creation;
            m_free_surface_creation = tp;
        }
    }
    else    {
        tp = m_wait_surface_creation;
        while (tp)  {
            if (tp->id_surface == 0)    {
                tp->id_surface = id_surface;
                tp->surface = surface;
                break;
            }
            tp = tp->next;
        }
        if (! tp)   {
            ICO_ERR("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: waiting surface dose not exist");
        }
    }
    ICO_TRA("CicoSCWlWinMgrIF::wlIviCtrlNativeHandleCB: Leave(id_surface=%08x)", id_surface);
}
// vim:set expandtab ts=4 sw=4:
