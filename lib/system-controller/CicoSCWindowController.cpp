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
 *  @file   CicoSCWindowController.cpp
 *
 *  @brief  This file implementation of CicoSCWindowController class
 */
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Ecore.h>

#include <vector>
#include <algorithm>
using namespace std;

#include "CicoSCWindow.h"
#include "CicoSCWayland.h"
#include "CicoSCWindowController.h"
#include "CicoLog.h"

#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoSCDisplay.h"
#include "CicoSCLayer.h"
#include "CicoSCDisplayZone.h"
#include "ico_syc_error.h"
#include "ico_syc_type.h"
#include "ico_syc_winctl.h"
#include "CicoSCCommand.h"
#include "ico_syc_msg_cmd_def.h"
#include "CicoSCServer.h"
#include "CicoSCMessage.h"
#include "CicoSCMessageRes.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCResourceManager.h"

//==========================================================================
//  private static variable
//==========================================================================
CicoSCWindowController* CicoSCWindowController::ms_myInstance = NULL;

struct ico_sc_win_check_t* CicoSCWindowController::win_check_timer = NULL;
struct ico_sc_win_check_t* CicoSCWindowController::win_check_free = NULL;

//==========================================================================
//  static function for Ecore timer
//==========================================================================
static Ecore_Timer* g_SCWindowChecker = 0;
static struct timeval   g_lasttime = { 0, 0 };
static int              g_hightimer = 0;

//--------------------------------------------------------------------------
/**
 *  @brief  get instance of CicoSCWindowController
 *
 *  @return instance of CicoSCWindowController
 */
//--------------------------------------------------------------------------
CicoSCWindowController*
CicoSCWindowController::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCWindowController();
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWindowController::CicoSCWindowController()
    : m_resMgr(NULL), m_physicalDisplayTotal(0)
{
    CicoSCWayland* wayland = CicoSCWayland::getInstance();
    wayland->getInstance()->addWaylandIF(ICO_WL_IVI_CONTROLLER_IF, this);
#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    wayland->getInstance()->addWaylandIF(ICO_WL_IVI_APPLICATION_IF, this);
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */
    wayland->getInstance()->addWaylandIF(ICO_WL_OUTPUT_IF, this);
    initDB();

    CicoSCWindowController::ms_myInstance = this;

    g_SCWindowChecker = ecore_timer_add(((double)ICO_SC_WINCHK_TIMER_BASE_L)/1000.0,
                                        ico_SCWindowChecker, this);
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWindowController::~CicoSCWindowController()
{
    CicoSCWindowController::ms_myInstance = NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize display and window database
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::initDB(void)
{
    ICO_TRA("CicoSCWindowController::initDB: Enter");

    const vector<CicoSCDisplayConf*>& displayList =
            CicoSystemConfig::getInstance()->getDisplayConfList();
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = displayList.begin();
    for (; itr != displayList.end(); ++itr) {
        CicoSCDisplay *display = new CicoSCDisplay();
        display->displayid = (*itr)->id;
        display->type      = (*itr)->type;
        display->nodeid    = (*itr)->node;
        display->displayno = (*itr)->no;
        display->width     = (*itr)->width;
        display->height    = (*itr)->height;
        display->name      = (*itr)->name;

        vector<CicoSCLayerConf*>::const_iterator itr2;
        itr2 = (*itr)->layerConfList.begin();
        for (; itr2 != (*itr)->layerConfList.end(); ++itr2) {
            CicoSCLayer *layer = new CicoSCLayer();
            layer->layerid     = (*itr2)->id;
            layer->type        = (*itr2)->type;
            layer->width       = display->width;
            layer->height      = display->height;
            layer->displayid   = display->displayid;
            layer->menuoverlap = (*itr2)->menuoverlap;
            display->layerList.push_back(layer);
        }

        vector<CicoSCDisplayZoneConf*>::const_iterator itr3;
        itr3 = (*itr)->zoneConfList.begin();
        for (; itr3 != (*itr)->zoneConfList.end(); ++itr3) {
            CicoSCDisplayZone *zone = new CicoSCDisplayZone();
            zone->zoneid   = (*itr3)->id;
            zone->x        = (*itr3)->x;
            zone->y        = (*itr3)->y;
            zone->width    = (*itr3)->w;
            zone->height   = (*itr3)->h;
            zone->fullname = (*itr3)->fullname;
            zone->aspectFixed       = (*itr3)->aspectFixed;
            zone->aspectAlignLeft   = (*itr3)->aspectAlignLeft;
            zone->aspectAlignRight  = (*itr3)->aspectAlignRight;
            zone->aspectAlignTop    = (*itr3)->aspectAlignTop;
            zone->aspectAlignBottom = (*itr3)->aspectAlignBottom;
            display->zoneList[zone->zoneid] = zone;
        }
        display->dump();
        m_displayList.push_back(display);
    }

    ICO_TRA("CicoSCWindowController::initDB: Leave");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set resource manager instance
 *
 *  @param [in] resMgr  resource manager instance
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::setResourceManager(CicoSCResourceManager *resMgr)
{
    m_resMgr = resMgr;
}

//--------------------------------------------------------------------------
/**
 *  @brief   show a target window
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::show(int        surfaceid,
                             const char *animation,
                             int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::show Enter"
            "(surfaceid=%08x animation=%s type=%x animationTime=%d)",
            surfaceid, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::show Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->visible = true;

    // update current displayed window at display zone
    CicoSCDisplayZone* zone = (CicoSCDisplayZone*)findDisplayZone(window->zoneid);
    if (NULL != zone) {
        CicoSCLayer *layer = findLayer(window->displayid, window->layerid);
        if ((NULL != layer) && (layer->type == ICO_LAYER_TYPE_APPLICATION)) {
            ICO_DBG("Entry display zone[%d] current displayed window"
                    "(%08x:\"%s\")",
                    zone->zoneid, window->surfaceid, window->appid.c_str());
            zone->displayedWindow = window;
        }
    }

    // set animation request
    int raiseFlag = ICO_SYC_WIN_RAISE_NOCHANGE;
    if (type & ICO_SYC_WIN_SURF_RAISE)  {
        raiseFlag = ICO_SYC_WIN_RAISE_RAISE;
    }
    else if (type & ICO_SYC_WIN_SURF_LOWER) {
        raiseFlag = ICO_SYC_WIN_RAISE_LOWER;
    }
    else if (((type & ICO_SYC_WIN_SURF_NOCHANGE) == 0) &&
             (false == window->raise))   {
        raiseFlag = ICO_SYC_WIN_RAISE_RAISE;
    }
    if ((animationTime > 0) && (NULL != animation) &&
        (animation[0] != '\0') && (strcasecmp(animation, "none") != 0)) {
        // set animation
        setWindowAnimation(window, ICO_SC_WINANIMA_SHOW, animation, animationTime);
    }
    else    {
        // reset animation
        resetWindowAnimation(window);
    }

    // set raise/lower request (if need)
    if (raiseFlag != ICO_SYC_WIN_RAISE_NOCHANGE)    {
        raiselower(window, (raiseFlag == ICO_SYC_WIN_RAISE_RAISE));
    }

    // set visible request
    CicoSCWlWinMgrIF::setVisible(window->surfaceid, ICO_SYC_WIN_VISIBLE_SHOW);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::show Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   raise/lower a target window
 *
 *  @param [in] window          target window
 *  @param [in] raise           raise(true)/lower(false)
 *
 *  @return nothing
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::raiselower(CicoSCWindow *window, bool raise)
{
    ICO_TRA("CicoSCWindowController::raiselower(%08x,%d)", window->surfaceid, raise);

    window->raise = raise;

    CicoSCLayer* layer = findLayer(window->displayid, window->layerid);
    if (! layer)    {
        ICO_ERR("CicoSCWindowController::raiselower: surface.%08x has no layer(%d)",
                window->surfaceid, window->layerid);
        return;
    }
    if ((raise) && (layer->is_topSurface(window->surfaceid)))   {
        ICO_DBG("CicoSCWindowController::raiselower %08x raise but top",
                window->surfaceid);
    }
    else if ((! raise) && (layer->is_buttomSurface(window->surfaceid))) {
        ICO_DBG("CicoSCWindowController::raiselower %08x lower but buttom",
                window->surfaceid);
    }
    else    {
        layer->addSurface(window->surfaceid, raise);

        int nsurf;
        const int *surfs = layer->getSurfaces(&nsurf);
        ICO_TRA("CicoSCWindowControllerCicoSCWindowController layer.%d %d.%x %x %x %x",
                window->layerid, nsurf, surfs[0], surfs[1], surfs[2], surfs[3]);
        if (nsurf > 4)  {
            ICO_TRA("CicoSCWindowControllerCicoSCWindowController          .%x %x %x %x",
                    surfs[4], surfs[5], surfs[6], surfs[7]);
        }
        if (ilm_layerSetRenderOrder(window->layerid, (t_ilm_layer *)surfs, nsurf)
            != ILM_SUCCESS) {
            ICO_ERR("CicoSCWindowController::raiselower "
                    "ilm_layerSetRenderOrder(%d,,%d) Error", window->layerid, nsurf);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   hide a target window
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::hide(int        surfaceid,
                             const char *animation,
                             int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::hide Enter"
            "(surfaceid=%08x animation=%s type=%x animationTime=%d)",
            surfaceid, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::hide Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    if (false == window->visible) {
        ICO_DBG("already hide state");
        ICO_TRA("CicoSCWindowController::hide Leave(EOK)");
        return ICO_SYC_EOK;
    }

    // update window attr
    window->visible = false;

    // update current displayed window at display zone
    CicoSCDisplayZone* zone = (CicoSCDisplayZone*)findDisplayZone(window->zoneid);
    if (NULL != zone) {
        CicoSCLayer *layer = findLayer(window->displayid, window->layerid);
        if ((NULL != layer) && (layer->type == ICO_LAYER_TYPE_APPLICATION) &&
            (getDisplayedWindow(zone->zoneid) == surfaceid)) {
            ICO_DBG("Exit  display zone[%d] current displayed window"
                    "(%08x:\"%s\")",
                    zone->zoneid, window->surfaceid, window->appid.c_str());
            zone->displayedWindow = NULL;
        }
    }

    if ((animationTime > 0) && (NULL != animation) &&
        (animation[0] != '\0') && (strcasecmp(animation, "none") != 0)) {
        // set animation
        setWindowAnimation(window, ICO_SC_WINANIMA_HIDE, animation, animationTime);
    }
    else    {
        // reset animation
        resetWindowAnimation(window);
        CicoSCWlWinMgrIF::setVisible(window->surfaceid, ICO_SYC_WIN_VISIBLE_HIDE);
    }

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::hide Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   resize window(surface) size
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] w               window width
 *  @param [in] h               window height
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(window dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::resize(int        surfaceid,
                               int        w,
                               int        h,
                               const char *animation,
                               int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::resize Enter"
            "(surfaceid=%08x h=%d w=%d animation=%s type=%x animationTime=%d)",
            surfaceid, w, h, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::resize Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set visible request
    window->width = w;
    window->height = h;
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid, window->nodeid,
                                      window->x, window->y, w, h);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::resize Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  move window(surface) position
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] nodeid          node id
 *  @param [in] x               window width
 *  @param [in] y               window height
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(window dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::move(int        surfaceid,
                             int        nodeid,
                             int        x,
                             int        y,
                             const char *animation,
                             int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::move Enter"
            "(surfaceid=%08x nodeid=%d x=%d y=%d "
            "animation=%s type=%x animationTime=%d)",
            surfaceid, nodeid, x, y, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::move not found window(%08x)", surfaceid);
        ICO_TRA("CicoSCWindowController::move Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }
    // check nodeid
    if (nodeid >= (int)m_physicalDisplayTotal)  {
        ICO_WRN("CicoSCWindowController::move not found node(%d)", nodeid);
        ICO_TRA("CicoSCWindowController::move Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    int moveNodeId = ICO_SYC_WIN_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }

    // set visible request
    window->x = x;
    window->y = y;
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid,
                                      moveNodeId, x, y,
                                      window->width, window->height);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::move Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  raise window(surface)
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::raise(int        surfaceid,
                              const char *animation,
                              int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::raise Enter"
            "(surfaceid=%08x animation=%s type=%x animationTime=%d)",
            surfaceid, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::raise Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set raise request
    raiselower(window, true);
    if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::raise ilm_commitChanges() Error");
    }

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::raise Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window(surface) geometry
 *
 *  @param [in] surfaceid           wayland surface id
 *  @param [in] nodeid              node id
 *  @param [in] layerid             layer id
 *  @param [in] x                   window x position
 *  @param [in] y                   window y position
 *  @param [in] w                   window width
 *  @param [in] h                   window height
 *  @param [in] resizeAnimation     resize animation name
 *  @param [in] resizeAnimationTime resize animation time
 *  @param [in] moveAnimation       move animation name
 *  @param [in] moveanimationTime   move animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(window dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setGeometry(int        surfaceid,
                                    int        nodeid,
                                    int        layerid,
                                    int        x,
                                    int        y,
                                    int        w,
                                    int        h,
                                    const char *resizeAnimation,
                                    int        resizeAnimationTime,
                                    const char *moveAnimation,
                                    int        moveAnimationTime)
{
    ICO_TRA("CicoSCWindowController::setGeometry Enter"
            "(surfaceid=%08x nodeid=%d layerid=%d x=%d y=%d w=%d h=%d "
            "resizeAnimation=%s resizeAnimationTime=%d "
            "moveAnimation=%s moveAnimationTime=%d)",
            surfaceid, nodeid, layerid, x, y, w, h,
            resizeAnimation, resizeAnimationTime,
            moveAnimation, moveAnimationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::setGeometry not found window(%08x)", surfaceid);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }
    // check nodeid
    if (nodeid >= (int)m_physicalDisplayTotal)  {
        ICO_WRN("CicoSCWindowController::setGeometry not found node(%d)", nodeid);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    int moveNodeId = ICO_SYC_WIN_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }
    else {
        moveNodeId = window->nodeid;
    }

    if (m_physicalDisplayTotal <= (unsigned int)moveNodeId) {
        ICO_WRN("nodeid(%d) is over physical display total(%d)",
                m_physicalDisplayTotal, nodeid);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }

    int moveX = window->x;
    if (0 <= x) {
        moveX = x;
        window->x = x;
    }

    int moveY = window->y;
    if (0 <= y) {
        moveY = y;
        window->y = y;
    }

    int moveW = window->width;
    if (0 <= w) {
        moveW = w;
        window->width = w;
    }

    int moveH = window->height;
    if (0 <= h) {
        moveH = h;
        window->height = h;
    }

    // set window layer
    if (0 <= layerid) {
        setWindowLayer(window->surfaceid, layerid);
    }

    // update window attr
    window->nodeid = moveNodeId;

    // set visible request
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid, moveNodeId,
                                      moveX, moveY, moveW, moveH);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::setGeometry Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window(surface) geometry
 *
 *  @param [in] surfaceid           wayland surface id
 *  @param [in] layerid             layer id
 *  @param [in] nodeid              node id
 *  @param [in] zone                display zone name
 *  @param [in] resizeAnimation     resize animation name
 *  @param [in] resizeAnimationTime resize animation time
 *  @param [in] moveAnimation       move animation name
 *  @param [in] moveanimationTime   move animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(window dose not exist)
 *  @retval ICO_SYC_EINVAL      error(zone is null, zone name is invalid)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setGeometry(int        surfaceid,
                                    const char *zone,
                                    int        layerid,
                                    const char *resizeAnimation,
                                    int        resizeAnimationTime,
                                    const char *moveAnimation,
                                    int        moveAnimationTime)
{
    ICO_TRA("CicoSCWindowController::setGeometry Enter"
            "(surfaceid=%08x zone=%s layerid=%d "
            "resizeAnimation=%s resizeAnimationTime=%d "
            "moveAnimation=%s moveAnimationTime=%d)",
            surfaceid, zone, layerid,
            resizeAnimation, resizeAnimationTime,
            moveAnimation, moveAnimationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::setGeometry not found window(%08x)", surfaceid);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // find zone information
    if ((NULL == zone) || ('\0' == zone[0])) {
        ICO_WRN("zone value is invalid");
        ICO_TRA("CicoSCWindowController::setGeometry Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }

    vector<CicoSCDisplay*>::iterator itr;
    itr = m_displayList.begin();
    CicoSCDisplayZone* dispzone = NULL;
    int displayno = 0;
    for (; itr != m_displayList.end(); ++itr) {
        dispzone = (*itr)->findDisplayZonebyFullName(zone);
        if (NULL != dispzone) {
            displayno = (*itr)->displayno;
            break;
        }
    }
    if (NULL == dispzone) {
        ICO_WRN("display zone name(%s) is invalid.", zone);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }
    if (m_physicalDisplayTotal <= (unsigned int)displayno) {
        ICO_WRN("nodeid(%d) is over physical display total(%d)",
                m_physicalDisplayTotal, displayno);
        ICO_TRA("CicoSCWindowController::setGeometry Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }

    if (window->zoneid != dispzone->zoneid) {
        // update current displayed window at display zone
        CicoSCDisplayZone* olddispzone = (CicoSCDisplayZone*)findDisplayZone(window->zoneid);
        if (NULL != olddispzone) {
            CicoSCLayer *layer = findLayer(window->displayid, window->layerid);
            if ((NULL != layer) &&
                (layer->type == ICO_LAYER_TYPE_APPLICATION) &&
                (getDisplayedWindow(olddispzone->zoneid) == surfaceid)) {
                ICO_DBG("Exit display zone[%d] current displayed window"
                        "(%08x:\"%s\")",
                        olddispzone->zoneid, window->surfaceid,
                        window->appid.c_str());
                olddispzone->displayedWindow = NULL;
            }
        }

        CicoSCLayer *layer = findLayer(displayno, window->layerid);
        if ((NULL != layer) && (layer->type == ICO_LAYER_TYPE_APPLICATION)) {
            ICO_DBG("Entry display zone[%d] current displayed window"
                    "(%08x:\"%s\")",
                    dispzone->zoneid, window->surfaceid, window->appid.c_str());
            dispzone->displayedWindow = window;
        }
    }

    // update window attr
    window->zoneid = dispzone->zoneid;
    window->zone = dispzone->fullname;
    setAttributes(window->surfaceid);

    int ret = setGeometry(surfaceid, displayno, layerid,
                          dispzone->x, dispzone->y,
                          dispzone->width, dispzone->height,
                          resizeAnimation, resizeAnimationTime,
                          moveAnimation, moveAnimationTime);

    ICO_TRA("CicoSCWindowController::setGeometry Leave(EOK)");
    return ret;
}

//--------------------------------------------------------------------------
/**
 *  @brief  lower window(surface)
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] animation       animation name
 *  @param [in] animationTime   animation time
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::lower(int        surfaceid,
                              const char *animation,
                              int        animationTime)
{
    int     type = animationTime & ICO_SYC_WIN_SURF_FLAGS;
    animationTime &= ~ICO_SYC_WIN_SURF_FLAGS;

    ICO_TRA("CicoSCWindowController::lower Enter"
            "(surfaceid=08x animation=%s type=%x animationTime=%d)",
            surfaceid, animation, type, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::lower Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set lower request
    raiselower(window, false);
    if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::lower ilm_commitChanges() Error");
    }

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::lower Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window layer
 *
 *  @param [in] surfaceid       wayland surface id
 *  @param [in] layer           layer id
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK         success
 *  @retval ICO_SYC_ESRCH       error(not initialized)
 *  @retval ICO_SYC_ENOENT      error(window or layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setWindowLayer(int surfaceid, int layerid)
{
    ICO_TRA("CicoSCWindowController::setWindowLayer Enter"
            "(surfaceid=%08x layerid=%x)", surfaceid, layerid);

    // find window information in window list
    CicoSCWindow* window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }
    if (window->layerid == layerid) {
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(same layer %d)", layerid);
        return ICO_SYC_EOK;
    }

    // find new layer information in layer list
    CicoSCLayer* layer = findLayer(window->displayid, layerid);
    if (NULL == layer) {
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT[disp=%d,layer=%d])",
                window->displayid, layerid);
        return ICO_SYC_ENOENT;
    }

    // find old layer information and remove surface if need
    CicoSCLayer* oldlayer = findLayer(window->displayid, window->layerid);
    if (oldlayer)   {
        if (ilm_layerRemoveSurface(window->layerid, window->surfaceid) != ILM_SUCCESS)  {
            ICO_ERR("CicoSCWindowController::setWindowLayer ilm_layerRemoveSurface(%d,%08x)"
                    " Error", window->layerid, window->surfaceid);
        }
        // must need ilm_commitChanges() after ilm_layerRemoveSurface()
        if (ilm_commitChanges() != ILM_SUCCESS) {
            ICO_ERR("CicoSCWindowController::setWindowLayer ilm_commitChanges Error");
        }

        oldlayer->removeSurface(window->surfaceid);
        int noldsurf;
        const int *oldsurfs = oldlayer->getSurfaces(&noldsurf);
        if (ilm_layerSetRenderOrder(window->layerid, (t_ilm_layer *)oldsurfs, noldsurf)
            != ILM_SUCCESS)   {
            ICO_ERR("CicoSCWindowController::setWindowLayer "
                "ilm_layerSetRenderOrder(%d,,%d) Error", window->layerid, noldsurf);
        }
        if (ilm_commitChanges() != ILM_SUCCESS) {
            ICO_ERR("CicoSCWindowController::setWindowLayer ilm_commitChanges Error");
        }
    }

    // update window attr
    window->layerid = layerid;

    // set window layer request
    if (ilm_layerAddSurface(window->layerid, window->surfaceid) != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::setWindowLayer ilm_layerAddSurface(%d,%08x) Error",
                window->layerid, window->surfaceid);
    }
    if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::setWindowLayer ilm_commitChanges() Error");
    }

    // add surface to new layer
    layer->addSurface(window->surfaceid, true);
    int nsurf;
    const int *surfs = layer->getSurfaces(&nsurf);
    if (ilm_layerSetRenderOrder(window->layerid, (t_ilm_layer *)surfs, nsurf)
        != ILM_SUCCESS)   {
        ICO_ERR("CicoSCWindowController::setWindowLayer "
                "ilm_layerSetRenderOrder(%d,,%d) Error", window->layerid, nsurf);
    }
    if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::setWindowLayer ilm_commitChanges() Error");
    }

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::setWindowLayer Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   show layer
 *
 *  @param [in] displayid   display id
 *  @param [in] layerid     layer id
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::showLayer(int displayid, int layerid)
{
    ICO_TRA("CicoSCWindowController::showLayer Enter"
            "(displayid=%d layerid=%d)", displayid, layerid);

    // find layer information in layer list
    CicoSCLayer* layer = findLayer(displayid, layerid);
    if (NULL == layer) {
        ICO_WRN("not found layer information");
        ICO_TRA("CicoSCWindowController::showLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    CicoSCWlWinMgrIF::setLayerVisible(layerid, ICO_SYC_LAYER_VISIBLE_SHOW);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::showLayer Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   show layer
 *
 *  @param [in] displayid   display id
 *  @param [in] layerid     layer id
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::hideLayer(int displayid, int layerid)
{
    ICO_TRA("CicoSCWindowController::hideLayer Enter(displayid=%d layerid=%d)",
            displayid, layerid);

    // find layer information in layer list
    CicoSCLayer* layer = findLayer(displayid, layerid);
    if (NULL == layer) {
        ICO_WRN("not found layer information");
        ICO_TRA("CicoSCWindowController::hideLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    CicoSCWlWinMgrIF::setLayerVisible(layerid, ICO_SYC_LAYER_VISIBLE_HIDE);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::hideLayer Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   active window(surface)
 *
 *  @param [in] surfaceid   wayland surface id
 *  @param [in] target      target(pointer and/or keyboard)
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::active(int surfaceid, int target)
{
    CicoSCWindow *window = NULL;

    ICO_TRA("CicoSCWindowController::active Enter"
            "(surfaceid=%08x, target=%08x)", surfaceid, target);

    // find window information in window list
    if (surfaceid) {
        window = findWindow(surfaceid);
        if (NULL == window) {
            ICO_WRN("not found window information");
            ICO_TRA("CicoSCWindowController::active Leave(ENOENT)");
            return ICO_SYC_ENOENT;
        }
    }

    if (target < 0) {
        target = 0xffff;
    }

    // set active request
    CicoSCWlWinMgrIF::setActive(surfaceid, target);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    // update visible attr
    if (window) {
        window->raise = true;
    }

    // update active window
    map<unsigned int, CicoSCWindow*>::iterator itr;
    itr = m_windowList.begin();
    for (; itr != m_windowList.end(); ++itr) {
        CicoSCWindow* window = itr->second;
        if ((surfaceid != 0) && (window->surfaceid == surfaceid)) {
            window->active = target;
        }
        else {
            if (target == 0) {
                window->active = 0;
            }
            else {
                window->active &= ~target;
            }
        }
    }

    ICO_TRA("CicoSCWindowController::active Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set surface map get
 *
 *  @param [in] surfaceid   wayland surface id
 *  @param [in] filepath    surface image pixel file path
 *
 *  @return ICO_SYC_EOK on success, other on error(currently EOK only)
 *  @retval ICO_SYC_EOK     success
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setmapGet(int surfaceid, const char *filepath)
{
    ICO_TRA("CicoSCWindowController::setmapGet Enter");

    CicoSCWlWinMgrIF::setmapGet(surfaceid, filepath);
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::setmapGet Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   map surface
 *
 *  @param [in] surfaceid   wayland surface id
 *  @param [in] framerate   surface change frame rate(frames par sec)
 *  @param [in] filepath    surface image pixel file path
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::mapSurface(int surfaceid, int framerate, const char *filepath)
{
    ICO_TRA("CicoSCWindowController::mapSurface Enter(%08x,%d,%s)",
            surfaceid, framerate, filepath ? filepath : "(null)");

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::mapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    setWindowMap(window, framerate, filepath);

    ICO_TRA("CicoSCWindowController::mapSurface Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   unmap surface
 *
 *  @param [in] surfaceid   wayland surface id
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::unmapSurface(int surfaceid)
{
    ICO_TRA("CicoSCWindowController::unmapSurface Enter(%08x)", surfaceid);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::unmapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    resetWindowMap(surfaceid);

    ICO_TRA("CicoSCWindowController::unmapSurface Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   get displayed window by display zone id
 *
 *  @param [in] zoneid  display zone id
 *
 *  @return surface id on success, -1 on not displayed
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::getDisplayedWindow(int zoneid)
{
    const CicoSCDisplayZone* zone = findDisplayZone(zoneid);
    if (NULL == zone) {
        return -1;
    }

    if (NULL == zone->displayedWindow) {
        return -1;
    }

    return zone->displayedWindow->surfaceid;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set surface attributes
 *
 *  @param [in] surfaceid   surface id
 *
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setAttributes(int surfaceid)
{
    ICO_TRA("CicoSCWindowController::setAttributes Enter(surfaceid=%08x)", surfaceid);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setAttributes Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // find zone
    const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
    if (NULL == zone) {
        ICO_WRN("not found zone information");
        ICO_TRA("CicoSCWindowController::setAttributes Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    uint32_t attributes = 0;
    if (true == zone->aspectFixed) {
        attributes |= ICO_SYC_WIN_ASPECT_FIXED;
        if (true == zone->aspectAlignLeft) {
            attributes |= ICO_SYC_WIN_ASPECT_ALIGN_LEFT;
        }
        if (true == zone->aspectAlignRight) {
            attributes |= ICO_SYC_WIN_ASPECT_ALIGN_RIGHT;
        }
        if (true == zone->aspectAlignTop) {
            attributes |= ICO_SYC_WIN_ASPECT_ALIGN_TOP;
        }
        if (true == zone->aspectAlignBottom) {
            attributes |= ICO_SYC_WIN_ASPECT_ALIGN_BOTTOM;
        }
    }
    ICO_TRA("CicoSCWindowController::setAttributes Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] surfaceid       surface Id
 *  @param [in] select          select device(unused)
 *                              (0=not active/1=pointer/2=touch)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::activeCB(void                  *data,
                                 uint32_t              surfaceid,
                                 int32_t               select)
{
    ICO_TRA("CicoSCWindowController::activeCB Enter(surfaceid=%08x select=%d)",
            surfaceid, select);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::activeCB Leave(not found window)");
        return;
    }

    if (0 == select) {
        ICO_TRA("CicoSCWindowController::activeCB Leave(not active)");
        return;
    }

    if (0 != window->active) {
        ICO_TRA("CicoSCWindowController::activeCB Leave(already active)");
        return;
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_ACTIVE);
    message->addRootObject("appid", window->appid);
    message->addRootObject("pid", window->pid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::activeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [in] event           event
 *  @param [in] surfaceid       surface Id
 *  @param [in] type            surface type (EGL buffer/Shared memory)
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] stride          surface buffer(frame buffer) stride
 *  @param [in] format          surface buffer format
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::mapSurfaceCB(int32_t                event,
                                     uint32_t               surfaceid,
                                     uint32_t               type,
                                     int32_t                width,
                                     int32_t                height,
                                     int32_t                stride,
                                     uint32_t               format)
{
    int     command;

    ICO_TRA("CicoSCWindowController::mapSurfaceCB Enter(ev=%d surf=%d[%x] w/h=%d/%d)",
            event, surfaceid, surfaceid, width, height);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_TRA("CicoSCWindowController::mapSurfaceCB Leave(NOENT)");
        return;
    }

    // convert event to command
    switch (event)  {
    case ICO_SC_WINCHK_MAP_SURFACE_EVENT_MAP:
    case ICO_SC_WINCHK_MAP_SURFACE_EVENT_CONTENTS:
        command = MSG_CMD_MAP_THUMB;
        break;
    case ICO_SC_WINCHK_MAP_SURFACE_EVENT_UNMAP:
        command = MSG_CMD_UNMAP_THUMB;
        break;
    default:
        ICO_TRA("CicoSCWindowController::mapSurfaceCB Leave(Unknown event(%d))", event);
        return;
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", command);
    message->addRootObject("appid", window->appid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("attr", type);
    message->addArgObject("width", width);
    message->addArgObject("height", height);
    message->addArgObject("stride", stride);
    message->addArgObject("format", format);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::mapSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface update event callback from GENIVI-LM
 *
 *  @param [in] surfaceid       surface Id
 *  @param [in] surfprop        surface Properties
 *  @param [in] maek            attribute change mask bits(unused)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::wlGeniviSurfaceNotification(t_ilm_surface               surfaceid,
                                                    struct ilmSurfaceProperties *surfprop,
                                                    t_ilm_notification_mask     mask)
{
    ICO_TRA("CicoSCWindowController::wlGeniviSurfaceNotification(%d(%x),,%x)",
            surfaceid, surfaceid, mask);

    if (mask & (ILM_NOTIFICATION_VISIBILITY | ILM_NOTIFICATION_SOURCE_RECT |
                ILM_NOTIFICATION_DEST_RECT))    {
        CicoSCWindowController::getInstance()->
                updateSurfaceCB((uint32_t)surfaceid, surfprop);
    }
    else if (mask & ILM_NOTIFICATION_CONTENT_REMOVED)   {
        CicoSCWindowController::getInstance()->
                destroySurfaceCB((uint32_t)surfaceid);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface update event callback
 *
 *  @param [in] surfaceid       surface Id
 *  @param [in] surfprop        surface Properties
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::updateSurfaceCB(uint32_t                    surfaceid,
                                        struct ilmSurfaceProperties *surfprop)
{
    ICO_TRA("CicoSCWindowController::updateSurfaceCB: Enter"
            "(%08x pid=%d v=%d,x/y=%d/%d,w/h=%d/%d->%d/%d)",
            surfaceid, surfprop->creatorPid, surfprop->visibility,
            surfprop->destX, surfprop->destY,
            surfprop->sourceWidth, surfprop->sourceHeight,
            surfprop->destWidth, surfprop->destHeight);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::updateSurfaceCB: not found window(%08x)",
                surfaceid);
        ICO_TRA("CicoSCWindowController::updateSurfaceCB: Leave");
        return;
    }

    // update attr
    window->visible = surfprop->visibility;
    window->srcwidth = surfprop->sourceWidth;
    window->srcheight = surfprop->sourceHeight;
    window->x = surfprop->destX;
    window->y = surfprop->destY;
    window->width = surfprop->destWidth;
    window->height = surfprop->destHeight;
    window->nodeid = window->layerid / ICO_SC_LAYERID_SCREENBASE;

    // notify to homescreen
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_ATTR);
    message->addRootObject("appid", window->appid);
    const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
    if (NULL != zone) {
        message->addArgObject("zone", zone->fullname);
    }
    else {
        message->addArgObject("zone", "");
    }
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    message->addArgObject("node", window->nodeid);
    message->addArgObject("layer", window->layerid);
    message->addArgObject("pos_x", window->x);
    message->addArgObject("pos_y", window->y);
    message->addArgObject("width", window->width);
    message->addArgObject("height", window->height);
    message->addArgObject("raise", window->raise ? 1 : 0);
    message->addArgObject("visible", window->visible ? 1 : 0);
    message->addArgObject("active", window->active ? 1 : 0);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::updateSurfaceCB: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [in] surfaceid       surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::destroySurfaceCB(uint32_t surfaceid)
{
    ICO_TRA("CicoSCWindowController::destroySurfaceCB: Enter(%08x)", surfaceid);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window(%08x)", surfaceid);
        ICO_TRA("CicoSCWindowController::destroySurfaceCB Leave");
        return;
    }

    // remove window check
    CicoSCWindowController::windowCheckerRemove(surfaceid);

    // delete surface from layer
    CicoSCLayer* layer = findLayer(window->displayid, window->layerid);
    if (NULL == layer) {
        ICO_TRA("CicoSCWindowController::destroySurfaceCB: surf.%08x not exist in layer.%d",
                surfaceid, window->layerid);
    }
    else    {
        layer->removeSurface(surfaceid);
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_DESTROY);
    message->addRootObject("appid", window->appid);
    message->addRootObject("pid", window->pid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    // TODO delete window in application
    if (NULL != m_resMgr) {
        CicoSCCommand cmd;
        CicoSCCmdResCtrlOpt *opt = new CicoSCCmdResCtrlOpt();

        cmd.cmdid = MSG_CMD_DESTORY_RES;
        cmd.appid = window->appid;
        cmd.pid   = window->pid;
        cmd.opt = opt;

        opt->dispres   = true;
        opt->winname   = window->name;
        opt->surfaceid = window->surfaceid;

        string fullname;
        const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
        if (NULL != zone) {
            opt->dispzone = zone->fullname;
        }

        opt->soundres  = true;
        opt->soundname = window->appid;
        opt->soundid   = 0;
        CicoSystemConfig *sysconf = CicoSystemConfig::getInstance();
        const CicoSCDefaultConf *defconf = sysconf->getDefaultConf();
        if (NULL != defconf) {
            const CicoSCSoundZoneConf *zoneconf =
                sysconf->findSoundZoneConfbyId(defconf->soundzone);
            if (NULL != zoneconf) {
                opt->soundzone = zoneconf->fullname;
            }
        }
        m_resMgr->handleCommand((const CicoSCCommand&)cmd, true);
    }
    // delete window in list
    m_windowList.erase(window->surfaceid);
    delete(window);

    ICO_TRA("CicoSCWindowController::destroySurfaceCB: Leave");
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
CicoSCWindowController::updateWinnameCB(uint32_t surfaceid,
                                        const char *winname)
{
    ICO_TRA("CicoSCWindowController::updateWinnameCB: Enter(%08x,<%s>)",
            surfaceid, winname ? winname : "(null)");

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::updateWinnameCB: not found window(%08x)",
                surfaceid);
        ICO_TRA("CicoSCWindowController::updateWinnameCB Leave");
        return;
    }

    window->name = winname;

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NAME);
    message->addRootObject("appid", window->appid);
    message->addRootObject("pid", window->pid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::updateWinnameCB: Leave");
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
CicoSCWindowController::outputGeometryCB(void             *data,
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
    ICO_TRA("CicoSCWindowController::outputGeometryCB Enter"
            "(x=%d y=%d physical_width=%d physical_height=%d "
            "subpixel=%d make=%s model=%s transform=%d)",
            x, y, physical_width, physical_height,
            subpixel, make, model, transform);

    ++ m_physicalDisplayTotal;
    if (0 == m_displayList.size()) {
        ICO_TRA("CicoSCWindowController::outputGeometryCB Leave"
                "(display is zero)");
        return;
    }

    if (m_displayList.size() < m_physicalDisplayTotal) {
        ICO_TRA("CicoSCWindowController::outputGeometryCB Leave"
                "(display total unmatch)");
        return;
    }

    CicoSCDisplay* display = m_displayList.at(m_physicalDisplayTotal-1);
    switch (transform) {
    case WL_OUTPUT_TRANSFORM_90:
    case WL_OUTPUT_TRANSFORM_270:
    case WL_OUTPUT_TRANSFORM_FLIPPED_90:
    case WL_OUTPUT_TRANSFORM_FLIPPED_270:
        display->pWidth      = physical_height;
        display->pHeight     = physical_width;
        display->orientation = CicoSCDisplay::ORIENTATION_VERTICAL;
        break;
    case WL_OUTPUT_TRANSFORM_NORMAL:
    case WL_OUTPUT_TRANSFORM_180:
    case WL_OUTPUT_TRANSFORM_FLIPPED:
    case WL_OUTPUT_TRANSFORM_FLIPPED_180:
    default:
        display->pWidth      = physical_width;
        display->pHeight     = physical_height;
        display->orientation = CicoSCDisplay::ORIENTATION_HORIZONTAL;
        break;
    }
    display->dump();

    ICO_TRA("CicoSCWindowController::outputGeometryCB Leave");
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
CicoSCWindowController::outputModeCB(void             *data,
                                     struct wl_output *wl_output,
                                     uint32_t         flags,
                                     int32_t          width,
                                     int32_t          height,
                                     int32_t          refresh)
{
    ICO_TRA("CicoSCWindowController::outputModeCB Enter"
            "(flags=%d width=%d height=%d refresh=%d)",
            flags, width, height, refresh);

    if (0 == m_displayList.size()) {
        ICO_TRA("CicoSCWindowController::outputModeCB Leave(display is zero)");
        return;
    }

    CicoSCDisplay* display = m_displayList.at(0);
    if (flags & WL_OUTPUT_MODE_CURRENT) {
        if (display->orientation == CicoSCDisplay::ORIENTATION_VERTICAL) {
            display->pWidth  = height;
            display->pHeight = width;
        }
        else {
            display->pWidth  = width;
            display->pHeight = height;
        }
        display->dump();
    }

    ICO_TRA("CicoSCWindowController::outputModeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland genivi ivi-shell surface create callback
 *
 *  @param [in] data        user data
 *  @param [in] ivi_controller  wayland ivi-controller plugin interface
 *  @param [in] id_surface      surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::createSurfaceCB(void *data,
                                        struct ivi_controller *ivi_controller,
                                        uint32_t id_surface)
{
    struct ilmSurfaceProperties SurfaceProperties;

    ICO_TRA("CicoSCWindowController::createSurfaceCB Enter"
            "(surfaceid=%08x)", id_surface);

    if (ilm_getPropertiesOfSurface(id_surface, &SurfaceProperties) != ILM_SUCCESS)  {
        ICO_ERR("CicoSCWindowController::createSurfaceCB: ilm_getPropertiesOfSurface(%x) Error",
                id_surface);
        return;
    }
    ICO_TRA("createSurfaceCB: surface=%d(0x%08x) pid=%d w/h=%d/%d(%d/%d)->%d/%d",
            id_surface, id_surface, SurfaceProperties.creatorPid,
            SurfaceProperties.sourceWidth, SurfaceProperties.sourceHeight,
            SurfaceProperties.origSourceWidth, SurfaceProperties.origSourceHeight,
            SurfaceProperties.destWidth, SurfaceProperties.destHeight);

#if 1       /* 2014/08/20 workaround, SurfaceProperties.creatorPid not set */
    if (SurfaceProperties.creatorPid == 0)  {
        SurfaceProperties.creatorPid = (id_surface - 6000) / 100;
        ICO_WRN("createSurfaceCB: surface=%d(0x%08x) pid is Zero, workaround to %d",
                id_surface, id_surface, SurfaceProperties.creatorPid);
    }
#endif

    CicoSCWindow* window = new CicoSCWindow();
    window->surfaceid = id_surface;
    window->name = "";
    window->pid  = SurfaceProperties.creatorPid;
    window->displayid = 0;              // currently fixed 0
    window->raise = 1;                  // created surface is top of layer
    window->srcwidth = SurfaceProperties.sourceWidth;
    window->srcheight = SurfaceProperties.sourceHeight;
    window->width = SurfaceProperties.destWidth;
    window->height = SurfaceProperties.destHeight;
    window->layerid = 0;

#ifdef GENIVI_WL_SHELL_INFO         /* GENIVI-LM is supporting the wl_shell_info    */
    const char *title = CicoSCWlWinMgrIF::wlIviCtrlGetSurfaceWaiting(id_surface, &pid);

    if (title) {
        window->name = title;
        window->pid  = pid;
    }
    else {
        ICO_WRN("Could not find window title!");
    }
#endif /*GENIVI_WL_SHELL_INFO*/     /* GENIVI-LM is supporting the wl_shell_info    */

    CicoSCLifeCycleController* appctrl;
    appctrl = CicoSCLifeCycleController::getInstance();
    const CicoAilItems* ailItem = NULL;
    const CicoAulItems* aulitem = appctrl->findAUL(window->pid);
    if (NULL == aulitem) {
        /* client does not exist in AppCore, search parent process  */
        ICO_DBG("application information not found. search parent process");

        int     fd;
        int     cpid = window->pid;
        int     ppid;
        int     size;
        char    *ppid_line;
        char    procpath[PATH_MAX];

        while ((cpid > 1) && (aulitem == NULL)) {
            snprintf(procpath, sizeof(procpath)-1, "/proc/%d/status", cpid);

            fd = open(procpath, O_RDONLY);
            if (fd < 0)     break;

            size = read(fd, procpath, sizeof(procpath));
            close(fd);

            if (size <= 0)  break;
            ppid_line = strstr(procpath, "PPid");

            if (ppid_line == NULL)  break;
            ppid = 0;
            sscanf(ppid_line, "PPid:    %d", &ppid);
            if (ppid <= 1)  break;
            ICO_DBG("application pid=%d parent=%d", cpid, ppid);
            cpid = ppid;
            aulitem = appctrl->findAUL(cpid);
        }
    }
    if (NULL != aulitem) {
        window->appid = aulitem->m_appid;
        ICO_DBG("appid=%s", window->appid.c_str());
        ailItem = appctrl->findAIL(window->appid.c_str());

        window->layerid = ailItem->m_layer;
        window->zoneid  = ailItem->m_displayZone;
        window->nodeid  = ailItem->m_nodeID;
        if ((window->displayid >= 0) && (window->zoneid >= 0)) {

            const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
            if (NULL != zone) {
                window->zone   = zone->fullname;
                window->x      = zone->x;
                window->y      = zone->y;
                window->width  = zone->width;
                window->height = zone->height;
            }
        }
    }
    else{
        delete window;
        ICO_WRN("ail item not found.");
        ICO_TRA("CicoSCWindowController::createSurfaceCB Leave(ENOENT)");
        return;
    }

    if (ilm_surfaceSetDestinationRectangle(window->surfaceid, window->x, window->y,
                 window->width, window->height) != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::createSurfaceCB "
                "ilm_surfaceSetDestinationRectangle(%08x) Error", window->surfaceid);
    }
    else if (ilm_surfaceSetSourceRectangle(window->surfaceid, 0, 0,
                 window->width, window->height) != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::createSurfaceCB "
                "ilm_surfaceSetSourceRectangle(%08x) Error", window->surfaceid);
    }
    else if (ilm_surfaceSetOrientation(window->surfaceid, ILM_ZERO) != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::createSurfaceCB "
                "ilm_surfaceSetOrientation(%08x) Error", window->surfaceid);
    }
    else if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_commitChanges() Error");
    }

    CicoSCLayer *layer = findLayer(window->displayid, window->layerid);
    if (layer) {
        if (ilm_layerAddSurface(window->layerid, window->surfaceid) != ILM_SUCCESS) {
            ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_layerAddSurface(%d,%08x) "
                    "Error", window->layerid, window->surfaceid);
        }
        if (ilm_commitChanges() != ILM_SUCCESS)    {
            ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_commitChanges() Error");
        }

        layer->addSurface(window->surfaceid, true);
        int nsurf;
        const int *surfs = layer->getSurfaces(&nsurf);
        if (ilm_layerSetRenderOrder(window->layerid, (t_ilm_layer *)surfs, nsurf)
            != ILM_SUCCESS)   {
            ICO_ERR("CicoSCWindowController::createSurfaceCB: "
                    "ilm_layerSetRenderOrder(%d,,%d) Error", window->layerid, nsurf);
        }
        if (ilm_commitChanges() != ILM_SUCCESS)    {
            ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_commitChanges() Error");
        }
    }
    // must set surfaceOpacity after surfcaeAddLayer
    if (ilm_surfaceSetOpacity(window->surfaceid , (t_ilm_float)1.0f) != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::createSurfaceCB "
                            "ilm_surfaceSetOpacity(%08x) Error", window->surfaceid);
    }
    else if (ilm_commitChanges() != ILM_SUCCESS)    {
        ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_commitChanges() Error");
    }

    appctrl->enterAUL(window->appid.c_str(), window->pid, window);

    // dump log window information
    window->dump();

    m_windowList[window->surfaceid] = window;

    // set surface attributes
    setAttributes(window->surfaceid);

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CREATE);
    message->addRootObject("appid", window->appid);
    message->addRootObject("pid", window->pid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    if (0 == window->appid.compare(ICO_SC_APPID_DEFAULT_ONS)) {
        CicoSCMessageRes *msgOS = new CicoSCMessageRes();
        msgOS->addRootObject("command",   MSG_CMD_WINDOW_ID_RES);
        msgOS->addRootObject("appid",     window->appid);
        msgOS->addRootObject("pid",       window->pid);
        msgOS->addWinObject(MSG_PRMKEY_ECU,         "");
        msgOS->addWinObject(MSG_PRMKEY_DISPLAY,     "");
        msgOS->addWinObject(MSG_PRMKEY_LAYER,       "");
        msgOS->addWinObject(MSG_PRMKEY_LAYOUT,      "");
        msgOS->addWinObject(MSG_PRMKEY_AREA,        "");
        msgOS->addWinObject(MSG_PRMKEY_DISPATCHAPP, "");
        msgOS->addWinObject(MSG_PRMKEY_ROLE,        window->name);
        msgOS->addWinObject(MSG_PRMKEY_RESOURCEID,  window->surfaceid);
        CicoSCServer::getInstance()->sendMessage(window->appid,
                                                 (CicoSCMessage*)msgOS);
    }

    if (NULL != m_resMgr) {
        CicoSCCommand cmd;
        CicoSCCmdResCtrlOpt *opt = new CicoSCCmdResCtrlOpt();

        cmd.cmdid = MSG_CMD_CREATE_RES;
        cmd.appid = window->appid;
        cmd.pid   = window->pid;
        cmd.opt = opt;

        opt->dispres   = true;
        opt->winname   = window->name;
        opt->surfaceid = window->surfaceid;

        string fullname;
        const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
        if (NULL != zone) {
            opt->dispzone = zone->fullname;
        }

#if 1   //TODO
        opt->soundres  = true;
        opt->soundname = window->appid;
        opt->soundid   = 0;
        CicoSystemConfig *sysconf = CicoSystemConfig::getInstance();
        const CicoSCDefaultConf *defconf = sysconf->getDefaultConf();
        if (NULL != defconf) {
            const CicoSCSoundZoneConf *zoneconf =
                sysconf->findSoundZoneConfbyId(defconf->soundzone);
            if (NULL != zoneconf) {
                opt->soundzone = zoneconf->fullname;
            }
        }
#endif
        m_resMgr->handleCommand((const CicoSCCommand&)cmd, true);
    }
    else {
        show(window->surfaceid, NULL, 0);
    }

    // set surface change notification
    if (ilm_surfaceAddNotification(window->surfaceid, wlGeniviSurfaceNotification)
            != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::createSurfaceCB "
                "ilm_surfaceAddNotification(%08x) Error", window->surfaceid);
    }

    ICO_TRA("CicoSCWindowController::createSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   genivi ivi-shell layer propaty callback
 *
 *  @param [in] layer           layer id
 *  @param [in] LayerProperties layer properties
 *  @param [in] mask            change properties
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::wlGeniviLayerNotification(t_ilm_layer layer,
                                            struct ilmLayerProperties *LayerProperties,
                                            t_ilm_notification_mask mask)
{
    ICO_TRA("CicoSCWindowController::wlGeniviLayerNotification Enter(%x,,%x)", layer, mask);

    if (mask & ILM_NOTIFICATION_VISIBILITY) {
        // change layer visibility, send message to HomeScreen
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_CHANGE_LAYER_ATTR);
        message->addRootObject("appid", "");
        message->addArgObject("layer", layer);
        message->addArgObject("visible", LayerProperties->visibility ? 1 : 0);
        CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
    }
    ICO_TRA("CicoSCWindowController::wlGeniviLayerNotification Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  setup Genivi ivi-shell layer managerment system finctions
 *
 *  @param      none
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::initializeGeniviLMS(void)
{
    t_ilm_uint  NumberOfScreens = 0;
    t_ilm_layer *pLayerId, *ppLayerId;
    t_ilm_int   LayerNumber;
    t_ilm_uint  *pScreenIds;
    struct ilmScreenProperties  ScreenProperties;
    int                     idxs, idx1, idx2;
    int                     DisplayId, LayerId;
    const CicoSCDisplayConf *DisplayConf;
    const CicoSCLayerConf   *LayerConf;

    // get all screen id
    if ((ilm_getScreenIDs(&NumberOfScreens, &pScreenIds) != ILM_SUCCESS) ||
        (NumberOfScreens <= 0)) {
        ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                "ilm_getScreenIDs() Error(num=%d)", NumberOfScreens);
        return;
    }
    ICO_TRA("initializeGeniviLMS: Screens=%d.%x %x", NumberOfScreens, pScreenIds[0],
            NumberOfScreens >= 2 ? pScreenIds[1] : 0);
    if ((int)NumberOfScreens > CicoSystemConfig::getInstance()->getNumberofDisplay())   {
        ICO_WRN("CicoSCWindowController::initializeGeniviLMS # of screens physical=%d config=%d",
                NumberOfScreens, CicoSystemConfig::getInstance()->getNumberofDisplay());
        NumberOfScreens = (t_ilm_uint)CicoSystemConfig::getInstance()->getNumberofDisplay();
    }
#if 1           /* At present, GENIVI (ivi-controller) is processing only one Display   */
    for (idxs = (int)NumberOfScreens - 1; idxs >= 0; idxs--)
#else
    for (idxs = 0; idxs < (int)NumberOfScreens; idxs++)
#endif
    {
        if (ilm_getPropertiesOfScreen(pScreenIds[idxs], &ScreenProperties) != ILM_SUCCESS)   {
            ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                    "ilm_getPropertiesOfScreen(%d.%x) Error", idxs, pScreenIds[idxs]);
            continue;
        }
        // It is referred to as Center when there is only one display
        if (NumberOfScreens == 1)   {
            DisplayId = CicoSystemConfig::getInstance()->getDisplayIdbyType(ICO_NODETYPE_CENTER);
        }
        else    {
            DisplayId = CicoSystemConfig::getInstance()->getDisplayIdbyNo((int)pScreenIds[idxs]);
        }
        ICO_TRA("CicoSCWindowController::initializeGeniviLMS: "
                "Screen[%d.%x] w/h=%d/%d layers=%d DisplayId=%d",
                idxs, pScreenIds[idxs], ScreenProperties.screenWidth,
                ScreenProperties.screenHeight, ScreenProperties.layerCount, DisplayId);

        if ((DisplayId < 0) ||
            ((DisplayConf = CicoSystemConfig::getInstance()->findDisplayConfbyId(DisplayId))
                == NULL))   {
            ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                    "ScreenId.%x not found", pScreenIds[idxs]);
        }
        else    {
            // set genivi layers
            for (idx1 = 0; ; idx1++)  {
                LayerConf = CicoSystemConfig::getInstance()->
                                                findLayerConfbyIdx(DisplayId, idx1);
                if (! LayerConf)    break;
            }
            pLayerId = (t_ilm_layer *)malloc(sizeof(t_ilm_layer) * idx1);
            ppLayerId = pLayerId;
            LayerNumber = 0;

            for (idx1 = 0; ; idx1++)  {
                LayerConf = CicoSystemConfig::getInstance()->
                                                findLayerConfbyIdx(DisplayId, idx1);
                if (! LayerConf)    break;

                LayerId = LayerConf->id + DisplayId * ICO_SC_LAYERID_SCREENBASE;
                for (idx2 = 0; idx2 < LayerNumber; idx2++)  {
                    if (LayerId == (int)pLayerId[idx2]) break;
                }
                if (idx2 < LayerNumber) {
                    ICO_TRA("CicoSCWindowController::initializeGeniviLMS: "
                            "layer.%d exist, Skip", LayerId);
                    continue;
                }

                if (ilm_layerCreateWithDimension((t_ilm_layer *)&LayerId,
                            DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)    {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_layerCreateWithDimension(%d,%d,%d) Error",
                            LayerId, DisplayConf->width, DisplayConf->height);
                }
                else if (ilm_commitChanges() != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_commitChanges() Error");
                }
                else if (ilm_layerSetOpacity(LayerId, (t_ilm_float)1.0f) != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_layerSetOpacity(%d) Error", LayerId);
                }
                else if (ilm_layerSetSourceRectangle(LayerId, 0, 0,
                             DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_layerSetSourceRectangle(%d) Error", LayerId);
                }
                else if (ilm_layerSetDestinationRectangle(LayerId, 0, 0,
                             DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_layerSetDestinationRectangle(%d) Error", LayerId);
                }
                else if (ilm_layerSetOrientation(LayerId, ILM_ZERO) != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_layerSetOrientation(%d) Error", LayerId);
                }
                else    {
                    if (ilm_commitChanges() != ILM_SUCCESS) {
                        ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                                "ilm_commitChanges() Error");
                    }
                    // SystemController default is layer visible
                    if ((ilm_layerSetVisibility(LayerId, 1) != ILM_SUCCESS) ||
                        (ilm_commitChanges() != ILM_SUCCESS))   {
                        ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                                "ilm_layerSetVisibility() Error");
                    }
                    ICO_TRA("initializeGeniviLMS: layer=%d created(%d,%d)",
                            LayerId, DisplayConf->width, DisplayConf->height);
                    *ppLayerId = LayerId;
                    ppLayerId ++;
                    LayerNumber ++;
                }
            }
            if (LayerNumber > 0)    {
                ICO_TRA("initializeGeniviLMS: layers %d.%d %d %d %d set to screen %x",
                        LayerNumber, pLayerId[0], pLayerId[1], pLayerId[2], pLayerId[3],
                        pScreenIds[idxs]);
                if (ilm_displaySetRenderOrder(pScreenIds[idxs], pLayerId, LayerNumber)
                    != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_displaySetRenderOrder(%d) Error", LayerNumber);
                }
                else if (ilm_commitChanges() != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                            "ilm_commitChanges() Error");
                }
                ppLayerId = pLayerId;
                for (idx2 = 0; idx2 < LayerNumber; idx2++) {
                    if (ilm_layerAddNotification(*ppLayerId, wlGeniviLayerNotification)
                        != ILM_SUCCESS)   {
                        ICO_ERR("CicoSCWindowController::initializeGeniviLMS "
                                "ilm_layerAddNotification(%d) Error", *ppLayerId);
                    }
                    ppLayerId ++;
                }
            }
            free(pLayerId);
        }
    }
    free(pScreenIds);
}

//==========================================================================
// private method
//==========================================================================

//--------------------------------------------------------------------------
/**
 *  @brief  find window object by surfaceid
 *
 *  @param [in] surfaceid   wayland surface id
 */
//--------------------------------------------------------------------------
CicoSCWindow*
CicoSCWindowController::findWindow(int surfaceid)
{
    map<unsigned int, CicoSCWindow*>::iterator itr;
    itr = m_windowList.find(surfaceid);
    if (m_windowList.end() == itr) {
        ICO_TRA("not found window object. surfaceid=%08x", surfaceid);
        return NULL;
    }

    return itr->second;
}

//--------------------------------------------------------------------------
/**
 *  @brief  find window object by surfaceid
 *
 *  @param [in] surfaceid   wayland surface id
 */
//--------------------------------------------------------------------------
CicoSCLayer*
CicoSCWindowController::findLayer(int displayid, int layerid)
{
    vector<CicoSCDisplay*>::iterator itr;
    itr = m_displayList.begin();
    for (; itr != m_displayList.end(); ++itr) {
        if ((*itr)->displayid != displayid) {
            continue;
        }
        vector<CicoSCLayer*>::iterator itr2;
        itr2 = (*itr)->layerList.begin();
        for (; itr2 != (*itr)->layerList.end(); ++itr2) {
            if ((*itr2)->layerid == layerid) {
                return *itr2;
            }
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  find display zone by id
 */
//--------------------------------------------------------------------------
const CicoSCDisplayZone *
CicoSCWindowController::findDisplayZone(int zoneid)
{
    vector<CicoSCDisplay*>::iterator itr;
    itr = m_displayList.begin();
    for (; itr != m_displayList.end(); ++itr) {
        std::map<unsigned int, CicoSCDisplayZone*>::iterator itr2;
        itr2 = (*itr)->zoneList.find(zoneid);
        if ((*itr)->zoneList.end() != itr2) {
            return itr2->second;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  handle command
 *
 *  @param [in] cmd     control command
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::handleCommand(const CicoSCCommand * cmd)
{
//    ICO_TRA("CicoSCWindowController::handleCommand Enter(%d)", cmd->cmdid);

    CicoSCCmdWinCtrlOpt *opt = static_cast<CicoSCCmdWinCtrlOpt*>(cmd->opt);

    switch (cmd->cmdid) {
    case MSG_CMD_SHOW:
        ICO_DBG("command: MSG_CMD_SHOW");
        if (opt->animationTime & ICO_SYC_WIN_SURF_NORESCTL) {
            /* show command but not resource control (for HomeScreen)   */
            (void)show(opt->surfaceid,
                       opt->animation.c_str(),
                       opt->animationTime);
        }
        else {
            /* show command (normal)    */
            (void)notifyResourceManager(opt->surfaceid,
                                        NULL,
                                        opt->layerid,
                                        opt->animation.c_str(),
                                        opt->animationTime);
        }
        break;
    case MSG_CMD_HIDE:
        ICO_DBG("command: MSG_CMD_HIDE");
        (void)hide(opt->surfaceid, opt->animation.c_str(), opt->animationTime);
        break;
    case MSG_CMD_MOVE:
    {
        ICO_DBG("command: MSG_CMD_MOVE");
        CicoSCWindow *window = findWindow(opt->surfaceid);
        if (NULL == window) {
            break;
        }
        if (true == opt->zone.empty()) {
            (void)setGeometry(opt->surfaceid, opt->nodeid, opt->layerid,
                              opt->x, opt->y, opt->width, opt->height,
                              opt->animation.c_str(), opt->animationTime,
                              opt->animation.c_str(), opt->animationTime);
        }
        else if (opt->zone == window->zone) {
            (void)setGeometry(opt->surfaceid, opt->zone.c_str(), opt->layerid,
                              opt->animation.c_str(), opt->animationTime,
                              opt->animation.c_str(), opt->animationTime);
        }
        else {
            (void)notifyResourceManager(opt->surfaceid,
                                        opt->zone.c_str(),
                                        opt->layerid,
                                        opt->animation.c_str(),
                                        opt->animationTime);
        }
        break;
    }
    case MSG_CMD_CHANGE_ACTIVE:
        ICO_DBG("command: MSG_CMD_CHANGE_ACTIVE");
        (void)active(opt->surfaceid, opt->active);
        break;
    case MSG_CMD_CHANGE_LAYER:
        ICO_DBG("command: MSG_CMD_CHANGE_LAYER");
        (void)setWindowLayer(opt->surfaceid, opt->layerid);
        break;
    case MSG_CMD_MAP_GET:
        ICO_DBG("command: MSG_CMD_MAP_GET");
        (void)setmapGet(opt->surfaceid, opt->animation.c_str());
        break;
    case MSG_CMD_MAP_THUMB:
        ICO_DBG("command: MSG_CMD_MAP_THUMB");
        (void)mapSurface(opt->surfaceid, opt->framerate, opt->animation.c_str());
        break;
    case MSG_CMD_UNMAP_THUMB:
        ICO_DBG("command: MSG_CMD_UNMAP_THUMB");
        (void)unmapSurface(opt->surfaceid);
        break;
    case MSG_CMD_SHOW_LAYER:
        ICO_DBG("command: MSG_CMD_SHOW_LAYER");
        (void)showLayer(opt->displayid, opt->layerid);
        break;
    case MSG_CMD_HIDE_LAYER:
        ICO_DBG("command: MSG_CMD_HIDE_LAYER");
        (void)hideLayer(opt->displayid, opt->layerid);
        break;
    default:
        ICO_WRN("command: Unknown(0x%08x)", cmd->cmdid);
        break;
    }

//    ICO_TRA("CicoSCWindowController::handleCommand Leave");
}

int
CicoSCWindowController::notifyResourceManager(int        surfaceid,
                                              const char *zone,
                                              int        layerid,
                                              const char *animation,
                                              int        animationTime)
{
    ICO_TRA("CicoSCWindowController::notifyResourceManager Enter"
            "(surfaceid=%08x zone=%s layerid=%d "
            "animation=%s animationTime=%d)",
            surfaceid, zone, layerid, animation, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::notifyResourceManager Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    CicoSCCommand cmd;
    CicoSCCmdResCtrlOpt *opt = new CicoSCCmdResCtrlOpt();

    cmd.cmdid = MSG_CMD_ACQUIRE_RES;
    cmd.appid = window->appid;
    cmd.pid   = window->pid;
    cmd.opt   = opt;

    opt->dispres       = true;
    opt->winname       = window->name;
    opt->layerid       = layerid;
    opt->surfaceid     = window->surfaceid;
    opt->animation     = animation ? animation : "";
    opt->animationTime = animationTime;
    string fullname;
    if (NULL == zone) {
        const CicoSCDisplayZone* zone = findDisplayZone(window->zoneid);
        if (NULL != zone) {
            opt->dispzone = zone->fullname;
        }
        else {
            ICO_WRN("not found zone information");
            ICO_TRA("CicoSCWindowController::notifyResourceManager Leave(ENOENT)");
            return ICO_SYC_ENOENT;
        }
    }
    else {
        opt->dispzone = zone;
    }
#if 1   //TODO
    opt->soundres  = true;
    opt->soundname = window->appid;
    opt->soundid   = 0;
    CicoSystemConfig *sysconf = CicoSystemConfig::getInstance();
    const CicoSCDefaultConf *defconf = sysconf->getDefaultConf();
    if (NULL != defconf) {
        const CicoSCSoundZoneConf *zoneconf =
            sysconf->findSoundZoneConfbyId(defconf->soundzone);
        if (NULL != zoneconf) {
            opt->soundzone = zoneconf->fullname;
        }
    }
#endif

    m_resMgr->handleCommand((const CicoSCCommand&)cmd, true);

    ICO_TRA("CicoSCWindowController::notifyResourceManager Leave(EOK)");
    return ICO_SYC_EOK;
}

const CicoSCWindow*
CicoSCWindowController::findWindowObj(int32_t pid, uint32_t surfaceid) const
{
    const CicoSCWindow* r = NULL;
    map<unsigned int, CicoSCWindow*>::const_iterator itr = m_windowList.begin();
    for (; itr != m_windowList.end(); ++itr) {
        const CicoSCWindow* tmp = itr->second;
        if ((pid == tmp->pid) && (surfaceid == (uint32_t)tmp->surfaceid)) {
            r = tmp;
            break;  // break of for itr
        }
    }
    ICO_TRA("return %x", r);
    return r;
}

const CicoSCResourceManager*
CicoSCWindowController::getResourceManager(void) const
{
    return m_resMgr;
}

//--------------------------------------------------------------------------
/**
 * @brief Window Check Timer
 * @param data      this object
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoSCWindowController::ico_SCWindowChecker(void *data)
{
    struct ico_sc_win_check_t   *tp, *bp;
    struct timeval      curtime;
    int                 ntime;
    int                 delflag;
    int                 commflag;
    int                 par;
    int                 htimer = 0;
    double              rate;
    int                 x, y;

    gettimeofday(&curtime, NULL);

    if ((curtime.tv_sec < g_lasttime.tv_sec) ||
        ((curtime.tv_sec - g_lasttime.tv_sec) > 2)) {
        ntime = ICO_SC_WINCHK_TIMER_BASE;
        htimer ++;
    }
    else    {
        ntime = (curtime.tv_sec - g_lasttime.tv_sec) * 1000;
        if (curtime.tv_usec >= g_lasttime.tv_usec)  {
            ntime += (curtime.tv_usec - g_lasttime.tv_usec) / 1000;
        }
        else    {
            ntime += ((1000000 + curtime.tv_usec - g_lasttime.tv_usec) / 1000) - 1000;
        }
    }
    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    delflag = 0;
    commflag = 0;
    while (tp)  {
        tp->lapsed += ntime;
        tp->timeout += ntime;
        if (tp->function != ICO_SC_WINCHK_CONTENTS_CHECK)   {
            htimer ++;
        }

        if (tp->timeout >= tp->cycle)   {
            /* timedout     */
            do  {
                tp->timeout -= tp->cycle;
            } while (tp->timeout > 0);

            switch (tp->function)   {
            case ICO_SC_WINCHK_CONTENTS_CHECK:      /* check contents change    */
                if (tp->state == ICO_SC_WINCONT_CHECK)  {
                    // GENIVI-LM 1.6.0 not support contents change event
                    // get surface image form GENIVI-LM
                    ICO_DBG("CicoSCWindowController::ico_SCWindowChecker "
                            "call ilm_takeSurfaceScreenshot %d[%x]",
                            tp->surfaceid, tp->surfaceid);
                    getInstance()->setmapGet(tp->surfaceid, tp->u.cont.path);
                    tp->state = ICO_SC_WINCONT_WAITFILE;
                    tp->time = ICO_SC_WINCHK_TIMER_BASE;
                    tp->cycle = ICO_SC_WINCHK_TIMER_BASE;
                    tp->timeout = 0;
                }
                else    {
                    struct stat stat_buf;

                    if (stat(tp->u.cont.path, &stat_buf) >= 0)  {
                        tp->state = ICO_SC_WINCONT_CHECK;
                        tp->time = ICO_SC_WINCHK_CONTENTS_TIME;
                        tp->cycle = ICO_SC_WINCHK_CONTENTS_TIME;
                        tp->timeout = 0;

                        getInstance()->mapSurfaceCB(ICO_SC_WINCHK_MAP_SURFACE_EVENT_MAP,
                                                    tp->surfaceid, 0,
                                                    tp->u.cont.width, tp->u.cont.height,
                                                    tp->u.cont.width * 4, 0);
                    }
                }
                break;

            case ICO_SC_WINCHK_ANIMA_FADE:          /* fade animation           */
                par = ((tp->lapsed * 100 / tp->time) / 3) * 3;
                if (par == tp->par)     break;

                if (par < 0)    par = 0;
                tp->par = par;
                if (par >= 97)  {
                    if (ilm_surfaceSetOpacity(tp->surfaceid, 1.0) != ILM_SUCCESS)   {
                        ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                                "ilm_surfaceSetOpacity Error %d[%x]",
                                tp->surfaceid, tp->surfaceid);
                    }
                    if (tp->state != ICO_SC_WINANIMA_SHOW)  {
                        if (ilm_surfaceSetVisibility(tp->surfaceid, 0) != ILM_SUCCESS)  {
                            ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                                    "ilm_surfaceSetVisibility Error %d[%x]",
                                    tp->surfaceid, tp->surfaceid);
                        }
                    }
                    delflag = 1;
                }
                else    {
                    if (tp->state == ICO_SC_WINANIMA_SHOW)  {
                        rate = ((double)par) / 100.0;
                    }
                    else    {
                        rate = ((double)(100 - par)) / 100.0;
                    }
                    if (ilm_surfaceSetOpacity(tp->surfaceid, rate) != ILM_SUCCESS) {
                        ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                                "ilm_surfaceSetOpacity Error %d[%x]",
                                tp->surfaceid, tp->surfaceid);
                    }
                }
                commflag ++;
                break;

            case ICO_SC_WINCHK_ANIMA_SLIDETOTOP:        /* slide totop animation    */
            case ICO_SC_WINCHK_ANIMA_SLIDETOBUTTOM:     /* slide totop animation    */
            case ICO_SC_WINCHK_ANIMA_SLIDETOLEFT:       /* slide totop animation    */
            case ICO_SC_WINCHK_ANIMA_SLIDETORIGHT:      /* slide totop animation    */
                par = ((tp->lapsed * 100 / tp->time) / 3) * 3;
                if (par == tp->par)     break;

                if (par < 0)    par = 0;
                tp->par = par;
                if (par >= 97)  {
                    if (tp->state != ICO_SC_WINANIMA_SHOW)  {
                        if (ilm_surfaceSetVisibility(tp->surfaceid, 0) != ILM_SUCCESS)  {
                            ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                                    "ilm_surfaceSetVisibility Error %d[%x]",
                                    tp->surfaceid, tp->surfaceid);
                        }
                    }
                    x = tp->u.anim.x;
                    y = tp->u.anim.y;
                    delflag = 1;
                }
                else    {
                    if (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOTOP)    {
                        x = tp->u.anim.x;
                        if (tp->state == ICO_SC_WINANIMA_SHOW)  {
                            y = tp->u.anim.screen_height -
                                (tp->u.anim.screen_height - tp->u.anim.y) * par / 100;
                        }
                        else    {
                            y = tp->u.anim.y +
                                (tp->u.anim.screen_height - tp->u.anim.y) * par / 100;
                        }
                    }
                    else if (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOBUTTOM) {
                        x = tp->u.anim.x;
                        if (tp->state == ICO_SC_WINANIMA_SHOW)  {
                            y = (-tp->u.anim.height) +
                                (tp->u.anim.y + tp->u.anim.height) * par / 100;
                        }
                        else    {
                            y = tp->u.anim.y -
                                (tp->u.anim.height + tp->u.anim.y) * par / 100;
                        }
                    }
                    else if (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOLEFT)   {
                        y = tp->u.anim.y;
                        if (tp->state == ICO_SC_WINANIMA_SHOW)  {
                            x = tp->u.anim.screen_width -
                                (tp->u.anim.screen_width - tp->u.anim.x) * par / 100;
                        }
                        else    {
                            x = tp->u.anim.x +
                                (tp->u.anim.screen_width - tp->u.anim.x) * par / 100;
                        }
                    }
                    else    {
                        y = tp->u.anim.y;
                        if (tp->state == ICO_SC_WINANIMA_SHOW)  {
                            x = (-tp->u.anim.width) +
                                (tp->u.anim.x + tp->u.anim.width) * par / 100;
                        }
                        else    {
                            x = tp->u.anim.x -
                                (tp->u.anim.x + tp->u.anim.width) * par / 100;
                        }
                    }
                }
                if (ilm_surfaceSetDestinationRectangle(tp->surfaceid, x, y,
                                                       tp->u.anim.width, tp->u.anim.height)
                        != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                            "ilm_surfaceSetDestinationRectangle Error %d[%x]",
                            tp->surfaceid, tp->surfaceid);
                }
                commflag ++;
                break;
            }
        }
        if (delflag) {
            delflag = 0;
            if (bp == NULL) {
                tp = tp->next;
                CicoSCWindowController::win_check_timer = tp;
            }
            else    {
                bp->next = tp->next;
                tp->next = CicoSCWindowController::win_check_free;
                CicoSCWindowController::win_check_free = tp;
                tp = bp->next;
            }
        }
        else    {
            bp = tp;
            tp = tp->next;
        }
    }
    if (commflag)   {
        if (ilm_commitChanges() != ILM_SUCCESS) {
            ICO_ERR("CicoSCWindowController::ico_SCWindowChecker "
                    "ilm_commitChanges Error");
        }
    }
    g_lasttime = curtime;
    if (((htimer == 0) && (g_hightimer != 0)) ||
        ((htimer != 0) && (g_hightimer == 0)))  {
        // change timer speed
        g_hightimer = 1 - g_hightimer;

        ecore_timer_interval_set(g_SCWindowChecker,
             ((double)(g_hightimer ?
                       ICO_SC_WINCHK_TIMER_BASE : ICO_SC_WINCHK_TIMER_BASE_L)) / 1000.0);
    }
    return ECORE_CALLBACK_RENEW;
}

//--------------------------------------------------------------------------
/**
 * @brief window checker destroy surface
 * @param surfaceid surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::windowCheckerRemove(uint32_t surfaceid)
{
    struct ico_sc_win_check_t   *tp, *bp, *fp;

    ICO_TRA("CicoSCWindowController::windowCheckerRemove: Enter %d[%x]",
            surfaceid, surfaceid);

    resetWindowMap(surfaceid);

    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    while (tp)  {
        if (tp->surfaceid == surfaceid) {
            // delete check target
            if (tp->function == ICO_SC_WINCHK_CONTENTS_CHECK)   {
                ICO_DBG("CicoSCWindowController::windowCheckerRemove: Remove Thumbnail %d",
                        tp->surfaceid);
            }

            fp = tp;
            if (bp == NULL) {
                tp = tp->next;
                CicoSCWindowController::win_check_timer = tp;
            }
            else    {
                bp->next = tp->next;
                tp = tp->next;
            }
            fp->next = CicoSCWindowController::win_check_free;
            CicoSCWindowController::win_check_free = fp;
        }
        else    {
            bp = tp;
            tp = tp->next;
        }
    }
    ICO_TRA("CicoSCWindowController::windowCheckerRemove: Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief window checker set animation
 * @param window    window object
 * @param show      show/hide
 * @param animation animation name
 * @param time      animation time(ms)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::setWindowAnimation(CicoSCWindow *window,
                                           int show, const char *animation, int time)
{
    struct ico_sc_win_check_t   *tp, *bp;
    uint32_t    surfaceid = window->surfaceid;

    ICO_TRA("CicoSCWindowController::setWindowAnimation: Enter %d[%x] %d <%s> %d",
            surfaceid, surfaceid, show, animation, time);

    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    while (tp)  {
        if ((tp->surfaceid == surfaceid) &&
            ((tp->function == ICO_SC_WINCHK_ANIMA_FADE) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOTOP) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOBUTTOM) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOLEFT) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETORIGHT)))   {
            // same surface target
            break;
        }
        bp = tp;
        tp = tp->next;
    }
    if (! tp)   {
        tp = CicoSCWindowController::win_check_free;
        if (tp) {
            CicoSCWindowController::win_check_free = tp->next;
        }
        else    {
            tp = (struct ico_sc_win_check_t *)malloc(sizeof(struct ico_sc_win_check_t));
            if (! tp)   {
                ICO_ERR("CicoSCWindowController::setWindowAnimation: Out of Memory");
                return;
            }
        }
        memset(tp, 0, sizeof(struct ico_sc_win_check_t));
        if (bp) {
            bp->next = tp;
        }
        else    {
            CicoSCWindowController::win_check_timer = tp;
        }
    }

    tp->surfaceid = surfaceid;
    tp->time = time;
    tp->lapsed = 0;
    tp->par = -1;
    tp->timeout = ICO_SC_WINCHK_ANIMATION_TIME;
    tp->cycle = ICO_SC_WINCHK_ANIMATION_TIME;
    if ((strcasecmp(animation, ICO_SYC_WIN_ANIMATION_SLIDE) == 0) ||
        (strcasecmp(animation, ICO_SYC_WIN_ANIMATION_SLIDE_TOTOP) == 0))    {
        tp->function = ICO_SC_WINCHK_ANIMA_SLIDETOTOP;
    }
    else if (strcasecmp(animation, ICO_SYC_WIN_ANIMATION_SLIDE_TOBUTTOM) == 0)  {
        tp->function = ICO_SC_WINCHK_ANIMA_SLIDETOBUTTOM;
    }
    else if (strcasecmp(animation, ICO_SYC_WIN_ANIMATION_SLIDE_TOLEFT) == 0)    {
        tp->function = ICO_SC_WINCHK_ANIMA_SLIDETOLEFT;
    }
    else if (strcasecmp(animation, ICO_SYC_WIN_ANIMATION_SLIDE_TORIGHT) == 0)   {
        tp->function = ICO_SC_WINCHK_ANIMA_SLIDETORIGHT;
    }
    else    {
        tp->function = ICO_SC_WINCHK_ANIMA_FADE;
    }
    tp->state = show;
    tp->u.anim.x = window->x;
    tp->u.anim.y = window->y;
    tp->u.anim.width = window->width;
    tp->u.anim.height = window->height;
    tp->u.anim.screen_width = 1080;
    tp->u.anim.screen_height = 1920;

    vector<CicoSCDisplay*>::iterator itr;
    itr = m_displayList.begin();
    for (; itr != m_displayList.end(); ++itr) {
        if ((*itr)->displayid == window->displayid) {
            tp->u.anim.screen_width = (*itr)->pWidth;
            tp->u.anim.screen_height = (*itr)->pHeight;
            break;
        }
    }
    (void) CicoSCWindowController::ico_SCWindowChecker(NULL);

    ICO_TRA("CicoSCWindowController::setWindowAnimation: Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief window checker reset animation
 * @param window    window object
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::resetWindowAnimation(CicoSCWindow *window)
{
    struct ico_sc_win_check_t   *tp, *bp;
    uint32_t        surfaceid = window->surfaceid;

    ICO_TRA("CicoSCWindowController::resetWindowAnimation: Enter %d[%x]",
            surfaceid, surfaceid);

    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    while (tp)  {
        if ((tp->surfaceid == surfaceid) &&
            ((tp->function == ICO_SC_WINCHK_ANIMA_FADE) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOTOP) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOBUTTOM) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETOLEFT) ||
             (tp->function == ICO_SC_WINCHK_ANIMA_SLIDETORIGHT)))   {
            // delete surface target
            if (ilm_surfaceSetOpacity(tp->surfaceid, 1.0) != ILM_SUCCESS)   {
                ICO_ERR("CicoSCWindowController::resetWindowAnimation "
                        "ilm_surfaceSetOpacity Error %d[%x]",
                        tp->surfaceid, tp->surfaceid);
            }
            if (ilm_surfaceSetDestinationRectangle(tp->surfaceid,
                                                   tp->u.anim.x, tp->u.anim.y,
                                                   tp->u.anim.width, tp->u.anim.height)
                    != ILM_SUCCESS) {
                ICO_ERR("CicoSCWindowController::resetWindowAnimation "
                        "ilm_surfaceSetDestinationRectangle Error %d[%x]",
                        tp->surfaceid, tp->surfaceid);
            }
            if (ilm_surfaceSetVisibility(tp->surfaceid,
                        (tp->state == ICO_SC_WINANIMA_SHOW) ? 1 : 0) != ILM_SUCCESS)  {
                ICO_ERR("CicoSCWindowController::resetWindowAnimation "
                        "ilm_surfaceSetVisibility Error %d[%x]",
                        tp->surfaceid, tp->surfaceid);
            }
            if (ilm_commitChanges() != ILM_SUCCESS) {
                ICO_ERR("CicoSCWindowController::resetWindowAnimation "
                        "ilm_commitChanges Error");
            }
            if (bp == NULL) {
                CicoSCWindowController::win_check_timer = tp->next;
            }
            else    {
                bp->next = tp->next;
            }
            tp->next = CicoSCWindowController::win_check_free;
            CicoSCWindowController::win_check_free = tp;
            break;
        }
        bp = tp;
        tp = tp->next;
    }
    ICO_TRA("CicoSCWindowController::resetWindowAnimation: Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief window checker set map
 * @param window    window object
 * @param framerate frame rate(frames/sec or -1)
 * @param filepath  surface image file path
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::setWindowMap(CicoSCWindow *window,
                                     int framerate, const char *filepath)
{
    struct ico_sc_win_check_t   *tp, *bp;
    uint32_t    surfaceid = window->surfaceid;

    ICO_TRA("CicoSCWindowController::setWindowMap: Enter %d[%x],%d,<%s>",
            surfaceid, surfaceid, framerate, filepath);

    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    while (tp)  {
        if ((tp->surfaceid == surfaceid) &&
            (tp->function == ICO_SC_WINCHK_CONTENTS_CHECK)) {
            // same surface target
            break;
        }
        bp = tp;
        tp = tp->next;
    }
    if (! tp)   {
        tp = CicoSCWindowController::win_check_free;
        if (tp) {
            CicoSCWindowController::win_check_free = tp->next;
        }
        else    {
            tp = (struct ico_sc_win_check_t *)malloc(sizeof(struct ico_sc_win_check_t));
            if (! tp)   {
                ICO_ERR("CicoSCWindowController::setWindowMap: Out of Memory");
                return;
            }
        }
        memset(tp, 0, sizeof(struct ico_sc_win_check_t));
        if (bp) {
            bp->next = tp;
        }
        else    {
            CicoSCWindowController::win_check_timer = tp;
        }
    }
    tp->function = ICO_SC_WINCHK_CONTENTS_CHECK;
    tp->surfaceid = surfaceid;
    if (framerate <= 0) {
        tp->cycle = 0x7fff;
    }
    else    {
        tp->cycle = 1000/framerate;
    }
    tp->time = 0x7fff;
    tp->lapsed = 0;
    tp->par = -1;
    tp->timeout = 0;
    strncpy(tp->u.cont.path, filepath, ICO_SYC_MAX_LEN-1);
    tp->state = ICO_SC_WINCONT_CHECK;
    tp->u.cont.x = window->x;
    tp->u.cont.y = window->y;
    tp->u.cont.width = window->width;
    tp->u.cont.height = window->height;

    (void) CicoSCWindowController::ico_SCWindowChecker(NULL);

    ICO_TRA("CicoSCWindowController::setWindowMap: Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief window checker reset map
 * @param surfaceid surface id
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::resetWindowMap(uint32_t surfaceid)
{
    struct ico_sc_win_check_t   *tp, *bp;

    ICO_TRA("CicoSCWindowController::resetWindowMap: Enter %d[%x]",
            surfaceid, surfaceid);

    tp = CicoSCWindowController::win_check_timer;
    bp = NULL;
    while (tp)  {
        if ((tp->surfaceid == surfaceid) &&
            (tp->function == ICO_SC_WINCHK_CONTENTS_CHECK)) {
            if (bp == NULL) {
                CicoSCWindowController::win_check_timer = tp->next;
            }
            else    {
                bp->next = tp->next;
            }
            tp->next = CicoSCWindowController::win_check_free;
            CicoSCWindowController::win_check_free = tp;
            break;
        }
        bp = tp;
        tp = tp->next;
    }
    if (tp) {
        mapSurfaceCB(ICO_SC_WINCHK_MAP_SURFACE_EVENT_UNMAP,
                     tp->surfaceid, 0, tp->u.cont.width, tp->u.cont.height,
                     tp->u.cont.width * 4, 0);
    }
    ICO_TRA("CicoSCWindowController::resetWindowMap: Leave");
}
// vim:set expandtab ts=4 sw=4:
