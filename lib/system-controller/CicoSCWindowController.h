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
 *  @file   CicoSCWindowController.h
 *
 *  @brief  This file is definition of CicoSCWindowController class
 */
//==========================================================================
#ifndef __CICO_SC_WINDOW_CONTROLLER_H__
#define __CICO_SC_WINDOW_CONTROLLER_H__

#include <vector>
#include <map>

using namespace std;

#include "CicoSCWlWinMgrIF.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCCommand;
class CicoSCDisplay;
class CicoSCLayer;
class CicoSCDisplayZone;
class CicoSCWindow;
class CicoSCResourceManager;

//--------------------------------------------------------------------------
/**
 *  @brief  This class is controller of window
 */
//--------------------------------------------------------------------------
class CicoSCWindowController : public CicoSCWlWinMgrIF
{
public:
    // default constructor
    CicoSCWindowController();

    // copy constructor
    virtual ~CicoSCWindowController();

    int initDB(void);

    void handleCommand(const CicoSCCommand * cmd);

    //
    void setResourceManager(CicoSCResourceManager *resMgr);

    int show(int         surfaceid,
             const char  *animation,
             int         animationTime);

    int hide(int         surfaceid,
             const char  *animation,
             int         animationTime);

    int resize(int        surfaceid,
               int        w,
               int        h,
               const char *animation,
               int        animationTime);

    int move(int        surfaceid,
             int        nodeid,
             int        x,
             int        y,
             const char *animation,
             int        animationTime);

    int setGeometry(int        surfaceid,
                    int        nodeid,
                    int        x,
                    int        y,
                    int        w,
                    int        h,
                    const char *resizeAnimation,
                    int        resizeAnimationTime,
                    const char *moveAnimation,
                    int        moveAnimationTime);

    int raise(int surfaceid,
             const char  *animation,
             int         animationTime);

    int lower(int surfaceid, const char *animation, int animationTime);

    int setWindowLayer(int surfaceid, int layerid);

    int showLayer(int displayid, int layerid);

    int hideLayer(int displayid, int layerid);

    int active(int surfaceid, int target);

    int mapSurface(int surfaceid, int framerate);

    int unmapSurface(int surfaceid);

    //
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
                              int32_t width,
                              int32_t height,
                              int32_t stride,
                              int32_t format);

    virtual void outputGeometryCB(void *data,
                                  struct wl_output *wl_output,
                                  int32_t x,
                                  int32_t y,
                                  int32_t physical_width,
                                  int32_t physical_height,
                                  int32_t subpixel,
                                  const char *make,
                                  const char *model,
                                  int32_t transform);

    virtual void outputModeCB(void *data,
                              struct wl_output *wl_output,
                              uint32_t flags,
                              int32_t width,
                              int32_t height,
                              int32_t refresh);

private:
    // assignment operator
    CicoSCWindowController& operator=(const CicoSCWindowController &object);

    // copy constructor
    CicoSCWindowController(const CicoSCWindowController &object);

    // find window object by surface id
    CicoSCWindow* findWindow(int surfaceid);

    // find layer object by display id and layer id
    CicoSCLayer* findLayer(int displayid, int layerid);

    // fine display zone by id
    const CicoSCDisplayZone * findDisplayZone(int zoneid);

    int notifyResourceManager(int        surfaceid,
                              const char *animation,
                              int        animationTime);

private:
    CicoSCResourceManager *m_resMgr;

    // window object list
    map<unsigned int, CicoSCWindow*> m_windowList;

    // display object list
    vector<CicoSCDisplay*>   m_displayList;

};
#endif  // __CICO_SC_WINDOW_CONTROLLER_H__
// vim:set expandtab ts=4 sw=4:
