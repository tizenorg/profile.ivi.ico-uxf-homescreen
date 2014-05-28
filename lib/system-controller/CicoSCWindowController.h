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

#define ICO_SC_APPID_DEFAULT_ONS    "org.tizen.ico.onscreen"
#define ICO_SC_LAYERID_SCREENBASE   1000

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
    // get instance of CicoSCWindowController
    static CicoSCWindowController* getInstance();

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
                    int        layerid,
                    int        x,
                    int        y,
                    int        w,
                    int        h,
                    const char *resizeAnimation,
                    int        resizeAnimationTime,
                    const char *moveAnimation,
                    int        moveAnimationTime);

    int setGeometry(int        surfaceid,
                    const char *zone,
                    int        layerid,
                    const char *resizeAnimation,
                    int        resizeAnimationTime,
                    const char *moveAnimation,
                    int        moveAnimationTime);

    int raise(int surfaceid,
             const char  *animation,
             int         animationTime);

    int lower(int surfaceid, const char *animation, int animationTime);

    int setWindowAnimation(int surfaceid, int type, const char *animation, int time);

    int setWindowLayer(int surfaceid, int layerid);

    int showLayer(int displayid, int layerid);

    int hideLayer(int displayid, int layerid);

    int active(int surfaceid, int target);

    int setmapGet(int surfaceid, const char *filepath);

    int mapSurface(int surfaceid, int framerate, const char *filepath);

    int unmapSurface(int surfaceid);

    int getDisplayedWindow(int zoneid);

    int setAttributes(int surfaceid);

    void initializeGeniviLMS(void);

    static void wlGeniviLayerNotification(t_ilm_layer layer,
                                          struct ilmLayerProperties *LayerProperties,
                                          t_ilm_notification_mask mask);
    //
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

    virtual void createSurfaceCB(void           *data,
                                 struct ivi_controller *ivi_controller,
                                 uint32_t id_surface);

    const CicoSCWindow* findWindowObj(int32_t pid, uint32_t surfaceid) const;

    const CicoSCResourceManager* getResourceManager(void) const;

#if 1   /* change show/hide to move (Weston may stop drawing to undisplayed Surface) */
    static Eina_Bool ChangeToHide(void *window);
#endif  /* change show/hide to move (Weston may stop drawing to undisplayed Surface) */

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

    void raiselower(CicoSCWindow *window, bool raise);

    int notifyResourceManager(int        surfaceid,
                              const char *zone,
                              int        layerid,
                              const char *animation,
                              int        animationTime);

private:
    /// my instance
    static CicoSCWindowController *ms_myInstance;

    // resource manager instance
    CicoSCResourceManager *m_resMgr;

    // window object list
    map<unsigned int, CicoSCWindow*> m_windowList;

    // display object list
    vector<CicoSCDisplay*> m_displayList;

    // total of physical display
    unsigned int m_physicalDisplayTotal;
};
#endif  // __CICO_SC_WINDOW_CONTROLLER_H__
// vim:set expandtab ts=4 sw=4:
