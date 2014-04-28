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
 *  @file   CicoBaseWin.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <string>

#include "CicoBaseWin.h"
#include "CicoLog.h"
//#include "Ecore_Wayland.h"
//#include "Ecore.h"

Ecore_Evas* CicoBaseWin::_window = NULL;

static void
_object_focus_in_cb(void *data,
                    Evas *e,
                    void *event_info)
{
    int x, y, w, h;
    ICO_DBG("#####_object_focus_in_cb() called.");
    /* show window */
    ecore_evas_show(CicoBaseWin::_window);

    ecore_evas_geometry_get (CicoBaseWin::_window, &x, &y, &w, &h);
    ICO_DBG("#####_object_focus_in_cb(x=%d y=%d w=%d h=%d)", x, y, w, h);
}

static void
_object_obj_focus_in_cb(void *data,
                    Evas *e,
                    void *event_info)
{
    ICO_DBG("$$$$$_object_obj_focus_in_cb() called.");
}

/**
 *  Default Constructor
 */
CicoBaseWin::CicoBaseWin(const char *title,
                         const CicoGeometry &geometry)
    : //_window(NULL),
      _geometry(geometry)
{
    ICO_DBG("CicoBaseWin::CicoBaseWin entry");

    /* create window */
    _window = ecore_evas_new(NULL, _geometry._x, _geometry._y,
                             _geometry._w, geometry._h, "frame=0");
    if (NULL == _window) {
        ICO_ERR("ecore_evas_new() failed.");
        throw std::string("ecore_evas_new() failed.");
    }

    /* get display screen size */ 
//    ecore_main_loop_iterate();
//    ecore_wl_screen_size_get(&_width, &_height);
//    _INFO("Display Screen Size(%dx%d)", _width, _height);

    /* set callback on destroy */
    ecore_evas_callback_destroy_set(_window, CicoBaseWin::onDestroy);

    /* setup attribute window */
    /* set title */
    ecore_evas_title_set(_window, title);

    /* enable alpha */
    ecore_evas_alpha_set(_window, EINA_TRUE);

    /* chanage window size */
    ecore_evas_resize(_window, _geometry._w, geometry._h);

    evas_event_callback_add(getEvas(), EVAS_CALLBACK_CANVAS_FOCUS_IN,
                            _object_focus_in_cb, NULL);
    evas_event_callback_add(getEvas(), EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN,
                            _object_obj_focus_in_cb, NULL);

    /* show window */
    ecore_evas_show(_window);

    ICO_DBG("CicoBaseWin::CicoBaseWin exit");
}

/**
 *  Destructor
 */
CicoBaseWin::~CicoBaseWin()
{
    ICO_DBG("CicoBaseWin::~CicoBaseWin entry");
    ICO_DBG("CicoBaseWin::~CicoBaseWin exit");
}

/**
 *  Show
 */
void
CicoBaseWin::show()
{
    ICO_DBG("CicoBaseWin::show entry");
    ecore_evas_show(_window);
    ICO_DBG("CicoBaseWin::show exit");
}

/**
 * 
 */
Ecore_Evas*
CicoBaseWin::getEcoreEvas(void) const
{
    return _window;
}

/**
 * 
 */
Evas*
CicoBaseWin::getEvas(void) const
{
    return ecore_evas_get(_window);
}

/**
 *
 */
void CicoBaseWin::onDestroy(Ecore_Evas *window)
{
}
/* vim: set expandtab ts=4 sw=4: */
