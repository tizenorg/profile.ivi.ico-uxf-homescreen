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
 *  @file   CicoSCDisplay.h
 *
 *  @brief  definition of CicoSCDisplay class
 */
//==========================================================================
#ifndef __CICO_SC_DISPLAY_H__
#define __CICO_SC_DISPLAY_H__

#include "wayland-client.h"

#include <string>
#include <vector>

using namespace std;

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCLayer;
class CicoSCDisplayZone;

//==========================================================================
/**
 *  @brief  display information
 */
//==========================================================================
class CicoSCDisplay
{
public:
    // default constructor
    CicoSCDisplay();

    // destructor
    ~CicoSCDisplay();

    // dump log CicoSCDisplay member variables
    void dump(void);

private:
    // assignment operator
    CicoSCDisplay& operator=(const CicoSCDisplay &object);

    // copy constructor
    CicoSCDisplay(const CicoSCDisplay &object);

public:
    // definition value of horizontal
    const static int ORIENTATION_HORIZONTAL = 0;    
    // definition value of vartical
    const static int ORIENTATION_VERTICAL   = 1;

    int    displayid;   //!< Display ID
    int    type;        //!< Type
    int    nodeid;      //!< Host ID
    int    displayno;   //!< Consecutive numbers
    int    width;       //!< The number of the lateral pixels
    int    height;      //!< The number of vertical pixels
    int    inch;        //!< Screen physical size(0.1 inches of units)
    int    pWidth;      //!< compositor width
    int    pHeight;     //!< compositor height
    int    orientation; //!< screen orientation
    string name;        //!< Display name
 
    vector<CicoSCLayer*> layerList;      //!< list of display layer
    vector<CicoSCDisplayZone*> zoneList; //!< list of display zone
};
#endif	// __CICO_SC_DISPLAY_H__
// vim:set expandtab ts=4 sw=4:
