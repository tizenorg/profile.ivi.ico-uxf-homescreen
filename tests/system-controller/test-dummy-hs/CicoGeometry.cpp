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
 *  @file   CicoGeometry.cpp
 *
 *  @brief  Geometry Implementaion
 */
/*========================================================================*/    

#include "CicoGeometry.h"

/**
 *  Default Constructor
 */
CicoGeometry::CicoGeometry(int x, int y, int w, int h)
    : _x(x), _y(y), _w(w), _h(h)
{
}

/**
 *  Copy Constructor
 */
CicoGeometry::CicoGeometry(const CicoGeometry& object)
{
    _x = object._x;
    _y = object._y;
    _w = object._w;
    _h = object._h;
}


/**
 *  Destructor
 */
CicoGeometry::~CicoGeometry()
{
}

/**
 *  Assignment Operator
 */
CicoGeometry&
CicoGeometry::operator=(const CicoGeometry& object)
{
    _x = object._x;
    _y = object._y;
    _w = object._w;
    _h = object._h;

    return *this;
}
/* vim: set expandtab ts=4 sw=4: */
