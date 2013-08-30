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
 *  @file   CicoEvasObject.cpp
 *
 *  @brief  Class implemetation of EvasObject Object
 */
/*========================================================================*/    

#include <exception>
#include <string>

#include "CicoEvasObject.h"
#include "CicoLog.h"
//#include "Ecore_Wayland.h"

/*------------------------------------------------------------------------*/
/**
 *  @brief  Constructor
 *
 *  @param  [IN]    parent      parent of this object
 *  @param  [IN]    geometry    geometry of this object
 */
/*------------------------------------------------------------------------*/
CicoEvasObject::CicoEvasObject(const Evas *parent,
                               CicoEvasObject::ObjType type,
                               const CicoGeometry &geometry)
    : _parent((Evas*)parent),
      _object(NULL),
      _geometry(geometry)
{
    ICO_DBG("CicoEvasObject::CicoEvasObject Entry");

    if (NULL == _parent) {
        ICO_ERR("Invalid argument: _window is NULL");
        throw std::string("_parent is NULL.");
    }

    /* create object */
    switch (type) {
    case OBJTYPE_RECTANGLE:
        _object = evas_object_rectangle_add(_parent);
        break;
    case OBJTYPE_POLYGON:
        _object = evas_object_polygon_add(_parent);
        break;
    case OBJTYPE_LINE:
        _object = evas_object_line_add(_parent);
        break;
    case OBJTYPE_IMAGE:
        _object = evas_object_image_add(_parent);
        break;
    case OBJTYPE_FILLED_IMAGE:
        _object = evas_object_image_filled_add(_parent);
        break;
    case OBJTYPE_TEXT:
        _object = evas_object_text_add(_parent);
        break;
    case OBJTYPE_TEXTBLOCK:
        _object = evas_object_textblock_add(_parent);
        break;
    default:
        _object = NULL;
        break;
    }

    if (NULL == _object) {
        ICO_ERR("create evas object failed. objtype=%d", type);
        throw std::string("create evas object failed.");
    }

    /* setup attribute background object */
    /* set object position */
    move(geometry._x, geometry._y);

    /* set object size */
    resize(geometry._w, geometry._h);

    ICO_DBG("CicoEvasObject::CicoEvasObject exit");
}

/**
 *  Destructor
 */
CicoEvasObject::~CicoEvasObject()
{
    ICO_DBG("CicoEvasObject::~CicoEvasObject entry");
    if (NULL != _object) {
        evas_object_del(_object);
        _object = NULL;
    }
    ICO_DBG("CicoEvasObject::~CicoEvasObject exit");
}

/**
 *  Show
 */
void
CicoEvasObject::show()
{
    ICO_DBG("CicoEvasObject::show entry");
    evas_object_show(_object);
    ICO_DBG("CicoEvasObject::show exit");
}

/**
 *  Hide Background
 */
void
CicoEvasObject::hide()
{
    ICO_DBG("CicoEvasObject::hide entry");
    evas_object_hide(_object);
    ICO_DBG("CicoEvasObject::hide exit");
}


/**
 *  Get visible state
 */
bool
CicoEvasObject::isVisible(void)
{
    return evas_object_visible_get(_object);
}

/**
 *  Move Background
 */
void
CicoEvasObject::move(int x, int y)
{
    ICO_DBG("CicoEvasObject::move entry");
    evas_object_move(_object, x, y);
    ICO_DBG("CicoEvasObject::move exit");
}

/**
 *  Move Background
 */
void
CicoEvasObject::resize(int w, int h)
{
    ICO_DBG("CicoEvasObject::resize entry");
    evas_object_resize(_object, w, h);
    ICO_DBG("CicoEvasObject::resize exit");
}

/**
 *  Get geometry of this object
 */
const CicoGeometry&
CicoEvasObject::getGeometry(void)
{
    ICO_DBG("CicoEvasObject::getGeometryentry entry");
    evas_object_geometry_get(_object,
                             &_geometry._x, &_geometry._y, 
                             &_geometry._w, &_geometry._h);
    ICO_DBG("CicoEvasObject::getGeometryentry exit");
    return _geometry;
}

/* Get this evas object */
Evas_Object*
CicoEvasObject::getObject(void) const
{
    return _object; 
}

void
CicoEvasObject::setClip(Evas_Object *clip)
{
    ICO_DBG("CicoEvasObject::getGeometryentry entry");
    evas_object_clip_set(_object, clip);
    ICO_DBG("CicoEvasObject::getGeometryentry exit");
}

/* Set this evas object */
void
CicoEvasObject::event_cb(void *data, Evas *evas, 
                         Evas_Object *obj, void *event_info)
{
    ICO_DBG("CicoEvasObject::event_cb entry");
    int *type = (int*)data;
    ICO_DBG("event_type = %d", *type);
    ICO_DBG("CicoEvasObject::event_cb exit");
}
/* vim: set expandtab ts=4 sw=4: */
