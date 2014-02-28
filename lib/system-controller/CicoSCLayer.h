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
 *  @file   CicoSCLayer.h
 *
 *  @brief  This file is definition of CicoSCLayer class
 */
//==========================================================================
#ifndef __CICO_SC_LAYER_H__
#define __CICO_SC_LAYER_H__

#include "CicoCommonDef.h"

//--------------------------------------------------------------------------
/**
 *  @brief  This class hold layer information
 */
//--------------------------------------------------------------------------
class CicoSCLayer
{
public:
    // default constructor
    CicoSCLayer();

    // destructor
    virtual ~CicoSCLayer();

    // surface
    void addSurface(int surfaceid, bool top);
    void removeSurface(int surfaceid);
    const int *getSurfaces(int *retsurfaces);
    bool is_topSurface(int surfaceid);
    bool is_buttomSurface(int surfaceid);

    // dump log this class member variables
    void dump(void);

private:
    // assignment operator
    CicoSCLayer& operator=(const CicoSCLayer &object);

    // copy constructor
    CicoSCLayer(const CicoSCLayer &object);

public:
    int     layerid;      //!< layer id
    int     type;         //!< enum ico_window_mgr_layer_attr
    int     width;        //!< The number of the lateral pixels
    int     height;       //!< The number of vertical pixels
    int     displayid;    //!< display ID

    int     *surfaceids;  //!< surface ID list
    int     numsurfaces;  //!< current number of surfaces
    int     maxsurfaces;  //!< max number of surfaces
    bool    menuoverlap;  //!< overlap menu window flag
};
#endif  // __CICO_SC_LAYER_H__
// vim:set expandtab ts=4 sw=4:
