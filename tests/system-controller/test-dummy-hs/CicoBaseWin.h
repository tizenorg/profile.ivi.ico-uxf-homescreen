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
 *  @file   CicoBaseWin.h
 *
 *  @brief  
 */
/*========================================================================*/    

#include <stdio.h>
#include <Ecore_Evas.h>

#include "CicoGeometry.h"

#ifndef __CICO_BASE_WIN_H__
#define __CICO_BASE_WIN_H__

#ifdef __cplusplus
extern "C" {
#endif

class CicoBaseWin {
public:
    // Default Constructor
    CicoBaseWin(const char *title,
                const CicoGeometry &geometry);
    
    // Destructor
    virtual ~CicoBaseWin();

    // Show Window
    void show(void);

    // Get instance of ecore_evas
    Ecore_Evas* getEcoreEvas(void) const;

    // Get instance of evas
    Evas* getEvas(void) const;

protected:
    // Default Constructor
    CicoBaseWin();
 
    // Assignment Operator
    CicoBaseWin& operator=(const CicoBaseWin &object);

    // Copy Constructor
    CicoBaseWin(const CicoBaseWin &object);

private:
    static void onDestroy(Ecore_Evas *window);


public:
    static Ecore_Evas *_window;
private:

    CicoGeometry _geometry;

//    CicoBackground *_background;

//    CicoImage *_bgImage;
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_BASE_WIN_H__ */
/* vim:set expandtab ts=4 sw=4: */
