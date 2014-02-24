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
 *  @file   CicoSCLayer.cpp
 *
 *  @brief  This file is implementation of CicoSCLayer class
 */
/*========================================================================*/

#include <stdlib.h>
#include <string.h>
#include "CicoSCLayer.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCLayer::CicoSCLayer()
    : layerid(-1), type(-1), width(-1), height(-1),
      displayid(-1), numsurfaces(0), menuoverlap(false)

{
    surfaceids = (int *)malloc(40 * sizeof(int));
    if (surfaceids) {
        maxsurfaces = 50;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCLayer::~CicoSCLayer()
{
    if (surfaceids) {
        maxsurfaces = 0;
        numsurfaces = 0;
        free(surfaceids);
        surfaceids = NULL;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  addSurface
 */
//--------------------------------------------------------------------------
void
CicoSCLayer::addSurface(int surfaceid, bool top)
{
    int     idx, idx2;
    int     *wksurfaceids;
    ICO_DBG("CicoSCLayer::addSurface(%d,%d)", surfaceid, top);

    idx2 = 0;
    for (idx = 0; idx < numsurfaces; idx++, idx2++) {
        if (surfaceids[idx] == surfaceid)   {
            idx2 --;
        }
        else    {
            surfaceids[idx2] = surfaceids[idx];
        }
    }
    numsurfaces = idx2;

    if (numsurfaces >= maxsurfaces) {
        maxsurfaces += 20;
        wksurfaceids = (int *)malloc(maxsurfaces * sizeof(int));
        if (! wksurfaceids) {
            ICO_ERR("CicoSCLayer::addSurface: Out of Memory");
            return;
        }
        if (surfaceids) {
            memcpy(wksurfaceids, surfaceids, numsurfaces * sizeof(int));
            free(surfaceids);
        }
        surfaceids = wksurfaceids;
    }
    if (top)    {
        surfaceids[numsurfaces] = surfaceid;
    }
    else    {
        for (idx = numsurfaces; idx > 0; idx--) {
            surfaceids[idx] = surfaceids[idx-1];
        }
        surfaceids[0] = surfaceid;
    }
    numsurfaces ++;
}

//--------------------------------------------------------------------------
/**
 *  @brief  removeSurface
 */
//--------------------------------------------------------------------------
void
CicoSCLayer::removeSurface(int surfaceid)
{
    int     idx, idx2;
    ICO_DBG("CicoSCLayer::removeSurface(%d)", surfaceid);

    idx2 = 0;
    for (idx = 0; idx < numsurfaces; idx++, idx2++) {
        if (surfaceids[idx] == surfaceid)   {
            idx2 --;
        }
        else    {
            surfaceids[idx2] = surfaceids[idx];
        }
    }
    numsurfaces = idx2;
}

//--------------------------------------------------------------------------
/**
 *  @brief  getSurfaces
 */
//--------------------------------------------------------------------------
const int *
CicoSCLayer::getSurfaces(int *retsurfaces)
{
    *retsurfaces = numsurfaces;
    return surfaceids;
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCLayer::dump(void)
{
    ICO_DBG("CicoSCLayer: layerid=%d type=%d width=%d height=%d "
            "displayid =%d menuoverlap=%s",
            layerid, type, width, height, displayid,
            menuoverlap ? "true" : "false");
}
// vim:set expandtab ts=4 sw=4:
