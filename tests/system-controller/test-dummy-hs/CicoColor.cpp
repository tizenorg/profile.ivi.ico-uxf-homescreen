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
 *  @file   CicoColor.cpp
 *
 *  @brief  Color Implementaion
 */
/*========================================================================*/    

#include "CicoColor.h"

/**
 *  Default Constructor
 */
CicoColor::CicoColor(int r, int g, int b, int a)
    : _r(r), _g(g), _b(b), _a(a)
{
}

/**
 *  Copy Constructor
 */
CicoColor::CicoColor(const CicoColor &object)
{
    _r = object._r;
    _g = object._g;
    _b = object._b;
    _a = object._a;
}


/**
 *  Destructor
 */
CicoColor::~CicoColor()
{
}

/**
 *  Assignment Operator
 */
CicoColor&
CicoColor::operator=(const CicoColor& object)
{
    _r = object._r;
    _g = object._g;
    _b = object._b;
    _a = object._a;

    return *this;
}
/* vim: set expandtab ts=4 sw=4: */
