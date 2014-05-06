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
 *  @file   CicoCommonWindow.h
 *
 *  @brief  This file is definition of CicoCommonWindow class
 */
//==========================================================================
#ifndef __CICO_COMMON_WINDOW_H__
#define __CICO_COMMON_WINDOW_H__

#include <Ecore_Evas.h>
#include <Evas.h>
#include <list>

//==========================================================================
/**
 *  @brief  This class provide the common window interfaces
 */
//==========================================================================
class CicoCommonWindow
{
public:
    // default constructor
    CicoCommonWindow();

    // destructor
    virtual ~CicoCommonWindow();

    // terminate the window
    virtual bool Terminate(void);

    // show the window
    virtual void Show(void);

    // hide the window
    virtual void Hide(void);

    // set position of the window
    virtual void SetPos(int x, int y);

    // set size of the window
    virtual void SetSize(int w, int h);

protected:

    Ecore_Evas *window_;        /// main window
    Evas_Object *windowobj_;    /// background
    int posx_;                  /// position x
    int posy_;                  /// position y
    int width_;                 /// window width
    int height_;                /// window height
};
#endif  // __CICO_COMMON_WINDOW_H__
// vim: set expandtab ts=4 sw=4:
