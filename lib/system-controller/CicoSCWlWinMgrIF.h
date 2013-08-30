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
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_WL_WINMGR_IF_H__
#define __CICO_SC_WL_WINMGR_IF_H__

#include <wayland-client.h>
#include <ico_window_mgr-client-protocol.h>

#include "CicoSCWaylandIF.h"

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
                           const char *appid);

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

private:
    // ico_window_mgr(Multi Window Manager) callback functions
    static void wlCreatedCB(void                  *data,
                            struct ico_window_mgr *ico_window_mgr,
                            uint32_t              surfaceid,
                            const char            *winname,
                            int32_t               pid,
                            const char            *appid);

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
    static struct ico_window_mgr_listener ms_listener;

    // wayland output listener
    static struct wl_output_listener ms_wlOutputListener;

    // Wayland's Window Manager PlugIn instance
    struct ico_window_mgr *m_winmgr;

    // wayland output instance
    struct wl_output *m_wloutput;

    // wayland shm instance
    struct wl_shm *m_wlshm;

    // shared memory temp file name
    char m_shmName[256];

    // shared memory temp file name
    static const int ICO_WL_SHM_SIZE = 16 * 1024 * 1024;
    
    // wayland shared memory  pool
    struct wl_shm_pool *m_wlshmpool;

};
#endif	// __CICO_SC_WL_WINMGR_IF_H__
// vim:set expandtab ts=4 sw=4:
