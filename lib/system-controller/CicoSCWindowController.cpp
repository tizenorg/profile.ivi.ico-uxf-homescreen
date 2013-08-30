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
 *  @brief  
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

#include "CicoSCSystemConfig.h"
#include "CicoSCConf.h"
#include "CicoSCDisplay.h"
#include "CicoSCLayer.h"
#include "CicoSCDisplayZone.h"
#include "ico_syc_error.h"
#include "CicoSCCommandParser.h"
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
    : m_resMgr(NULL)
{
    CicoSCWayland* wayland = CicoSCWayland::getInstance();
    wayland->getInstance()->addWaylandIF(ICO_WL_WIN_MGR_IF, this);
    wayland->getInstance()->addWaylandIF(ICO_WL_OUTPUT_IF, this);
    wayland->getInstance()->addWaylandIF(ICO_WL_SHM_IF, this);
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
    ICO_DBG("CicoSCWindowController::initDB: Enter");

    const vector<CicoSCDisplayConf*>& displayList =
            CicoSCSystemConfig::getInstance()->getDisplayConfList();
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = displayList.begin();
    for (; itr != displayList.end(); ++itr) {
        const CicoSCDisplayConf *dconf = const_cast<CicoSCDisplayConf*>(*itr);
        CicoSCDisplay *display = new CicoSCDisplay();

        display->displayid = dconf->id;
        display->type      = dconf->type;
        display->nodeid    = dconf->node;
        display->displayno = dconf->no;
        display->width     = dconf->width;
        display->height    = dconf->height;
        display->inch      = dconf->inch;
        display->name      = dconf->name;

        vector<CicoSCLayerConf*>::const_iterator itr2;
        itr2 = dconf->layerConfList.begin();
        for (; itr2 != dconf->layerConfList.end(); ++itr2) {
            const CicoSCLayerConf *lconf = const_cast<CicoSCLayerConf*>(*itr2);

            CicoSCLayer *layer = new CicoSCLayer();
            layer->layerid     = lconf->id;
            layer->type        = lconf->type;
            layer->width       = display->width;
            layer->height      = display->height;
            layer->displayid   = display->displayid;
            layer->menuoverlap = lconf->menuoverlap;
            display->layerList.push_back(layer);
        }

        vector<CicoSCDisplayZoneConf*>::const_iterator itr3;
        itr3 = dconf->zoneConfList.begin();
        for (; itr3 != dconf->zoneConfList.end(); ++itr3) {
//            const CicoSCDisplayZoneConf *dzconf = const_cast<CicoSCLayerConf*>(*itr2);

            CicoSCDisplayZone *zone = new CicoSCDisplayZone();
            zone->zoneid = (*itr3)->id;
            zone->x      = (*itr3)->x;
            zone->y      = (*itr3)->y;
            zone->width  = (*itr3)->w;
            zone->height = (*itr3)->h;
            display->zoneList.push_back(zone);
        }
        display->dump();
        m_displayList.push_back(display);

        display->dump();
        m_displayList.push_back(display);
    }

    ICO_DBG("CicoSCWindowController::initDB: Leave");
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
    ICO_DBG("CicoSCWindowController::show Enter"
            "(surfaceid=%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::show Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->visible = true;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    int raiseFlag = ICO_WINDOW_MGR_RAISE_RAISE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ICO_DBG("ico_window_mgr_set_animation"
                "(surface=0x%08X type=%d anima=%s time=%d)",
                window->surfaceid,
                ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                animation, animationTime);
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
        raiseFlag = ICO_WINDOW_MGR_V_NOCHANGE;
    }

    // show request to Multi Window Manager
    ICO_DBG("ico_window_mgr_set_visible called."
            "(surfaceid=0x%08X visible=%d raise=%d anima=%d",
            window->surfaceid, ICO_WINDOW_MGR_VISIBLE_SHOW,
            raiseFlag, animaFlag);
    ico_window_mgr_set_visible(m_winmgr,
                               window->surfaceid,
                               ICO_WINDOW_MGR_VISIBLE_SHOW,
                               raiseFlag,
                               animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::show Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::hide Enter"
            "(surfaceid=%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::hide Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update window attr
    window->visible = false;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // show request to Multi Window Manager
    ico_window_mgr_set_visible(m_winmgr,
                               window->surfaceid,
                               ICO_WINDOW_MGR_VISIBLE_HIDE,
                               ICO_WINDOW_MGR_V_NOCHANGE,
                               animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::hide Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::resize Enter"
            "(surfaceid=%08X h=%d w=%d animation=%s animationTime=%d)",
            surfaceid, w, h, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::resize Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_RESIZE,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    ico_window_mgr_set_positionsize(m_winmgr,
                                    window->surfaceid,
                                    0, //ICO_WINDOW_MGR_V_NOCHANGE,
                                    ICO_WINDOW_MGR_V_NOCHANGE,
                                    ICO_WINDOW_MGR_V_NOCHANGE,
                                    w,
                                    h,
                                    animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::resize Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::move Enter"
            "(surfaceid=%08X nodeid=%d x=%d y=%d "
            "animation=%s animationTime=%d)",
            surfaceid, nodeid, x, y, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::move Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_MOVE,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    int moveNodeId = ICO_WINDOW_MGR_V_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }

    // set visible request to Multi Window Manager
    ico_window_mgr_set_positionsize(m_winmgr,
                                    window->surfaceid,
                                    moveNodeId,
                                    x,
                                    y,
                                    ICO_WINDOW_MGR_V_NOCHANGE,
                                    ICO_WINDOW_MGR_V_NOCHANGE,
                                    animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::move Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::raise Enter"
            "(surfaceid=%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::raise Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->raise = true;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    ico_window_mgr_set_visible(m_winmgr,
                               window->surfaceid,
                               ICO_WINDOW_MGR_V_NOCHANGE,
                               ICO_WINDOW_MGR_RAISE_RAISE,
                               animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::raise Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set window(surface) geometry
 *
 *  @param [in] surfaceid           wayland surface id
 *  @param [in] nodeid              node id
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
                                    int        x,
                                    int        y,
                                    int        w,
                                    int        h,
                                    const char *resizeAnimation,
                                    int        resizeAnimationTime,
                                    const char *moveAnimation,
                                    int        moveAnimationTime)
{
    ICO_DBG("CicoSCWindowController::setGeometry Enter"
            "(surfaceid=0x%08X nodeid=%d x=%d y=%d w=%d h=%d "
            "resizeAnimation=%s resizeAnimationTime=%d "
            "moveAnimation=%s moveAnimationTime=%d)",
            surfaceid, nodeid, x, y, w, h,
            resizeAnimation, resizeAnimationTime,
            moveAnimation, moveAnimationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::setGeometry Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if (NULL != resizeAnimation) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_RESIZE,
                                     resizeAnimation,
                                     resizeAnimationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    if (NULL != moveAnimation) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_MOVE,
                                     moveAnimation,
                                     moveAnimationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    int moveNodeId = ICO_WINDOW_MGR_V_NOCHANGE;
    if (nodeid >= 0) {
        moveNodeId = nodeid;
    }
    else {
        moveNodeId = window->nodeid;
    }

    // set visible request to Multi Window Manager
    ICO_DBG("ico_window_mgr_set_positionsize"
            "(surfaceid=0x%08X nodeid=%d x=%d y=%d w=%d h=%d anima=%d)",
            window->surfaceid, moveNodeId, x, y, w, h, animaFlag);
    ico_window_mgr_set_positionsize(m_winmgr,
                                    window->surfaceid,
                                    moveNodeId,
                                    x,
                                    y,
                                    w,
                                    h,
                                    animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::setGeometry Leave(EOK)");
    return ICO_SYC_EOK;
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
    ICO_DBG("CicoSCWindowController::lower Enter"
            "(surfaceid=%08X animation=%s animationTime=%d)",
            surfaceid, animation, animationTime);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::lower Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update visible attr
    window->raise = false;

    // set animation request to Multi Window Manager
    int animaFlag = ICO_WINDOW_MGR_FLAGS_NO_CONFIGURE;
    if ((NULL != animation) && (animation[0] != '\0')) {
        ico_window_mgr_set_animation(m_winmgr,
                                     window->surfaceid,
                                     ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE,
                                     animation,
                                     animationTime);
        animaFlag = ICO_WINDOW_MGR_FLAGS_ANIMATION;
    }

    // set visible request to Multi Window Manager
    ico_window_mgr_set_visible(m_winmgr,
                               window->surfaceid,
                               ICO_WINDOW_MGR_V_NOCHANGE,
                               ICO_WINDOW_MGR_RAISE_LOWER,
                               animaFlag);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::lower Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::setWindowLayer Enter"
            "(surfaceid=%08X layerid=%d)", surfaceid, layerid);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::setWindowLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // find layer infomation in layer list
    CicoSCLayer* layer = findLayer(window->displayid, layerid);
    if (NULL == layer) {
        ICO_WRN("CicoSCWindowController::setWindowLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    // update window attr
    window->layerid = layerid;

    // set window layer request to Multi Window Manager
    ICO_DBG("ico_window_mgr_set_window_layer called."
            "surfaceid=0x%08X layerid=%d",
            window->surfaceid, window->layerid);
    ico_window_mgr_set_window_layer(m_winmgr,
                                    window->surfaceid,
                                    window->layerid);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::setWindowLayer Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::showLayer Enter"
            "displayid=%d layerid=%d)", displayid, layerid);

    // find layer infomation in layer list
    CicoSCLayer* layer = findLayer(displayid, layerid);
    if (NULL == layer) {
        ICO_WRN("CicoSCWindowController::showLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    setLayerVisible(layerid, ICO_WINDOW_MGR_VISIBLE_SHOW);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::showLayer Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::hideLayer Enter"
            "displayid=%d layerid=%d)",
            displayid, layerid);

    // find layer infomation in layer list
    CicoSCLayer* layer = findLayer(displayid, layerid);
    if (NULL == layer) {
        ICO_WRN("CicoSCWindowController::hideLayer Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    setLayerVisible(layerid, ICO_WINDOW_MGR_VISIBLE_HIDE);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    ICO_DBG("CicoSCWindowController::hideVisible Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   control layer visibility
 *
 *  @param [in] layerid     layer id
 *  @param [in] visible     layer show
 *                          ICO_WINDOW_MGR_VISIBLE_SHOW
 *                          ICO_WINDOW_MGR_VISIBLE_HIDE
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::setLayerVisible(int layerid, int visible)
{
    // set layer visible request to Multi Window Manager
    ICO_DBG("ico_window_mgr_set_layer_visible(%d,%d) called.",
            layerid, visible);
    ico_window_mgr_set_layer_visible(m_winmgr, layerid, visible);
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
#if 0
    Ico_Uxf_Mng_Window  *winmng;
    Ico_Uxf_Mng_Process *proc;
    Ico_Uxf_Mng_Process *aproc;
    int                 hash;
#endif

    ICO_DBG("CicoSCWindowController::active Enter"
            "(surfaceid=%08X, target=%08X)", surfaceid, target);

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::active Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    if (target < 0) {
        target = ICO_WINDOW_MGR_ACTIVE_POINTER  |
                 ICO_WINDOW_MGR_ACTIVE_KEYBOARD |
                 ICO_WINDOW_MGR_ACTIVE_SELECTED;
    }

    ICO_DBG("ico_window_mgr_set_active(0x%08X, %d) called.",
            surfaceid, target);
    // set active request to Multi Window Manager
    ico_window_mgr_set_active(m_winmgr, window->surfaceid, target);

    // flush display 
    CicoSCWayland::getInstance()->flushDisplay();

    // update active window
    map<unsigned int, CicoSCWindow*>::iterator itr;
    itr = m_windowList.begin();
    for (; itr != m_windowList.end(); ++itr) {
        CicoSCWindow* window = itr->second;
        if (window->surfaceid == surfaceid) {
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

    // TODO update active application
#if 0
    /* reset all active application without this application    */
    aproc = NULL;
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)    {
            if (proc->attr.mainwin.window == window)    {
                aproc = proc;
            }
            else    {
                if (target == 0)    {
                    proc->attr.active = 0;
                }
                else    {
                    proc->attr.active &= ~target;
                }
            }
            proc = proc->nextidhash;
        }
    }
    aproc->attr.active = target;
#endif

    ICO_DBG("CicoSCWindowController::active Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   map surface
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
CicoSCWindowController::mapSurface(int surfaceid, int framerate)
{
    ICO_WRN("CicoSCWindowController::mapSurface Enter");

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::mapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    if (framerate <= 0) {
        framerate = 15;
    }

    ICO_DBG("ico_window_mgr_map_surface(%d, %d) called.",
            surfaceid, framerate);
    ico_window_mgr_map_surface(m_winmgr, surfaceid, framerate);
    ICO_DBG("CicoSCWindowController::mapSurface Leave(EOK)");
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
    ICO_DBG("CicoSCWindowController::unmapSurface Enter");

    // find window infomation in window list
    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_WRN("CicoSCWindowController::unmapSurface Leave(ENOENT)");
        return ICO_SYC_ENOENT;
    }

    ICO_DBG("ico_window_mgr_unmap_surface(%d) called.", surfaceid);
    ico_window_mgr_unmap_surface(m_winmgr, surfaceid);

    ICO_DBG("CicoSCWindowController::unmapSurface Leave(EOK)");
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
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::createdCB(void                  *data,
                                  struct ico_window_mgr *ico_window_mgr,
                                  uint32_t              surfaceid,
                                  const char            *winname,
                                  int32_t               pid,
                                  const char            *appid)
{
    ICO_DBG("CicoSCWindowController::createdCB Enter"
            "(surfaceid=%08X winname=%s pid=%d appid=%s)",
            surfaceid, winname, pid, appid);

    CicoSCWindow* window = new CicoSCWindow();

    window->surfaceid = surfaceid;
    window->name      = winname;
    window->appid     = appid;
    window->pid       = pid;
    window->displayid = ICO_SURFACEID_2_NODEID(surfaceid);

    CicoSCLifeCycleController* appctrl;
    appctrl = CicoSCLifeCycleController::getInstance();
    const CicoSCAilItems *ailItem = appctrl->findAIL(appid);
    if (NULL == ailItem) {
        const CicoSCAulItems* aulitem = appctrl->findAUL(pid);
        if (NULL != aulitem) {
            window->appid = aulitem->m_appid;
            ICO_DBG("appid=%s", window->appid.c_str());
            ailItem = appctrl->findAIL(window->appid.c_str());
        }
        else {
            ICO_DBG("aulitem not found.");
        }
    }

    string fullZoneName;
    if (NULL != ailItem) {
        window->layerid = ailItem->m_layer;
        window->zoneid  = ailItem->m_displayZone;
        window->nodeid  = ailItem->m_nodeID;
        if ((window->displayid >= 0) && (window->zoneid >= 0)) {
            // TODO at errer try catch
            CicoSCDisplay* display = m_displayList.at(window->displayid);
            CicoSCDisplayZone *zone = display->zoneList.at(window->zoneid);
            window->x       = zone->x;
            window->y       = zone->y;
            window->width   = zone->width;
            window->height  = zone->height;

            //TODO
            //fullZoneName = display->name + "." + zone->name;
            fullZoneName = "Center.Full";
        }
    }
    else{
        ICO_WRN("ail item not found.");
        ICO_WRN("CicoSCWindowController::createdCB Leave(ENOENT)");
        return;
    }

    appctrl->enterAUL(appid, pid, window);

    // dump log window infomartion
    window->dump();

    m_windowList[surfaceid] = window;

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CREATE);
    message->addRootObject("appid", window->appid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    if (NULL != m_resMgr) {
        CicoSCCommand cmd;
        CicoSCCmdResCtrlOpt *opt = new CicoSCCmdResCtrlOpt();

        cmd.cmdid = MSG_CMD_ACQUIRE_RES;
        cmd.appid = window->appid;
        cmd.pid   = window->pid;
        cmd.opt = opt;
        
        opt->displayres  = true;
        opt->displayZone = fullZoneName;
        opt->windowName  = window->name;
        opt->surfaceid   = window->surfaceid;

        m_resMgr->handleCommand((const CicoSCCommand&)cmd, true);
    }
    else {
        //show(window->surfaceid, NULL, 0);
    }

    ICO_DBG("CicoSCWindowController::createdCB Leave");
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
    ICO_DBG("CicoSCWindowController::nameCB Enter"
            "(surfaceid=%08X winname=%s)", surfaceid, winname);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_DBG("CicoSCWindowController::nameCB Leave");
        return;
    }

    ICO_DBG("nameCB: Update window name %s=>%s",
            window->name.c_str(), winname);
    window->name = winname;

    ICO_DBG("CicoSCWindowController::nameCB Leave");
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
    ICO_DBG("CicoSCWindowController::destroyedCB Enter");

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_DBG("CicoSCWindowController::destroyedCB Leave");
        return;
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_DESTROY);
    message->addRootObject("appid", window->appid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    // TODO delete window in application

    // delete window in list
    m_windowList.erase(window->surfaceid);
    delete(window);
 
    ICO_DBG("CicoSCWindowController::destroyedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] visible         surface visible
 *                              (1=visible/0=unvisible/other=nochange)
 *  @param [in] raise           surface raise
 *                              (1=raise/0=lower/other=nochange)
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
    ICO_DBG("CicoSCWindowController::visibleCB Enter"
            "(surfaceid=%08X visible=%d raise=%d hint=%d)",
            surfaceid, visible, raise, hint);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_DBG("CicoSCWindowController::visibleCB Leave");
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
        ICO_DBG("CicoSCWindowController::visibleCB Leave");
        return;
    }

    // notify homescreen
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_ATTR);
    message->addRootObject("appid", window->appid);
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

    ICO_DBG("CicoSCWindowController::visibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland surface configure callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] surfaceid       ico_window_mgr surface Id
 *  @param [in] node            surface node Id
 *  @param [in] x               surface upper-left X coodinate
 *  @param [in] y               surface upper-left Y coodinate
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
                                    uint32_t              layer,
                                    int32_t               x,
                                    int32_t               y,
                                    int32_t               width,
                                    int32_t               height,
                                    int32_t               hint)
{
    ICO_DBG("CicoSCWindowController::configureCB Enter"
            "surfaceid=%08X node=%d layer=%d x=%d y=%d "
            "width=%d height=%d hint=%d",
            surfaceid, node, layer, x, y, width, height, hint);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_DBG("CicoSCWindowController::visibleCB Leave"
                "(update window visible failed)");
        return;
    }

    if (ICO_WINDOW_MGR_HINT_CHANGE == hint) {
        window->x      = x;
        window->y      = y;
        window->width  = width;
        window->height = height;
#if 0
        if (window->layerid != layer) {
            window->layerid = layer;
            layer = findLayer(window->displayid, layer);
        }
#endif
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_ATTR);
    message->addRootObject("appid", window->appid);
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

    ICO_DBG("CicoSCWindowController::configureCB Leave");
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
    ICO_DBG("CicoSCWindowController::activeCB Enter"
            "(surfaceid=%08X active=%d)", surfaceid, active);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        ICO_DBG("CicoSCWindowController::activeCB Leave");
        return;
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_ACTIVE);
    message->addRootObject("appid", window->appid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("winname", window->name);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_DBG("CicoSCWindowController::activeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  wayland layer visible callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] layer           layer Id
 *  @param [in] visible         layer visible
 *                              (1=visible/0=unvisible/other=nochange)
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::layerVisibleCB(void                  *data,
                                       struct ico_window_mgr *ico_window_mgr,
                                       uint32_t              layer,
                                       int32_t               visible)
{
    ICO_DBG("CicoSCWindowController::layerVisibleCB Enter"
            "layer=%d visible=%d)", layer, visible);

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_CHANGE_LAYER_ATTR);
    message->addRootObject("appid", "");
    message->addArgObject("layer", layer);
    message->addArgObject("visible", visible);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_DBG("CicoSCWindowController::layerVisibleCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  query applicationsurface callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] appid           application Id
 *  @param [in] suface          surface Id array
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::appSurfacesCB(void                  *data,
                                      struct ico_window_mgr *ico_window_mgr,
                                      const char            *appid,
                                      struct wl_array       *surfaces)
{
    ICO_DBG("CicoSCWindowController::appSurfacesCB Enter(appid=%s)", appid);

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

    ICO_DBG("CicoSCWindowController::appSurfacesCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   surface map event callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] ico_window_mgr  wayland ico_window_mgr plugin interface
 *  @param [in] event           event
 *  @param [in] surfaceid       surface Id
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
                                     int32_t               width,
                                     int32_t               height,
                                     int32_t               stride,
                                     int32_t               format)
{
    ICO_DBG("CicoSCWindowController::mapSurfaceCB Enter"
            "(event=%d surface=%d width=%d height=%d stride=%d format=%d)",
            event, surfaceid, width, height, stride, format);

    CicoSCWindow *window = findWindow(surfaceid);
    if (NULL == window) {
        return;
    }

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_MAP_THUMB);
    message->addRootObject("appid", window->appid);
    message->addArgObject("surface", window->surfaceid);
    message->addArgObject("width", width);
    message->addArgObject("height", height);
    message->addArgObject("stride", stride);
    message->addArgObject("format", format);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_DBG("CicoSCWindowController::mapSurfaceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   wayland display attribute callback
 *
 *  @param [in] data            user data(unused)
 *  @param [in] wl_output       wayland wl_output interface
 *  @param [in] x               display upper-left X coodinate
 *  @param [in] y               display upper-left Y coodinate
 *  @param [in] physical_width  display physical width
 *  @param [in] physical_height display physical height
 *  @param [in] subpixel        display sub pixcel
 *  @param [in] make            display maker
 *  @param [in] model           diaplay model
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
    ICO_DBG("CicoSCWlWinMgrIF::outputGeometryCB Enter"
            "(x=%d y=%d physical_width=%d physical_height=%d "
            "subpixel=%d make=%s model=%s transform=%d)",
            x, y, physical_width, physical_height,
            subpixel, make, model, transform);

    if (0 == m_displayList.size()) {
        ICO_DBG("CicoSCWlWinMgrIF::outputGeometryCB Leave(display is zero");
        return;
    }

    CicoSCDisplay* display = m_displayList.at(0);
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

    ICO_DBG("CicoSCWlWinMgrIF::outputGeometryCB Leave");
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
    ICO_DBG("CicoSCWlWinMgrIF::outputModeCB Enter"
            "(flags=%d width=%d height=%d refresh=%d)",
            flags, width, height, refresh);

    if (0 == m_displayList.size()) {
        ICO_DBG("CicoSCWlWinMgrIF::outputModeCB Leave(display is zero");
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

    ICO_DBG("CicoSCWlWinMgrIF::outputModeCB Leave");
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
        ICO_WRN("not found window object. surfaceid=%d", surfaceid);
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
    CicoSCDisplay* display = NULL;
    CicoSCLayer* layer = NULL;
    try {
        display = m_displayList.at(displayid);
        layer = display->layerList.at(layerid);
    }
    catch (const std::exception& e) {
        ICO_ERR("catch exception %s", e.what());
        return NULL;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        return NULL;
    }
    return layer;
}

//--------------------------------------------------------------------------
/**
 *  @brief  handle command
 *
 *  @param [in] commnad     control commnad
 */
//--------------------------------------------------------------------------
void
CicoSCWindowController::handleCommand(const CicoSCCommand * cmd)
{
    ICO_DBG("CicoSCWindowController::handleCommand Enter(%d)", cmd->cmdid);

    CicoSCCmdWinCtrlOpt *opt = static_cast<CicoSCCmdWinCtrlOpt*>(cmd->opt);

    int ret = 0;
    switch (cmd->cmdid) {
    case MSG_CMD_SHOW:
        ICO_DBG("command: MSG_CMD_SHOW");
        ret = show(opt->surfaceid, opt->animation.c_str(), opt->animationTime);
        break;
    case MSG_CMD_HIDE:
        ICO_DBG("command: MSG_CMD_HIDE");
        ret = hide(opt->surfaceid, opt->animation.c_str(), opt->animationTime);
        break;
    case MSG_CMD_MOVE:
        ICO_DBG("command: MSG_CMD_MOVE");
        ret = setGeometry(opt->surfaceid, opt->nodeid, opt->x, opt->y, 
                          opt->width, opt->height, 
                          opt->animation.c_str(), opt->animationTime,
                          opt->animation.c_str(), opt->animationTime);
        break;
    case MSG_CMD_CHANGE_ACTIVE:
        ICO_DBG("command: MSG_CMD_CHANGE_ACTIVE");
        ret = active(opt->surfaceid, opt->active);
        break;
    case MSG_CMD_CHANGE_LAYER:
        ICO_DBG("command: MSG_CMD_CHANGE_LAYER");
        ret = setWindowLayer(opt->surfaceid, opt->layerid);
        break;
    case MSG_CMD_PREPARE_THUMB:
        ICO_DBG("command: MSG_CMD_PREPARE_THUMB");
        ret = mapSurface(opt->surfaceid, opt->framerate);
        break;
    case MSG_CMD_UNMAP_THUMB:
        ICO_DBG("command: MSG_CMD_UNMAP_THUMB");
        ret = unmapSurface(opt->surfaceid);
        break;
    case MSG_CMD_SHOW_LAYER:
        ICO_DBG("command: MSG_CMD_SHOW_LAYER");
        ret = showLayer(opt->displayid, opt->layerid);
        break;
    case MSG_CMD_HIDE_LAYER:
        ICO_DBG("command: MSG_CMD_HIDE_LAYER");
        ret = hideLayer(opt->displayid, opt->layerid);
        break;
    default:
        ICO_WRN("Unknown Commnad(0x%08X)", cmd->cmdid);
        break;
    }
    ICO_DBG("commnd result(%d)", ret);

    ICO_DBG("CicoSCWindowController::handleCommand Leave");
}
// vim:set expandtab ts=4 sw=4:
