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
 *  @file   CicoSCWlWinMgrIF.h
 *
 *  @brief  This file is definition of CicoSCWlWinMgrIF class
 */
//==========================================================================
#ifndef __CICO_SC_WL_WINMGR_IF_H__
#define __CICO_SC_WL_WINMGR_IF_H__

#include <wayland-client.h>
#include <ilm/ilm_client.h>
#include <ilm/ilm_control.h>
#include <ico-uxf-weston-plugin/ico_window_mgr-client-protocol.h>
#include <weston/ivi-controller-client-protocol.h>
#include <weston/ivi-application-client-protocol.h>

#include "ico_syc_type.h"
#include "CicoSCWaylandIF.h"
#include "CicoSCWindow.h"

//--------------------------------------------------------------------------
/**
 *  @brief  This class is wayland interface of multi window manager
 */
//--------------------------------------------------------------------------
struct creation_surface_wait    {
    struct creation_surface_wait    *next;
    int32_t     pid;
    uint32_t    id_surface;
    struct wl_surface *surface;
    int32_t     busy;
    char        title[ICO_SYC_MAX_WINNAME_LEN];
};
#define SCWINMGR_GENIVI_BUSY_NONE       0
#define SCWINMGR_GENIVI_BUSY_REQSURF    1
#define SCWINMGR_GENIVI_BUSY_REQBIND    2
#define SCWINMGR_GENIVI_BUSY_WAIT       3

class CicoSCWlWinMgrIF : public CicoSCWaylandIF {
public:
    virtual void initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version);

    virtual void activeCB(void *data,
                          struct ico_window_mgr *ico_window_mgr,
                          uint32_t surfaceid,
                          int32_t select);

    virtual void mapSurfaceCB(void *data,
                              struct ico_window_mgr *ico_window_mgr,
                              int32_t event,
                              uint32_t surfaceid,
                              uint32_t type,
                              int32_t width,
                              int32_t height,
                              int32_t stride,
                              uint32_t format);

    virtual void updateSurfaceCB(void *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 uint32_t surfaceid,
                                 int visible,
                                 int srcwidth,
                                 int srcheight,
                                 int x,
                                 int y,
                                 int width,
                                 int height);

    virtual void destroySurfaceCB(void *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t surfaceid);

    virtual void updateWinnameCB(uint32_t surfaceid,
                                 const char *winname);
    //
    virtual void outputGeometryCB(void             *data,
                                  struct wl_output *wl_output,
                                  int32_t          x,
                                  int32_t          y,
                                  int32_t          physical_width,
                                  int32_t          physical_height,
                                  int32_t          subpixel,
                                  const char       *make,
                                  const char       *model,
                                  int32_t          transform);

    virtual void outputModeCB(void              *data,
                              struct wl_output  *wl_output,
                              uint32_t          flags,
                              int32_t           width,
                              int32_t           height,
                              int32_t           refresh);

    virtual void createSurfaceCB(void           *data,
                                 struct ivi_controller *ivi_controller,
                                 uint32_t id_surface);

    static void wlIviCtrlRemoveSurface(uint32_t id_surface);

protected:
    // default constructor
    CicoSCWlWinMgrIF();

    // destructor
    virtual ~CicoSCWlWinMgrIF();

    // assignment operator
    CicoSCWlWinMgrIF& operator=(const CicoSCWlWinMgrIF &object);

    // copy constructor
    CicoSCWlWinMgrIF(const CicoSCWlWinMgrIF &object);

    // wrapper function ico_window_mgr_set_window_layer
    void setWindowLayer(uint32_t surfaceid, uint32_t layer, uint32_t oldlayer);

    // wrapper function ico_window_mgr_set_positionsize
    void setPositionsize(uint32_t surfaceid, uint32_t node,
                         int32_t x, int32_t y, int32_t width, int32_t height);

    // wrapper function ico_window_mgr_set_visible
    void setVisible(uint32_t surfaceid, int32_t visible);

    // wrapper function of ico_window_mgr_set_animation
    void setAnimation(uint32_t surfaceid, int32_t type,
                      const char *animation, int32_t time);

    // wrapper function of ico_window_mgr_set_active
    void setActive(uint32_t surfaceid, int32_t active);

    // wrapper function of ico_window_mgr_set_layer_visible
    void setLayerVisible(uint32_t layer, int32_t visible);

    // wrapper function of ilm_takeSurfaceScreenshot
    void setmapGet(int surfaceid, const char *filepath);

    // wrapper function of ico_window_mgr_map_surface
    void mapSurface(uint32_t surfaceid, int32_t framerate, const char *filepath);

    // wrapper function of ico_window_mgr_unmap_surface
    void unmapSurface(uint32_t surfaceid);

    static const char *wlIviCtrlGetSurfaceWaiting(uint32_t id_surface, int *pid);

private:
    // ico_window_mgr(Multi Window Manager) callback functions
    static void wlActiveCB(void *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid,
                           int32_t active);

    static void wlMapSurfaceCB(void *data,
                               struct ico_window_mgr *ico_window_mgr,
                               int32_t event,
                               uint32_t surfaceid,
                               uint32_t type,
                               int32_t width,
                               int32_t height,
                               int32_t stride,
                               uint32_t format);

    static void wlUpdateSurfaceCB(void *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t surfaceid,
                                  int layer,
                                  int srcwidth,
                                  int srcheight,
                                  int x,
                                  int y,
                                  int width,
                                  int height);

    static void wlDestroySurfaceCB(void *data,
                                   struct ico_window_mgr *ico_window_mgr,
                                   uint32_t surfaceid);
    //
    static void wlOutputGeometryCB(void             *data,
                                   struct wl_output *wl_output,
                                   int32_t          x,
                                   int32_t          y,
                                   int32_t          physical_width,
                                   int32_t          physical_height,
                                   int32_t          subpixel,
                                   const char       *make,
                                   const char       *model,
                                   int32_t          transform);

    static void wlOutputModeCB(void             *data,
                               struct wl_output *wl_output,
                               uint32_t         flags,
                               int32_t          width,
                               int32_t          height,
                               int32_t          refresh);

    static void wlIviAppNativeShellInfoCB(void          *data,
                                          struct ivi_application *ivi_application,
                                          int32_t       pid,
                                          const char    *title);
    static void wlIviCtrlScreenCB(void                  *data,
                                  struct ivi_controller *ivi_controller,
                                  uint32_t              id_screen,
                                  struct ivi_controller_screen *screen);

    static void wlIviCtrlLayerCB(void                   *data,
                                 struct ivi_controller  *ivi_controller,
                                 uint32_t               id_layer);

    static void wlIviCtrlSurfaceCB(void                 *data,
                                   struct ivi_controller *ivi_controller,
                                   uint32_t             id_surface);

    static void wlIviCtrlErrorCB(void                   *data,
                                 struct ivi_controller  *ivi_controller,
                                 int32_t                object_id,
                                 int32_t                object_type,
                                 int32_t                error_code,
                                 const char             *error_text);

    static void wlIviCtrlNativeHandleCB(void            *data,
                                        struct ivi_controller *ivi_controller,
                                        struct wl_surface *surface);

protected:
    // ico_window_mgr listener
    struct ico_window_mgr_listener m_listener;

    // wayland output listener
    struct wl_output_listener m_wlOutputListener;

    // genivi ivi-application listener
    struct ivi_application_listener m_ivi_app_listener;

    // genivi ivi-controller listener
    struct ivi_controller_listener m_ivi_ctrl_listener;

    // Wayland's Window Manager PlugIn instance
    static struct ico_window_mgr *m_winmgr;

    // Wayland's genivi ivi_application instance
    static struct ivi_application *m_ivi_app;

    // Wayland's genivi ivi_controller instance
    static struct ivi_controller *m_ivi_ctrl;

    // Wayland's genivi ivi_controller_surface instance
    static struct ivi_controller_surface *m_ivi_ctrl_surf;

    // wayland output instance
    static struct wl_output *m_wloutput;

    // surface id for wayland/weston applications
    static int m_id_surface;

    // creation surface title name
    static struct creation_surface_wait *m_wait_surface_creation;
    static struct creation_surface_wait *m_free_surface_creation;
};
#endif  // __CICO_SC_WL_WINMGR_IF_H__
// vim:set expandtab ts=4 sw=4:
