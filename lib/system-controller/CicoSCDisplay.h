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
 *  @brief  This file is definition of CicoSCDisplay class
 */
//==========================================================================
#ifndef __CICO_SC_DISPLAY_H__
#define __CICO_SC_DISPLAY_H__

#include <string>
#include <vector>
#include <map>

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCLayer;
class CicoSCDisplayZone;

//==========================================================================
/**
 *  @brief  This class hold display information
 */
//==========================================================================
class CicoSCDisplay
{
public:
    // default constructor
    CicoSCDisplay();

    // destructor
    ~CicoSCDisplay();

    // dump log this class member variables
    void dump(void);

    // find display zone
    CicoSCDisplayZone* findDisplayZonebyFullName(const string & fullname);

private:
    /// assignment operator
    CicoSCDisplay& operator=(const CicoSCDisplay &object);

    /// copy constructor
    CicoSCDisplay(const CicoSCDisplay &object);

public:
    ///< definition value of horizontal
    const static int ORIENTATION_HORIZONTAL = 0;
    ///< definition value of vertical
    const static int ORIENTATION_VERTICAL   = 1;

    int         displayid;   ///< display id
    int         type;        ///< display type
    int         nodeid;      ///< node id
    int         displayno;   ///< consecutive numbers
    int         width;       ///< the number of the lateral pixels
    int         height;      ///< the number of vertical pixels
    int         pWidth;      ///< compositor width
    int         pHeight;     ///< compositor height
    int         orientation; ///< screen orientation
    std::string name;        ///< display name

    /// list of display layer
    std::vector<CicoSCLayer*> layerList;

    /// list of display zone
    std::map<unsigned int, CicoSCDisplayZone*> zoneList;
};
#endif  // __CICO_SC_DISPLAY_H__
// vim:set expandtab ts=4 sw=4:
