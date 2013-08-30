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
 *  @file   CicoSCDisplayZone.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_DISPLAYZONE_H__
#define __CICO_SC_DISPLAYZONE_H__

//==========================================================================
/**
 *  @brief  display zone information
 */
//==========================================================================
class CicoSCDisplayZone
{
public:
    // default constructor
    CicoSCDisplayZone();

    // destructor
    virtual ~CicoSCDisplayZone();

    // dump log CicoSCDisplay member variables
    void dump(void);

private:
    // assignment operator
    CicoSCDisplayZone& operator=(const CicoSCDisplayZone &object);

    // copy constructor
    CicoSCDisplayZone(const CicoSCDisplayZone &object);

public:
    int zoneid;  //!< layer ID
    int x;       //!< display zone x position
    int y;       //!< display zone y position
    int width;   //!< display zone width
    int height;  //!< display zone height
};
#endif	// __CICO_SC_DISPLAYZONE_H__
// vim:set expandtab ts=4 sw=4:
