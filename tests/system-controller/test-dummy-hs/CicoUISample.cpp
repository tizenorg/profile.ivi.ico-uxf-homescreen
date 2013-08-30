/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   CicoUISample.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <iostream>
#include <string>

#include "CicoUISample.h"
#include "CicoBaseWin.h"
#include "CicoGeometry.h"
#include "CicoBackground.h"
#include "CicoColor.h"
#include "CicoLog.h"
#include "ico_syc_common.h"
#include "ico_syc_privilege.h"


CicoBackground* _background = NULL;
CicoBackground* _controlbar = NULL;
extern "C" {
static void
_syc_callback(const ico_syc_ev_e event,
              const void *detail, void *user_data)
{
    switch (event) {
    case ICO_SYC_EV_WIN_CREATE:
    {
        ICO_DBG("--- ICO_SYC_EV_WIN_CREATE "
                "(appid[%s], name[%s], surface[%d])",
               ((ico_syc_win_info_t *)detail)->appid,
               ((ico_syc_win_info_t *)detail)->name,
               ((ico_syc_win_info_t *)detail)->surface);

		ico_syc_win_info_t *info = (ico_syc_win_info_t *)detail;
		if (0 == strcmp(info->name, "HSBackground")) {
			ico_syc_change_layer(info->appid, info->surface, 0);
			ico_syc_show(info->appid, info->surface, NULL);
			ico_syc_win_move_t win_move = {NULL, 0, 64, 1080, 1920-64-128};
        	ico_syc_move(info->appid, info->surface, &win_move, NULL);

            ico_syc_prepare_thumb(info->surface, 0);
		}
		if (0 == strcmp(info->name, "HSControllBar")) {
			ico_syc_change_layer(info->appid, info->surface, 1);
			ico_syc_show(info->appid, info->surface, NULL);
			ico_syc_win_move_t win_move = {NULL, 0, 1920-128, 1080, 128};
        	ico_syc_move(info->appid, info->surface, &win_move, NULL);

			ico_syc_change_layer(info->appid, info->surface, 0);
			ico_syc_win_move_t win_move2 = {NULL, 0, 128, 1080, 512};
        	ico_syc_move(info->appid, info->surface, &win_move2, NULL);
		}

        break;
    }
    case ICO_SYC_EV_WIN_DESTROY:
        printf("--- ICO_SYC_EV_WIN_DESTROY ");
        printf("(appid[%s], name[%s], surface[%d])\n",
               ((ico_syc_win_info_t *)detail)->appid,
               ((ico_syc_win_info_t *)detail)->name,
               ((ico_syc_win_info_t *)detail)->surface);
        break;
    case ICO_SYC_EV_WIN_ACTIVE:
        printf("--- ICO_SYC_EV_WIN_ACTIVE ");
        printf("(appid[%s], name[%s], surface[%d])\n",
               ((ico_syc_win_info_t *)detail)->appid,
               ((ico_syc_win_info_t *)detail)->name,
               ((ico_syc_win_info_t *)detail)->surface);
        break;
    case ICO_SYC_EV_WIN_ATTR_CHANGE:
        ICO_DBG("--- ICO_SYC_EV_WIN_ATTR_CHANGE"
                "(appid[%s], name[%s], surface[%d], node[%d], layer[%d], "
                "(x,y)[%d, %d], width[%d], height[%d], raise[%d], visible[%d], active[%d])",
               ((ico_syc_win_attr_t *)detail)->appid,
               ((ico_syc_win_attr_t *)detail)->name,
               ((ico_syc_win_attr_t *)detail)->surface,
               ((ico_syc_win_attr_t *)detail)->nodeid,
               ((ico_syc_win_attr_t *)detail)->layer,
               ((ico_syc_win_attr_t *)detail)->pos_x,
               ((ico_syc_win_attr_t *)detail)->pos_y,
               ((ico_syc_win_attr_t *)detail)->width,
               ((ico_syc_win_attr_t *)detail)->height,
               ((ico_syc_win_attr_t *)detail)->raise,
               ((ico_syc_win_attr_t *)detail)->visible,
               ((ico_syc_win_attr_t *)detail)->active);
        break;
    case ICO_SYC_EV_THUMB_PREPARE:
        printf("--- ICO_SYC_EV_THUMB_PREPARE\n");
        printf("\t(appid[%s], surface[%d], width[%d], height[%d], ",
               ((ico_syc_thumb_info_t *)detail)->appid,
               ((ico_syc_thumb_info_t *)detail)->surface,
               ((ico_syc_thumb_info_t *)detail)->width,
               ((ico_syc_thumb_info_t *)detail)->height);
        printf("stride[%d], format[%d])\n",
               ((ico_syc_thumb_info_t *)detail)->stride,
               ((ico_syc_thumb_info_t *)detail)->format);
        break;
    case ICO_SYC_EV_THUMB_CHANGE:
        printf("--- ICO_SYC_EV_THUMB_CHANGE\n");
        printf("\t(appid[%s], surface[%d], width[%d], height[%d], ",
               ((ico_syc_thumb_info_t *)detail)->appid,
               ((ico_syc_thumb_info_t *)detail)->surface,
               ((ico_syc_thumb_info_t *)detail)->width,
               ((ico_syc_thumb_info_t *)detail)->height);
        printf("stride[%d], format[%d])\n",
               ((ico_syc_thumb_info_t *)detail)->stride,
               ((ico_syc_thumb_info_t *)detail)->format);
        break;
    case ICO_SYC_EV_THUMB_UNMAP:
        printf("--- ICO_SYC_EV_THUMB_UNMAP\n");
        printf("\t(appid[%s], surface[%d], width[%d], height[%d], ",
               ((ico_syc_thumb_info_t *)detail)->appid,
               ((ico_syc_thumb_info_t *)detail)->surface,
               ((ico_syc_thumb_info_t *)detail)->width,
               ((ico_syc_thumb_info_t *)detail)->height);
        printf("stride[%d], format[%d])\n",
               ((ico_syc_thumb_info_t *)detail)->stride,
               ((ico_syc_thumb_info_t *)detail)->format);
        break;
    case ICO_SYC_EV_LAYER_ATTR_CHANGE:
        printf("--- ICO_SYC_EV_LAYER_ATTR_CHANGE ");
        printf("(layer[%d], visible[%d])\n",
               ((ico_syc_layer_attr_t *)detail)->layer,
               ((ico_syc_layer_attr_t *)detail)->visible);
        break;
    default:
        printf("--- UNKNOWN");
        break;
    }

    return;
}
}

/**
 *  Default Constructor
 */
CicoUISample::CicoUISample()
{
    ICO_DBG("CicoUISample::CicoUISample Enter");
    ICO_DBG("CicoUISample::CicoUISample Leave");
}

/**
 *  Destructor
 */
CicoUISample::~CicoUISample()
{
    ICO_DBG("CicoUISample::~CicoUISample Enter");
    ICO_DBG("CicoUISample::~CicoUISample Leave");
}

bool
CicoUISample::onCreate(void *user_data)
{
    ICO_DBG("CicoUISample::onCreate Enter");

    try {
        ico_syc_connect(_syc_callback, NULL);

        ecore_evas_init();
        int w = 600;
        int h = 800;
        CicoBaseWin* _bgwin = new CicoBaseWin("HSBackground",
										      CicoGeometry(0, 0, 1080, 1920-64-128));
        _background = new CicoBackground(_bgwin->getEvas(),
                                         CicoGeometry(0, 0, 1080, 1920-64-128),
                                         CicoColor(0, 0, 128, 255));

        CicoBaseWin* _ctlwin = new CicoBaseWin("HSControllBar",
                                               CicoGeometry(0, 0, 1080, 128));
        _controlbar = new CicoBackground(_ctlwin->getEvas(),
                                         CicoGeometry(0, 0, 1080, 128),
                                         CicoColor(128, 128, 128, 255));
 
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("catch exception %s", e.what());
        ICO_DBG("CicoUISample::onCreate Leave(false)");
        return false;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("catch exception %s", str.c_str());
        ICO_DBG("CicoUISample::onCreate Leave(false)");
        return false;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        ICO_DBG("CicoUISample::onCreate Leave(false)");
        return false;
    }

    ICO_DBG("CicoUISample::onCreate Leave(true)");

    return true;
}

#if 0
void
CicoUISample::onTerminate(void *user_data)
{
    _DBG("CicoUISample::onTerminate entry");
}

void
CicoUISample::onPause(void *user_data)
{
    _DBG("CicoUISample::onPause entry");
}

void
CicoUISample::onResume(void *user_data)
{
    _DBG("CicoUISample::onResume entry");
}

void
CicoUISample::onService(service_h service, void *user_data)
{
    _DBG("CicoUISample::onService entry");
}
#endif
/* vim: set expandtab ts=4 sw=4: */
