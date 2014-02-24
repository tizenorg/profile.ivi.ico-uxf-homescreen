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
#include <sys/ioctl.h>

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
#include "CicoSCLifeCycleController.h"
#include "CicoSCResourceManager.h"

//==========================================================================
//  private static variable
//==========================================================================
CicoSCWindowController* CicoSCWindowController::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  get instance of CicoSCWindowController
 *
 *  @return instance of CicoSCWayland
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
    wayland->getInstance()->addWaylandIF(ICO_WL_WIN_MGR_IF, this);
    wayland->getInstance()->addWaylandIF(ICO_WL_IVI_APPLICATION_IF, this);
    wayland->getInstance()->addWaylandIF(ICO_WL_IVI_CONTROLLER_IF, this);
    wayland->getInstance()->addWaylandIF(ICO_WL_OUTPUT_IF, this);
    initDB();

    CicoSCWindowController::ms_myInstance = this;
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
    ICO_TRA("CicoSCWindowController::show Enter"
            "(surfaceid=0x%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

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
                    "(0x%08X:\"%s\")",
                    zone->zoneid, window->surfaceid, window->appid.c_str());
            zone->displayedWindow = window;
        }
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    int raiseFlag = ICO_SYC_WIN_RAISE_NOCHANGE;
    if (animationTime & ICO_SYC_WIN_SURF_RAISE) {
        raiseFlag = ICO_SYC_WIN_RAISE_RAISE;
        window->raise = true;
    }
    else if (animationTime & ICO_SYC_WIN_SURF_LOWER)    {
        raiseFlag = ICO_SYC_WIN_RAISE_LOWER;
        window->raise = false;
    }
    else if (((animationTime & ICO_SYC_WIN_SURF_NOCHANGE) == 0) &&
             (false == window->raise))   {
        raiseFlag = ICO_SYC_WIN_RAISE_RAISE;
        window->raise = true;
    }
    if ((NULL != animation) && (animation[0] != '\0')) {
        // set animation request to Multi Window Manager
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                    ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                    animation, animationTime & ~ICO_SYC_WIN_SURF_FLAGS);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                 ICO_SYC_WIN_VISIBLE_SHOW,
                                 raiseFlag, animaFlag);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::show Leave(EOK)");
    return ICO_SYC_EOK;
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
    ICO_TRA("CicoSCWindowController::hide Enter"
            "(surfaceid=0x%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

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
                    "(0x%08X:\"%s\")",
                    zone->zoneid, window->surfaceid, window->appid.c_str());
            zone->displayedWindow = NULL;
        }
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                       animation, animationTime & ~ICO_SYC_WIN_SURF_FLAGS);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                    ICO_SYC_LAYER_VISIBLE_HIDE,
                                    ICO_SYC_WIN_RAISE_NOCHANGE, animaFlag);

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
    ICO_TRA("CicoSCWindowController::resize Enter"
            "(surfaceid=0x%08X h=%d w=%d animation=%s animationTime=%d)",
            surfaceid, w, h, animation, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::resize Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_RESIZE,
                                       animation, animationTime);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    // set visible request to Multi Window Manager
    window->width = w;
    window->height = h;
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid, window->nodeid,
                                      window->x, window->y,
                                      w, h, animaFlag);

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
    ICO_TRA("CicoSCWindowController::move Enter"
            "(surfaceid=0x%08X nodeid=%d x=%d y=%d "
            "animation=%s animationTime=%d)",
            surfaceid, nodeid, x, y, animation, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::move Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_MOVE,
                                       animation, animationTime);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    int moveNodeId = ICO_SYC_WIN_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }

    // set visible request to Multi Window Manager
    window->x = x;
    window->y = y;
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid,
                                      moveNodeId, x, y,
                                      window->width, window->height,
                                      animaFlag);

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
    ICO_TRA("CicoSCWindowController::raise Enter"
            "(surfaceid=0x%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::raise Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->raise = true;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWindowController::setAnimation(window->surfaceid,
                                             ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                             animation, animationTime);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                 ICO_SYC_WIN_VISIBLE_NOCHANGE,
                                 ICO_SYC_WIN_RAISE_RAISE, animaFlag);

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
            "(surfaceid=0x%08X nodeid=%d layerid=%d x=%d y=%d w=%d h=%d "
            "resizeAnimation=%s resizeAnimationTime=%d "
            "moveAnimation=%s moveAnimationTime=%d)",
            surfaceid, nodeid, layerid, x, y, w, h,
            resizeAnimation, resizeAnimationTime,
            moveAnimation, moveAnimationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    const char *animation = "";
    if (NULL != resizeAnimation) {
        animation = resizeAnimation;
    }
    CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                   ICO_WINDOW_MGR_ANIMATION_TYPE_RESIZE,
                                   animation, resizeAnimationTime);

    if (NULL != moveAnimation) {
        animation = moveAnimation;
    }
    CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                   ICO_WINDOW_MGR_ANIMATION_TYPE_MOVE,
                                   animation, moveAnimationTime);

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

    // set window layer to Multi Window Manager
    if (0 <= layerid) {
        setWindowLayer(window->surfaceid, layerid);
    }

    // update window attr
    window->nodeid = moveNodeId;

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid, moveNodeId,
                                      moveX, moveY, moveW, moveH,
                                      ICO_SYC_ANIMATION_ON);

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
            "(surfaceid=0x%08X zone=%s layerid=%d "
            "resizeAnimation=%s resizeAnimationTime=%d "
            "moveAnimation=%s moveAnimationTime=%d)",
            surfaceid, zone, layerid,
            resizeAnimation, resizeAnimationTime,
            moveAnimation, moveAnimationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

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
                        "(0x%08X:\"%s\")",
                        olddispzone->zoneid, window->surfaceid,
                        window->appid.c_str());
                olddispzone->displayedWindow = NULL;
            }
        }

        CicoSCLayer *layer = findLayer(displayno, window->layerid);
        if ((NULL != layer) && (layer->type == ICO_LAYER_TYPE_APPLICATION)) {
            ICO_DBG("Entry display zone[%d] current displayed window"
                    "(0x%08X:\"%s\")",
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
    ICO_TRA("CicoSCWindowController::lower Enter"
            "(surfaceid=0x%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::lower Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->raise = false;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_SYC_ANIMATION_OFF;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                       animation, animationTime);
        animaFlag = ICO_SYC_ANIMATION_ON;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid, ICO_SYC_WIN_VISIBLE_NOCHANGE,
                                 ICO_SYC_WIN_RAISE_LOWER, animaFlag);

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
    uint32_t    oldlayerid;

    ICO_TRA("CicoSCWindowController::setWindowLayer Enter"
            "(surfaceid=0x%08X layerid=%x)", surfaceid, layerid);

    // find window information in window list
    CicoSCWindow* window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // find layer information in layer list
    CicoSCLayer* layer = findLayer(window->displayid, layerid);
    if (NULL == layer) {
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT[disp=%d,layer=%d])",
                window->displayid, layerid);
        return ICO_SYC_ENOENT;
    }

    // update window attr
    oldlayerid = window->layerid;
    window->layerid = layerid;

    // set window layer request to Multi Window Manager
    CicoSCWlWinMgrIF::setWindowLayer(window->surfaceid,
                                     (uint32_t)window->layerid, oldlayerid);
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
    ICO_TRA("CicoSCWindowController::hideLayer Enter"
            "displayid=%d layerid=%d)",
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

    ICO_TRA("CicoSCWindowController::hideVisible Leave(EOK)");
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
            "(surfaceid=0x%08X, target=%08X)", surfaceid, target);

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

    // set active request to Multi Window Manager
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
    ICO_TRA("CicoSCWindowController::mapSurface Enter(0x%08x,%d,%s)",
            surfaceid, framerate, filepath ? filepath : "(null)");

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::mapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    if (framerate < 0) {
        framerate = 0;
    }

    CicoSCWlWinMgrIF::mapSurface(surfaceid, framerate, filepath);
    CicoSCWayland::getInstance()->flushDisplay();

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
    ICO_TRA("CicoSCWindowController::unmapSurface Enter(0x%08x)", surfaceid);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::unmapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    CicoSCWlWinMgrIF::unmapSurface(surfaceid);
    CicoSCWayland::getInstance()->flushDisplay();

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
    ICO_TRA("CicoSCWindowController::setAttributes Enter"
            "(surfaceid=0x%08X)", surfaceid);

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
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] select          select device(unused)
 *                              (0=not active/1=pointer/2=touch)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::activeCB(void                  *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 uint32_t              surfaceid,
                                 int32_t               select)
{
    ICO_TRA("CicoSCWindowController::activeCB Enter"
            "(surfaceid=0x%08X select=%d)", surfaceid, select);

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
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
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
CicoSCWindowController::mapSurfaceCB(void                  *data,
                                     struct ico_window_mgr *ico_window_mgr,
                                     int32_t               event,
                                     uint32_t              surfaceid,
                                     uint32_t              type,
                                     int32_t               width,
                                     int32_t               height,
                                     int32_t               stride,
                                     uint32_t               format)
{
    int command;

    ICO_TRA("CicoSCWindowController::mapSurfaceCB Enter(ev=%d surf=%x "
            "type=%d w/h=%d/%d stride=%d form=%x)",
            event, surfaceid, type, width, height, stride, format);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        return;
    }

    // convert event to command
    switch (event)  {
    case ICO_WINDOW_MGR_MAP_SURFACE_EVENT_CONTENTS:
    case ICO_WINDOW_MGR_MAP_SURFACE_EVENT_RESIZE:
    case ICO_WINDOW_MGR_MAP_SURFACE_EVENT_MAP:
        command = MSG_CMD_MAP_THUMB;
        break;
    case ICO_WINDOW_MGR_MAP_SURFACE_EVENT_UNMAP:
    case ICO_WINDOW_MGR_MAP_SURFACE_EVENT_ERROR:
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

    ++m_physicalDisplayTotal;
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
    int     pid;
    int     ret;
    struct ilmSurfaceProperties SurfaceProperties;

    ICO_TRA("CicoSCWindowController::createSurfaceCB Enter"
            "(surfaceid=0x%08x)", id_surface);

    if ((ilm_getPropertiesOfSurface(id_surface, &SurfaceProperties) != ILM_SUCCESS) ||
        (ilm_commitChanges() != ILM_SUCCESS))   {
        ICO_ERR("CicoSCWindowController::createSurfaceCB: ilm_getPropertiesOfSurface(%x) Error",
                id_surface);
        return;
    }
    ICO_TRA("createSurfaceCB: surface=%08x pid=%d w/h=%d/%d->%d/%d",
            id_surface, SurfaceProperties.creatorPid,
            SurfaceProperties.sourceWidth, SurfaceProperties.sourceHeight,
            SurfaceProperties.destWidth, SurfaceProperties.destHeight);

    CicoSCWindow* window = new CicoSCWindow();
    window->surfaceid = id_surface;
    window->name = CicoSCWlWinMgrIF::wlIviCtrlGetSurfaceWaiting(id_surface, &pid);
    window->pid       = pid;
    window->displayid = 0;              // currently fixed 0
    window->raise = 1;                  // created surface is top of layer

    CicoSCLifeCycleController* appctrl;
    appctrl = CicoSCLifeCycleController::getInstance();
    const CicoAilItems* ailItem = NULL;
    const CicoAulItems* aulitem = appctrl->findAUL(window->pid);
    if (NULL != aulitem) {
        window->appid = aulitem->m_appid;
        ICO_DBG("appid=%s", window->appid.c_str());
        ailItem = appctrl->findAIL(window->appid.c_str());
    }
    else {
        ICO_DBG("application information not found.");
    }

    if (NULL != ailItem) {
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

    CicoSCLayer *blayer = findLayer(window->displayid, window->layerid);
    if (blayer) {
        blayer->addSurface(window->surfaceid, true);
        int nsurf;
        const int *surfs = blayer->getSurfaces(&nsurf);
        if (ilm_layerSetRenderOrder(window->layerid, (t_ilm_layer *)surfs, nsurf)
            != ILM_SUCCESS)   {
            ICO_ERR("CicoSCWindowController::createSurfaceCB: "
                    "ilm_layerSetRenderOrder(%d,,%d) Error", window->layerid, nsurf);
        }
        else if (ilm_commitChanges() != ILM_SUCCESS)    {
            ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_commitChanges() Error");
        }
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

    // add notification
    if ((ret = ilm_surfaceAddNotification(window->surfaceid, wlGeniviSurfaceNotification))
        != ILM_SUCCESS) {
        ICO_ERR("CicoSCWindowController::createSurfaceCB ilm_surfaceAddNotification(%08x) "
                "Error.%d", window->surfaceid, ret);
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
 *  @brief   genivi ivi-shell surface propaty callback
 *
 *  @param [in] surface           surface id
 *  @param [in] SurfaceProperties surface properties
 *  @param [in] mask              change properties
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::wlGeniviSurfaceNotification(t_ilm_surface surface,
                                              struct ilmSurfaceProperties *SurfaceProperties,
                                              t_ilm_notification_mask mask)
{
    int newVisible;

    ICO_TRA("CicoSCWindowController::wlGeniviSurfaceNotification Enter(%x,,%x)", surface, mask);
    newVisible = SurfaceProperties->visibility ? ICO_SYC_WIN_VISIBLE_SHOW
                                               : ICO_SYC_WIN_VISIBLE_HIDE;
    CicoSCWindow *window = CicoSCWindowController::getInstance()->findWindow(surface);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::wlGeniviSurfaceNotification Leave(surface=%08x not exist)", surface);
        return;
    }
    if ((window->visible != newVisible) ||
        (window->x != (int)SurfaceProperties->destX) ||
        (window->y != (int)SurfaceProperties->destY) ||
        (window->width != (int)SurfaceProperties->destWidth) ||
        (window->height != (int)SurfaceProperties->destHeight))  {
        // change visibility
        window->visible = newVisible;
        window->x = (int)SurfaceProperties->destX;
        window->y = (int)SurfaceProperties->destY;
        window->width = (int)SurfaceProperties->destWidth;
        window->height = (int)SurfaceProperties->destHeight;

        // update attr
        window->visible = newVisible;

        // notify to homescreen
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_CHANGE_ATTR);
        message->addRootObject("appid", window->appid);
        const CicoSCDisplayZone* zone
                = CicoSCWindowController::getInstance()->findDisplayZone(window->zoneid);
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
    }
    else    {
        ICO_TRA("CicoSCWindowController::wlGeniviSurfaceNotification Leave(no change)");
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  setup Genivi ivi-shell notification(callback) finctions
 *
 *  @param      none
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::initializeGeniviNotifications(void)
{
    t_ilm_uint  NumberOfScreens = 16;
    t_ilm_uint  ScreenIds[16];
    t_ilm_layer *pLayerId, *ppLayerId;
    t_ilm_int   LayerNumber;
    t_ilm_uint  *pScreenIds = ScreenIds;
    struct ilmScreenProperties  ScreenProperties;
    int                     idx, DisplayId;
    const CicoSCDisplayConf *DisplayConf;
    const CicoSCLayerConf   *LayerConf;
    int                     LayerId;

    // get all screen id
    memset(ScreenIds, 0, sizeof(ScreenIds));
    if ((ilm_getScreenIDs(&NumberOfScreens, &pScreenIds) != ILM_SUCCESS) ||
        (ilm_commitChanges() != ILM_SUCCESS))   {
        ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                "ilm_getScreenIDs() Error");
        return;
    }
    ICO_TRA("initializeGeniviNotifications: Screens=%d.%x %x %x %x",
            NumberOfScreens, ScreenIds[0], ScreenIds[1], ScreenIds[2], ScreenIds[3]);
    for (idx = 0; idx < (int)NumberOfScreens; idx++) {
        ICO_TRA("CicoSCWindowController::initializeGeniviNotifications: call ilm_getPropertiesOfScreen(%x)", ScreenIds[idx]);
        if ((ilm_getPropertiesOfScreen(ScreenIds[idx], &ScreenProperties) != ILM_SUCCESS) ||
            (ilm_commitChanges() != ILM_SUCCESS))   {
            ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                    "ilm_getPropertiesOfScreen(%d.%x) Error", idx, ScreenIds[idx]);
            continue;
        }
        ICO_TRA("CicoSCWindowController::initializeGeniviNotifications: "
                "Screen[%d.%x] w/h=%d/%d layers=%d",
                idx, ScreenIds[idx], ScreenProperties.screenWidth,
                ScreenProperties.screenHeight, ScreenProperties.layerCount);

        DisplayId = CicoSystemConfig::getInstance()->getDisplayIdbyNo((int)ScreenIds[idx]);
        if ((DisplayId < 0) ||
            ((DisplayConf = CicoSystemConfig::getInstance()->findDisplayConfbyId(DisplayId))
                == NULL))   {
            ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                    "ScreenId.%x not found", ScreenIds[idx]);
        }
        else    {
#if 0       // TODO
            CicoSystemConfig::getInstance()->setDisplaySize(DisplayId,
                                                            ScreenProperties.screenWidth,
                                                            ScreenProperties.screenHeight);
#endif
            // set genivi layers
            for (idx = 0; ; idx++)  {
                LayerConf = CicoSystemConfig::getInstance()->
                                                findLayerConfbyIdx(DisplayId, idx);
                if (! LayerConf)    break;
            }
            pLayerId = (t_ilm_layer *)malloc(sizeof(t_ilm_layer) * idx);
            ppLayerId = pLayerId;
            LayerNumber = 0;

            for (idx = 0; ; idx++)  {
                LayerConf = CicoSystemConfig::getInstance()->
                                                findLayerConfbyIdx(DisplayId, idx);
                if (! LayerConf)    break;

                LayerId = (DisplayId << 16) | LayerConf->id;
                if (ilm_layerCreateWithDimension((t_ilm_layer *)&LayerId,
                            DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)    {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerCreateWithDimension(%x,%d,%d) Error",
                            LayerId, DisplayConf->width, DisplayConf->height);
                }
                else if (ilm_commitChanges() != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_commitChanges() Error");
                }
                else if (ilm_layerSetOpacity(LayerId, (t_ilm_float)1.0f) != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerSetOpacity(%x) Error", LayerId);
                }
                else if (ilm_layerSetSourceRectangle(LayerId, 0, 0,
                             DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerSetSourceRectangle(%x) Error", LayerId);
                }
                else if (ilm_layerSetDestinationRectangle(LayerId, 0, 0,
                             DisplayConf->width, DisplayConf->height) != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerSetDestinationRectangle(%x) Error", LayerId);
                }
                else if (ilm_layerSetOrientation(LayerId, ILM_ZERO) != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerSetOrientation(%x) Error", LayerId);
                }
                else if (ilm_layerSetOrientation(LayerId, ILM_ZERO) != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerSetOrientation(%x) Error", LayerId);
                }
                else if (ilm_layerAddNotification(LayerId, wlGeniviLayerNotification)
                    != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_layerAddNotification(%x) Error", LayerId);
                }
                else    {
                    if (ilm_commitChanges() != ILM_SUCCESS) {
                        ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                                "ilm_commitChanges() Error");
                    }
                    ICO_TRA("initializeGeniviNotifications: layer=%x created(%d,%d)",
                            LayerId, DisplayConf->width, DisplayConf->height);
                    *ppLayerId = LayerId;
                    ppLayerId ++;
                    LayerNumber ++;
                }
            }
            if (LayerNumber > 0)    {
                if (ilm_displaySetRenderOrder(ScreenIds[idx], pLayerId, LayerNumber)
                    != ILM_SUCCESS)   {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_displaySetRenderOrder(%d) Error", LayerNumber);
                }
                else if (ilm_commitChanges() != ILM_SUCCESS) {
                    ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                            "ilm_commitChanges() Error");
                }
                ppLayerId = pLayerId;
                for (idx = 0; idx < LayerNumber; idx++) {
                    if (ilm_layerAddNotification(*ppLayerId, wlGeniviLayerNotification)
                        != ILM_SUCCESS)   {
                        ICO_ERR("CicoSCWindowController::initializeGeniviNotifications "
                                "ilm_layerAddNotification(%x) Error", *ppLayerId);
                    }
                    ppLayerId ++;
                }
            }
            free(pLayerId);
        }
    }
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
        ICO_TRA("not found window object. surfaceid=0x%08X", surfaceid);
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
        ICO_WRN("command: Unknown(0x%08X)", cmd->cmdid);
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
            "(surfaceid=0x%08X zone=%s layerid=%d "
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
#if 1
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
// vim:set expandtab ts=4 sw=4:
