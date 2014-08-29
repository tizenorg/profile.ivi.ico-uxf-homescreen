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
#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
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
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

class CicoSCWlWinMgrIF : public CicoSCWaylandIF {
public:
    virtual void initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version);

    virtual void activeCB(void *data,
                          uint32_t surfaceid,
                          int32_t select);

    virtual void destroySurfaceCB(void *data,
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
#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    static void wlIviCtrlRemoveSurface(uint32_t id_surface);
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

protected:
    // default constructor
    CicoSCWlWinMgrIF();

    // destructor
    virtual ~CicoSCWlWinMgrIF();

    // assignment operator
    CicoSCWlWinMgrIF& operator=(const CicoSCWlWinMgrIF &object);

    // copy constructor
    CicoSCWlWinMgrIF(const CicoSCWlWinMgrIF &object);

    // wrapper function  GENIVI-LM set layer
    void setWindowLayer(uint32_t surfaceid, uint32_t layer, uint32_t oldlayer);

    // wrapper function GENIVI-LM set position and size
    void setPositionsize(uint32_t surfaceid, uint32_t node,
                         int32_t x, int32_t y, int32_t width, int32_t height);

    // wrapper function GENIVI-LM set visibility
    void setVisible(uint32_t surfaceid, int32_t visible);

    // wrapper function of GENIVI-LM set active
    void setActive(uint32_t surfaceid, int32_t active);

    // wrapper function of GENIVI-LM set layer visibility
    void setLayerVisible(uint32_t layer, int32_t visible);

    // wrapper function of ilm_takeSurfaceScreenshot
    void setmapGet(int surfaceid, const char *filepath);

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    static const char *wlIviCtrlGetSurfaceWaiting(uint32_t id_surface, int *pid);
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

private:
    // GENIVI-LM callback functions
    static void wlActiveCB(void *data,
                           uint32_t surfaceid,
                           int32_t active);

    static void wlDestroySurfaceCB(void *data,
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

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    static void wlIviAppNativeShellInfoCB(void          *data,
                                          struct ivi_application *ivi_application,
                                          int32_t       pid,
                                          const char    *title);
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

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

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    static void wlIviCtrlNativeHandleCB(void            *data,
                                        struct ivi_controller *ivi_controller,
                                        struct wl_surface *surface);
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

protected:
    // wayland output listener
    struct wl_output_listener m_wlOutputListener;

    // genivi ivi-controller listener
    struct ivi_controller_listener m_ivi_ctrl_listener;

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    // genivi ivi-application listener
    struct ivi_application_listener m_ivi_app_listener;
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

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

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    // creation surface title name
    static struct creation_surface_wait *m_wait_surface_creation;
    static struct creation_surface_wait *m_free_surface_creation;
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */
};
#endif  // __CICO_SC_WL_WINMGR_IF_H__
// vim:set expandtab ts=4 sw=4:
