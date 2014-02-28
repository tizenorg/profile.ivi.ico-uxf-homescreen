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
 *  @file   CicoSCWindow.h
 *
 *  @brief  This file is definition of CicoSCWindow class
 */
//==========================================================================
#ifndef __CICO_SC_WINDOW_H__
#define __CICO_SC_WINDOW_H__

#include <string>

//==========================================================================
//  define macro
//==========================================================================
#define ICO_SURFACEID_2_HOSTID(surfid)      (((unsigned int)surfid) >> 24)
#define ICO_SURFACEID_2_DISPLAYNO(surfid)   (((unsigned int)surfid) >> 16)
#define ICO_SURFACEID_2_NODEID(surfid)      (((unsigned int)surfid) >> 16)
#define ICO_SURFACEID_BASE(nodeid)          (((unsigned int)nodeid) << 16)

//--------------------------------------------------------------------------
/**
 *  @brief This class holds window information
 */
//--------------------------------------------------------------------------
class CicoSCWindow
{
public:
    // default constructor
    CicoSCWindow();

    // destructor
    virtual ~CicoSCWindow();

    // dump log CicoSCWindow member variables
    void dump(void);

private:
    // assignment operator
    CicoSCWindow& operator=(const CicoSCWindow &object);

    // copy constructor
    CicoSCWindow(const CicoSCWindow &object);

public:
    int               surfaceid;    ///< surface id (window id)
    std::string       name;         ///< window name
    std::string       appid;        ///< application id
    int               pid;          ///< process id

    int               nodeid;       ///< node id
    int               displayid;    ///< display id
    int               layerid;      ///< layer id
    std::string       zone;         ///< display zone name
    int               zoneid;       ///< display zone id
    int               subwindow;    ///< main window(0) or sub window(!0)
    unsigned int      eventmask;    ///< event to receive through this window
    int               srcwidth;     ///< width of application frame buffer
    int               srcheight;    ///< height of application frame buffer
    int               x;            ///< upper left X coord of the window
    int               y;            ///< upper left Y coord of the window
    int               width;        ///< width of window
    int               height;       ///< height of window
    bool              visible;      ///< visibility
    bool              raise;        ///< raise/lower
    bool              active;       ///< active/inactive
};
#endif  // __CICO_SC_WINDOW_H__
// vim:set expandtab ts=4 sw=4:
