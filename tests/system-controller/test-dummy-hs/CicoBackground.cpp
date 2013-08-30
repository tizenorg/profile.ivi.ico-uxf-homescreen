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
 *  @file   CicoBackground.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <string>

#include "CicoBackground.h"
#include "CicoLog.h"

/**
 *  Default Constructor
 */
CicoBackground::CicoBackground(const Evas         *parent,
                               const CicoGeometry &geometry,
                               const CicoColor    &color)
    : CicoEvasObject(parent, CicoEvasObject::OBJTYPE_RECTANGLE, geometry),
      _color(color)
{
    ICO_DBG("CicoBackground::CicoBackground entry");

    /* setup attribute background object */
    /* set color */
    setColor(color);

    /* show object */
    show();

    ICO_DBG("CicoBackground::CicoBackground exit");
}

/**
 *  Destructor
 */
CicoBackground::~CicoBackground()
{
    ICO_DBG("CicoBackground::~CicoBackground entry");
    ICO_DBG("CicoBackground::~CicoBackground exit");
}

/**
 *  Set background color
 */
void
CicoBackground::setColor(const CicoColor &color)
{
    ICO_DBG("CicoBackground::setColor entry");
    _color = color;
    ICO_DBG("color(R:%03d G:%03d B:%03d A:%03d)",
         _color._r, _color._g,
         _color._b, _color._a);
    evas_object_color_set(_object,
                          _color._r, _color._g,
                          _color._b, _color._a);
    ICO_DBG("CicoBackground::setColor exit");
}
/* vim: set expandtab ts=4 sw=4: */
