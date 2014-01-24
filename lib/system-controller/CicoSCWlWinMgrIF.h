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
#include <ico_window_mgr-client-protocol.h>

#include "CicoSCWaylandIF.h"

//--------------------------------------------------------------------------
/**
 *  @brief  This class is wayland interface of multi window manager
 */
//--------------------------------------------------------------------------
class CicoSCWlWinMgrIF : public CicoSCWaylandIF {
public:
    virtual void initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version);

    virtual void createdCB(void *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid,
                           const char *winname,
                           int32_t pid,
                           const char *appid,
                           int32_t layertype);

    virtual void nameCB(void *data,
                        struct ico_window_mgr *ico_window_mgr,
                        uint32_t surfaceid,
                        const char *winname);

    virtual void destroyedCB(void *data,
                             struct ico_window_mgr *ico_window_mgr,
                             uint32_t surfaceid);

    virtual void visibleCB(void *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid,
                           int32_t visible,
                           int32_t raise,
                           int32_t hint);

    virtual void configureCB(void *data,
                             struct ico_window_mgr *ico_window_mgr,
                             uint32_t surfaceid,
                             uint32_t node,
                             int32_t layertype,
                             uint32_t layer,
                             int32_t x,
                             int32_t y,
                             int32_t width,
                             int32_t height,
                             int32_t hint);

    virtual void activeCB(void *data,
                          struct ico_window_mgr *ico_window_mgr,
                          uint32_t surfaceid,
                          int32_t active);

    virtual void layerVisibleCB(void *data,
                                struct ico_window_mgr *ico_window_mgr,
                                uint32_t layer,
                                int32_t visible);

    virtual void appSurfacesCB(void *data,
                               struct ico_window_mgr *ico_window_mgr,
                               const char *appid,
                               int32_t    pid,
                               struct wl_array *surfaces);

    virtual void mapSurfaceCB(void *data,
                              struct ico_window_mgr *ico_window_mgr,
                              int32_t event,
                              uint32_t surfaceid,
                              uint32_t type,
                              uint32_t target,
                              int32_t width,
                              int32_t height,
                              int32_t stride,
                              uint32_t format);

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

    virtual void outputModeCB(void             *data,
                              struct wl_output *wl_output,
                              uint32_t         flags,
                              int32_t          width,
                              int32_t          height,
                              int32_t          refresh);

protected:
    // default constructor
    CicoSCWlWinMgrIF();

    // destructor
    virtual ~CicoSCWlWinMgrIF();

    // assignment operator
    CicoSCWlWinMgrIF& operator=(const CicoSCWlWinMgrIF &object);

    // copy constructor
    CicoSCWlWinMgrIF(const CicoSCWlWinMgrIF &object);

    // wrapper function ico_window_mgr_declare_manager
    void declareManager(int32_t manager);

    // wrapper function ico_window_mgr_set_window_layer
    void setWindowLayer(uint32_t surfaceid, uint32_t layer);

    // wrapper function ico_window_mgr_set_positionsize
    void setPositionsize(uint32_t surfaceid, uint32_t node,
                         int32_t x, int32_t y, int32_t width,
                         int32_t height, int32_t flags);

    // wrapper function ico_window_mgr_set_visible
    void setVisible(uint32_t surfaceid, int32_t  visible,
                    int32_t  raise, int32_t  flags);

    // wrapper function ico_window_mgr_visible_animation
    void visibleAnimation(uint32_t surfaceid, int32_t visible,
                          int32_t x, int32_t y,
                          int32_t width, int32_t height);

    // wrapper function of ico_window_mgr_set_animation
    void setAnimation(uint32_t surfaceid, int32_t type,
                      const char *animation, int32_t time);

    // wrapper function of ico_window_mgr_set_attributes
    void setAttributes(uint32_t surfaceid, uint32_t attributes);

    // wrapper function of ico_window_mgr_set_active
    void setActive(uint32_t surfaceid, int32_t active);

    // wrapper function of ico_window_mgr_set_layer_visible
    void setLayerVisible(uint32_t layer, int32_t visible);

    // wrapper function of ico_window_mgr_get_surfaces
    void getSurfaces(const char *appid, int32_t pid);

    // wrapper function of ico_window_mgr_set_map_buffer
    void setmapBuffer(const char *shmname, int bufsize, int bufnum);

    // wrapper function of ico_window_mgr_map_surface
    void mapSurface(uint32_t surfaceid, int32_t framerate);

    // wrapper function of ico_window_mgr_unmap_surface
    void unmapSurface(uint32_t surfaceid);

private:
    // ico_window_mgr(Multi Window Manager) callback functions
    static void wlCreatedCB(void                  *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t              surfaceid,
                            const char            *winname,
                            int32_t               pid,
                            const char            *appid,
                            int32_t               layertype);

    static void wlNameCB(void *data,
                         struct ico_window_mgr *ico_window_mgr,
                         uint32_t surfaceid,
                         const char *winname);

    static void wlDestroyedCB(void *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t surfaceid);

    static void wlVisibleCB(void *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t surfaceid,
                            int32_t visible,
                            int32_t raise,
                            int32_t hint);

    static void wlConfigureCB(void *data,
                              struct ico_window_mgr *ico_window_mgr,
                              uint32_t surfaceid,
                              uint32_t node,
                              int32_t layertype,
                              uint32_t layer,
                              int32_t x,
                              int32_t y,
                              int32_t width,
                              int32_t height,
                              int32_t hint);

    static void wlActiveCB(void *data,
                           struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid,
                           int32_t active);

    static void wlLayerVisibleCB(void *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 uint32_t layer,
                                 int32_t visible);

    static void wlAppSurfacesCB(void *data,
                                struct ico_window_mgr *ico_window_mgr,
                                const char *appid,
                                int32_t    pid,
                                struct wl_array *surfaces);

    static void wlMapSurfaceCB(void *data,
                               struct ico_window_mgr *ico_window_mgr,
                               int32_t event,
                               uint32_t surfaceid,
                               uint32_t type,
                               uint32_t target,
                               int32_t width,
                               int32_t height,
                               int32_t stride,
                               uint32_t format);

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

protected:
    // ico_window_mgr listener
    struct ico_window_mgr_listener m_listener;

    // wayland output listener
    struct wl_output_listener m_wlOutputListener;

    // Wayland's Window Manager PlugIn instance
    struct ico_window_mgr *m_winmgr;

    // wayland output instance
    struct wl_output *m_wloutput;

};
#endif  // __CICO_SC_WL_WINMGR_IF_H__
// vim:set expandtab ts=4 sw=4:
