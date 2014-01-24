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
    wayland->getInstance()->addWaylandIF(ICO_WL_OUTPUT_IF, this);
    initDB();
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWindowController::~CicoSCWindowController()
{
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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    int raiseFlag = ICO_WINDOW_MGR_V_NOCHANGE;
    if (animationTime & ICO_SYC_WIN_SURF_RAISE) {
        raiseFlag = ICO_WINDOW_MGR_RAISE_RAISE;
        window->raise = true;
    }
    else if (animationTime & ICO_SYC_WIN_SURF_LOWER)    {
        raiseFlag = ICO_WINDOW_MGR_RAISE_LOWER;
        window->raise = false;
    }
    else if (((animationTime & ICO_SYC_WIN_SURF_NOCHANGE) == 0) &&
             (false == window->raise))   {
        raiseFlag = ICO_WINDOW_MGR_RAISE_RAISE;
        window->raise = true;
    }
    if ((NULL != animation) && (animation[0] != '\0')) {
        // set animation request to Multi Window Manager
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                    ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                    animation, animationTime & ~ICO_SYC_WIN_SURF_FLAGS);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                 ICO_WINDOW_MGR_VISIBLE_SHOW,
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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                       animation, animationTime & ~ICO_SYC_WIN_SURF_FLAGS);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                    ICO_WINDOW_MGR_VISIBLE_HIDE,
                                    ICO_WINDOW_MGR_V_NOCHANGE, animaFlag);

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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_RESIZE,
                                       animation, animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid, window->nodeid,
                                      ICO_WINDOW_MGR_V_NOCHANGE,
                                      ICO_WINDOW_MGR_V_NOCHANGE,
                                      w, h, animaFlag);

    // flush display
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::resize Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  move window(surface) size
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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_MOVE,
                                       animation, animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    int moveNodeId = ICO_WINDOW_MGR_V_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setPositionsize(window->surfaceid,
                                      moveNodeId, x, y,
                                      ICO_WINDOW_MGR_V_NOCHANGE,
                                      ICO_WINDOW_MGR_V_NOCHANGE,
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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWindowController::setAnimation(window->surfaceid,
                                             ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                             animation, animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid,
                                 ICO_WINDOW_MGR_V_NOCHANGE,
                                 ICO_WINDOW_MGR_RAISE_RAISE, animaFlag);

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

    int moveNodeId = ICO_WINDOW_MGR_V_NOCHANGE;
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

    int moveX = ICO_WINDOW_MGR_V_NOCHANGE;
    if (0 <= x) {
        moveX = x;
    }

    int moveY = ICO_WINDOW_MGR_V_NOCHANGE;
    if (0 <= y) {
        moveY = y;
    }

    int moveW = ICO_WINDOW_MGR_V_NOCHANGE;
    if (0 <= w) {
        moveW = w;
    }

    int moveH = ICO_WINDOW_MGR_V_NOCHANGE;
    if (0 <= h) {
        moveH = h;
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
                                      ICO_WINDOW_MGR_FLAGS_ANIMATION);

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
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        CicoSCWlWinMgrIF::setAnimation(window->surfaceid,
                                       ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                       animation, animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    CicoSCWlWinMgrIF::setVisible(window->surfaceid, ICO_WINDOW_MGR_V_NOCHANGE,
                                 ICO_WINDOW_MGR_RAISE_LOWER, animaFlag);

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
            "(surfaceid=0x%08X layerid=%x)", surfaceid, layerid);

    // find window information in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // find layer information in layer list
    CicoSCLayer* layer = findLayer(window->displayid, layerid);
    if (NULL == layer) {
        // check special layer
        if ((layerid != ICO_WINDOW_MGR_LAYERTYPE_TOUCH) &&
            (layerid != ICO_WINDOW_MGR_LAYERTYPE_CURSOR)) {
            ICO_TRA("CicoSCWindowController::setWindowLayer Leave(ENOENT[disp=%d,layer=%d])",
                    window->displayid, layerid);
            return ICO_SYC_ENOENT;
        }
    }

    // update window attr
    window->layerid = layerid;

    // set window layer request to Multi Window Manager
    CicoSCWlWinMgrIF::setWindowLayer(window->surfaceid, window->layerid);

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

    CicoSCWlWinMgrIF::setLayerVisible(layerid, ICO_WINDOW_MGR_VISIBLE_SHOW);

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

    CicoSCWlWinMgrIF::setLayerVisible(layerid, ICO_WINDOW_MGR_VISIBLE_HIDE);

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
        target = ICO_WINDOW_MGR_ACTIVE_POINTER  |
                 ICO_WINDOW_MGR_ACTIVE_KEYBOARD;
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
 *  @brief   set surface map buffer
 *
 *  @param [in] shmname     shared memory name (POSIX I/F)
 *  @param [in] bufsize     shared memory buffer size
 *  @param [in] bufnum      number of shared memory buffer
 *
 *  @return ICO_SYC_EOK on success, other on error(currently EOK only)
 *  @retval ICO_SYC_EOK     success
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::setmapBuffer(const char *shmname, int bufsize, int bufnum)
{
    ICO_TRA("CicoSCWindowController::setmapBuffer Enter");

    CicoSCWlWinMgrIF::setmapBuffer(shmname, bufsize, bufnum);
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::setmapBuffer Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   map surface
 *
 *  @param [in] surfaceid   wayland surface id
 *  @param [in] framerate   surface change frame rate(frames par sec)
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ESRCH   error(not initialized)
 *  @retval ICO_SYC_ENOENT  error(layer dose not exist)
 */
//--------------------------------------------------------------------------
int
CicoSCWindowController::mapSurface(int surfaceid, int framerate)
{
    ICO_TRA("CicoSCWindowController::mapSurface Enter");

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

    CicoSCWlWinMgrIF::mapSurface(surfaceid, framerate);
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
    ICO_TRA("CicoSCWindowController::unmapSurface Enter");

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
        attributes |= ICO_WINDOW_MGR_ATTR_FIXED_ASPECT;
        if (true == zone->aspectAlignLeft) {
            attributes |= ICO_WINDOW_MGR_ATTR_ALIGN_LEFT;
        }
        if (true == zone->aspectAlignRight) {
            attributes |= ICO_WINDOW_MGR_ATTR_ALIGN_RIGHT;
        }
        if (true == zone->aspectAlignTop) {
            attributes |= ICO_WINDOW_MGR_ATTR_ALIGN_TOP;
        }
        if (true == zone->aspectAlignBottom) {
            attributes |= ICO_WINDOW_MGR_ATTR_ALIGN_BOTTOM;
        }
    }

    CicoSCWlWinMgrIF::setAttributes(surfaceid, attributes);
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_TRA("CicoSCWindowController::setAttributes Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland surface create callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] winname         surface window name(title)
 *  @param [in] pid             wayland client process Id
 *  @param [in] appid           wayland client application Id
 *  @param [in] layertype       surface layer type
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::createdCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t              surfaceid,
                                  const char            *winname,
                                  int32_t               pid,
                                  const char            *appid,
                                  int32_t               layertype)
{
    ICO_TRA("CicoSCWindowController::createdCB Enter"
            "(surfaceid=0x%08X winname=%s pid=%d appid=%s layer=%x)",
            surfaceid, winname, pid, appid, layertype);

    if (layertype == ICO_WINDOW_MGR_LAYERTYPE_INPUTPANEL)   {
        // So far, SystemController ignores this event
        // because it does not support InputPanel(Software Keyboard).
        ICO_WRN("Input Panel layer create window information skip.");
        ICO_TRA("CicoSCWindowController::createdCB Leave(Input Panel)");
        return;
    }

    CicoSCWindow* window = new CicoSCWindow();

    window->surfaceid = surfaceid;
    window->name      = winname;
    window->appid     = appid;
    window->layertype = layertype;
    window->pid       = pid;
    window->displayid = ICO_SURFACEID_2_NODEID(surfaceid);

    CicoSCLifeCycleController* appctrl;
    appctrl = CicoSCLifeCycleController::getInstance();
    const CicoAilItems *ailItem = appctrl->findAIL(appid);
    if (NULL == ailItem) {
        const CicoAulItems* aulitem = appctrl->findAUL(pid);
        if (NULL != aulitem) {
            window->appid = aulitem->m_appid;
            ICO_DBG("appid=%s", window->appid.c_str());
            ailItem = appctrl->findAIL(window->appid.c_str());
        }
        else {
            ICO_DBG("application information not found.");
        }
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
        ICO_TRA("CicoSCWindowController::createdCB Leave(ENOENT)");
        return;
    }

    appctrl->enterAUL(appid, pid, window);

    // dump log window information
    window->dump();

    m_windowList[surfaceid] = window;

    // set surface attributes
    setAttributes(surfaceid);

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

    ICO_TRA("CicoSCWindowController::createdCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland change surface name callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] winname         surface window name(title)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::nameCB(void                  *data,
                               struct ico_window_mgr *ico_window_mgr,
                               uint32_t              surfaceid,
                               const char            *winname)
{
    ICO_TRA("CicoSCWindowController::nameCB Enter"
            "(surfaceid=0x%08X winname=%s)", surfaceid, winname);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::nameCB Leave");
        return;
    }

    ICO_DBG("nameCB: Update window name %s=>%s",
            window->name.c_str(), winname);
    window->name = winname;

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NAME);
    message->addRootObject("appid", window->appid);
    message->addRootObject("pid", window->pid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::nameCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface destroy callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::destroyedCB(void                  *data,
                                    struct ico_window_mgr *ico_window_mgr,
                                    uint32_t              surfaceid)
{
    ICO_TRA("CicoSCWindowController::destroyedCB Enter"
            "(surfaceid=0x%08X)", surfaceid);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::destroyedCB Leave");
        return;
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

    ICO_TRA("CicoSCWindowController::destroyedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] visible         surface visible
 *                              (1=visible/0=invisible/other=no-change)
 *  @param [in] raise           surface raise
 *                              (1=raise/0=lower/other=no change)
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::visibleCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t              surfaceid,
                                  int32_t               visible,
                                  int32_t               raise,
                                  int32_t               hint)
{
    ICO_TRA("CicoSCWindowController::visibleCB Enter"
            "(surfaceid=0x%08X visible=%d raise=%d hint=%d)",
            surfaceid, visible, raise, hint);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::visibleCB Leave");
        return;
    }

    int newVisible = visible;
    int newRaise   = raise;

    if (visible == ICO_WINDOW_MGR_V_NOCHANGE) {
        newVisible = window->visible;
    }

    if (raise == ICO_WINDOW_MGR_V_NOCHANGE) {
        newVisible = window->raise;
    }

    if ((window->visible == newVisible) && (window->raise == newRaise) ){
        ICO_TRA("CicoSCWindowController::visibleCB Leave");
        return;
    }

    // update attr
    window->visible = newVisible;
    window->raise = newRaise;

    // notify homescreen
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

    ICO_TRA("CicoSCWindowController::visibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface configure callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] node            surface node Id
 *  @param [in] layertype       surface layer type
 *  @param [in] layer           surface layer Id
 *  @param [in] x               surface upper-left X coordinate
 *  @param [in] y               surface upper-left Y coordinate
 *  @param [in] width           surface width
 *  @param [in] height          surface height
 *  @param [in] hint            client request
 *                              (1=client request(not changed)/0=changed)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::configureCB(void                  *data,
                                    struct ico_window_mgr *ico_window_mgr,
                                    uint32_t              surfaceid,
                                    uint32_t              node,
                                    int32_t               layertype,
                                    uint32_t              layer,
                                    int32_t               x,
                                    int32_t               y,
                                    int32_t               width,
                                    int32_t               height,
                                    int32_t               hint)
{
    ICO_TRA("CicoSCWindowController::configureCB Enter"
            "(surfaceid=0x%08X node=%d layer=%x.%x x=%d y=%d "
            "width=%d height=%d hint=%d)",
            surfaceid, node, layertype, layer, x, y, width, height, hint);

    if (layertype == ICO_WINDOW_MGR_LAYERTYPE_INPUTPANEL)   {
        // So far, SystemController ignores this event
        // because it does not support InputPanel(Software Keyboard).
        ICO_TRA("CicoSCWindowController::configureCB Leave(Input Panel)");
        return;
    }
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::visibleCB Leave"
                "(update window visible failed)");
        return;
    }

    if (ICO_WINDOW_MGR_HINT_CHANGE == hint) {
        window->x      = x;
        window->y      = y;
        window->width  = width;
        window->height = height;
        window->layertype = layertype;
    }

    // send message
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
    message->addArgObject("layertype", window->layertype);
    message->addArgObject("layer", window->layerid);
    message->addArgObject("pos_x", window->x);
    message->addArgObject("pos_y", window->y);
    message->addArgObject("width", window->width);
    message->addArgObject("height", window->height);
    message->addArgObject("raise", window->raise ? 1 : 0);
    message->addArgObject("visible", window->visible ? 1 : 0);
    message->addArgObject("active", window->active ? 1 : 0);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::configureCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface active callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] active          surface active
 *                              (1=active/0=not active)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::activeCB(void                  *data,
                                 struct ico_window_mgr *ico_window_mgr,
                                 uint32_t              surfaceid,
                                 int32_t               active)
{
    ICO_TRA("CicoSCWindowController::activeCB Enter"
            "(surfaceid=0x%08X active=%d)", surfaceid, active);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("not found window information");
        ICO_TRA("CicoSCWindowController::activeCB Leave(not found window)");
        return;
    }

    if (0 == active) {
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
 *  @brief  wayland layer visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] layer           layer Id
 *  @param [in] visible         layer visible
 *                              (1=visible/0=invisible/other=no change)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::layerVisibleCB(void                  *data,
                                       struct ico_window_mgr *ico_window_mgr,
                                       uint32_t              layer,
                                       int32_t               visible)
{
    ICO_TRA("CicoSCWindowController::layerVisibleCB Enter"
            "layer=%d visible=%d)", layer, visible);

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_LAYER_ATTR);
    message->addRootObject("appid", "");
    message->addArgObject("layer", layer);
    message->addArgObject("visible", visible);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCWindowController::layerVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  query application surface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] appid           application Id
 *  @param [in] surface         surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::appSurfacesCB(void                  *data,
                                      struct ico_window_mgr *ico_window_mgr,
                                      const char            *appid,
                                      struct wl_array       *surfaces)
{
    ICO_TRA("CicoSCWindowController::appSurfacesCB Enter(appid=%s)", appid);

    //struct wl_array {
    //  size_t size;
    //  size_t alloc;
    //  void *data;
    //};
    uint32_t **p;
    for (p = (uint32_t**)(surfaces)->data;
         (const char*) p < ((const char*) (surfaces)->data + (surfaces)->size);
         (p)++) {
        ICO_DBG("appSurfacesCB: surface=%d", (int)*p);
    }

    ICO_TRA("CicoSCWindowController::appSurfacesCB Leave");
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
 *  @param [in] target          surface buffer target(EGL buffer name)
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
                                     uint32_t              target,
                                     int32_t               width,
                                     int32_t               height,
                                     int32_t               stride,
                                     uint32_t               format)
{
    int command;

    ICO_TRA("CicoSCWindowController::mapSurfaceCB Enter(ev=%d surf=%x "
            "type=%d target=%d w/h=%d/%d stride=%d form=%x)",
            event, surfaceid, type, target, width, height, stride, format);

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
    message->addArgObject("name", target);
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
        ICO_WRN("not found window object. surfaceid=0x%08X", surfaceid);
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
    case MSG_CMD_MAP_BUFFER:
        ICO_DBG("command: MSG_CMD_MAP_BUFFER");
        (void)setmapBuffer(opt->animation.c_str(), opt->width, opt->height);
        break;
    case MSG_CMD_MAP_THUMB:
        ICO_DBG("command: MSG_CMD_MAP_THUMB");
        (void)mapSurface(opt->surfaceid, opt->framerate);
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